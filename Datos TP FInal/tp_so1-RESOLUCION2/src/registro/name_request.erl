%%--------------------------------------------------------------------
%% @doc
%% Módulo encargado del protocolo de registro de nodos en la red P2P.
%%
%% Este módulo implementa el mecanismo de consenso de nombres entre
%% nodos al ingresar a la red, evitando colisiones de IDs.
%%
%% Funcionalidades:
%%   - Envía un mensaje `NAME_REQUEST <ID>` por broadcast UDP.
%%   - Espera durante un tiempo limitado una respuesta `INVALID_NAME`.
%%   - Si recibe una respuesta de conflicto, vuelve a intentar con otro ID.
%%   - Si no hay respuesta (timeout), asume el ID como válido y se registra.
%%   - Inicia el broadcasting periódico de HELLO y el listener UDP.
%%--------------------------------------------------------------------
-module(name_request).

%% Exportamos las funciones a usar desde otros módulos o tests.
-export([start/0, encode/1, decode/1, start_request/1]).

%% Incluir los records necesarios
-include("../header/records.hrl").

%% Constantes
-define(PORT, 12346). %% Puerto UDP compartido para broadcast
-define(TCP_PORT, 12345). %% Puerto TCP para conexiones entrantes
-define(TIMEOUT_MS, 10000). %% Tiempo de espera por una respuesta

%%--------------------------------------------------------------------
%% @doc
%% Inicia el proceso de registro del nodo en la red.
%%
%% Abre un socket UDP en modo activo para enviar y recibir mensajes,
%% y spawnea un proceso que hace la solicitud de nombre.
%%
%% @spec start() -> pid()
%%-------------------------------------------------------------------
start() ->
  Pid = spawn(fun() ->
    {ok, Socket} = gen_udp:open(0, [binary,{reuseaddr, true},{broadcast, true},{active, true}]),
    start_request(Socket)
  end),
  Pid.

%%--------------------------------------------------------------------
%% @doc
%% Envia un NAME_REQUEST con un ID generado aleatoriamente y espera una
%% posible respuesta `INVALID_NAME`.
%%
%% Si hay conflicto, reintenta con otro ID. Si no hay respuesta,
%% asume que el ID fue aceptado.
%%
%% @spec start_request(Socket :: port()) -> ok
%%--------------------------------------------------------------------
start_request(Socket) ->
    Id= gen_id(),
    %Id = "aaaa",
    Msg = encode(Id),
    %% Enviamos el mensaje a todos los nodos
    gen_udp:send(Socket, {255,255,255,255}, ?PORT, Msg),
    %% Esperamos una posible respuesta INVALID_NAME
    receive
        {udp, _Socket, Ip, InPortNo, Bin} ->
            case decode(Bin) of
                {ok, #invalid_name{id=_}}->
                    %% Recibimos INVALID_NAME para nuestro ID, reintentar
                    io:format("Invalid name. Reintentando...~n"),
                    Delay = rand:uniform(9) + 1,  %% 2-10s
                    timer:sleep(Delay * 1000),
                    start_request(Socket);
                {ok, #name_request{id=RecvId}} when RecvId=:=Id->
                    io:format("Recibí un name request con mi id: ~p~n", RecvId),
                    Msg = lists:flatten(io_lib:format("INVALID_NAME ~s~n", [Id])),
                    gen_udp:send(Socket, Ip, InPortNo, Msg),
                    start_request(Socket);
                _ ->
                    %% Recibimos otra cosa (u otro ID)
                    ok
            end
        after ?TIMEOUT_MS ->
            %% Nadie respondió con INVALID_NAME → consideramos válido
            fallback_or_success(Socket, ?TCP_PORT, Id)
    end.

%%--------------------------------------------------------------------
%% @doc
%% Lógica post-registro: inicia la difusión periódica de HELLO y el
%% listener UDP para recibir mensajes entrantes.
%%
%% @spec fallback_or_success(Socket :: port(), TcpPort :: integer(), Id :: string()) -> string()
%%-------------------------------------------------------------------
fallback_or_success(Socket, TcpPort, Id) ->
    self_id:set(Id),
    hello:start_broadcaster(Id, TcpPort, Socket),
    udp_listener:start(),
    Id.

%%--------------------------------------------------------------------
%% @doc
%% Codifica un mensaje NAME_REQUEST.
%%
%% @spec encode(Id :: string()) -> string()
%%--------------------------------------------------------------------
encode(Id) when is_list(Id) ->
    lists:flatten(io_lib:format("NAME_REQUEST ~s~n", [Id])).

%%--------------------------------------------------------------------
%% @doc
%% Decodifica un mensaje NAME_REQUEST.
%%
%% @spec decode(Binary :: string()) ->
%%           {ok, #name_request{}} | {error, invalid_format}
%%--------------------------------------------------------------------
decode(Bin) when is_binary(Bin) ->
    decode(binary_to_list(Bin));  % transforma a lista y reusa la cláusula siguiente
decode(Str) when is_list(Str) ->
    case string:tokens(string:trim(Str), " ") of
        ["INVALID_NAME", RecvId] ->
            {ok, #invalid_name{id = RecvId}};
        ["NAME_REQUEST", RecvId]->
            io:format("Recibí un name_request~n"),
            {ok, #name_request{id = RecvId}};
        _ ->
            {error, invalid_format}
    end.


%%--------------------------------------------------------------------
%% @doc
%% Genera un ID aleatorio de 4 caracteres alfanuméricos.
%%
%% @spec gen_id() -> string()
%%--------------------------------------------------------------------
-spec gen_id() -> string().
gen_id() ->
    lists:flatten([random_char() || _ <- lists:seq(1,4)]).

%%--------------------------------------------------------------------
%% @doc
%% Devuelve un carácter alfanumérico aleatorio.
%%
%% Alfanuméricos:
%% - 0–9: códigos ASCII 48–57
%% - A–Z: 65–90
%% - a–z: 97–122
%%
%% @spec random_char() -> integer()
%%--------------------------------------------------------------------
random_char() ->
    Code = rand:uniform( (57-48+1) + (90-65+1) + (122-97+1) ),
    case Code of
        N when N =< 10 -> 48 + N - 1;
        N when N =< 10 + 26 -> 65 + N - 11;
        N -> 97 + N - 10 - 26 - 1
    end.

