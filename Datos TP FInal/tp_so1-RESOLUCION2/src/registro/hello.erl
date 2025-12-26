%%--------------------------------------------------------------------
%% @doc
%% Módulo para manejo de mensajes HELLO en la red P2P.
%%
%% Este módulo:
%%   - Codifica y decodifica mensajes `HELLO`.
%%   - Lanza un proceso periódico que transmite mensajes HELLO por UDP
%%     para anunciar la presencia del nodo en la red.
%%
%%--------------------------------------------------------------------
-module(hello).

%% Exportamos las funciones a usar desde otros módulos o tests.
-export([encode/2, decode/1, start_broadcaster/3]).

%% Incluir los records necesarios
-include("../header/records.hrl").

%%--------------------------------------------------------------------
%% @doc
%% Codifica un mensaje HELLO con el ID del nodo y su puerto TCP.
%%
%% @spec encode(Id :: string(), TcpPort :: integer()) -> string().
%%
%% Devuelve un string con el formato:
%%     "HELLO <Id> <Puerto>\n"
%%--------------------------------------------------------------------
encode(Id, TcpPort) when is_list(Id), is_integer(TcpPort) ->
    Message = io_lib:format("HELLO ~s ~p~n", [Id, TcpPort]),
    lists:flatten(Message).

%%--------------------------------------------------------------------
%% @doc
%% Decodifica un mensaje HELLO recibido.
%%
%% @spec decode(Binary :: string()) ->
%%          {ok, #hello{}} | {error, invalid_port | invalid_format}.
%%
%% Extrae el ID y el puerto del mensaje y los convierte en un record `#hello{}`.
%%--------------------------------------------------------------------
decode(Binary) when is_list(Binary) ->
    case string:tokens(string:trim(Binary), " \t") of
        ["HELLO", Id, PortStr] ->
            case string:to_integer(PortStr) of
                {IntPort, _} -> {ok, #hello{id=Id, port=IntPort}};
                _ -> {error, invalid_port}
            end;
        _ -> {error, invalid_format}
    end.

%%--------------------------------------------------------------------
%% @doc
%% Inicia un proceso que transmite periódicamente un mensaje HELLO.
%%
%% @spec start_broadcaster(Id :: string(), TcpPort :: integer(), Socket :: port()) -> ok.
%%
%% El mensaje se envía a la dirección de broadcast 255.255.255.255:12346
%% cada 15 a 20 segundos (aleatorio).
%%--------------------------------------------------------------------
start_broadcaster(Id, TcpPort, Socket) ->
    spawn(fun() -> loop(Id, TcpPort, Socket) end),
    ok.

%%--------------------------------------------------------------------
%% @private
%% Bucle del proceso que envía periódicamente el mensaje HELLO.
%%
%% Cada iteración:
%%   - Codifica y envía el mensaje HELLO por UDP.
%%   - Espera un tiempo aleatorio entre 15 y 20 segundos.
%%   - Repite el proceso.
%%--------------------------------------------------------------------
loop(Id, TcpPort, Socket) ->
    Bin = encode(Id, TcpPort),
    %% Enviar a todos los nodos: dirección de broadcast
    gen_udp:send(Socket, {255,255,255,255}, 12346, Bin),
    Timer = rand:uniform(1) + 1,
    %% original: Timer = rand:uniform(6) + 14,  %% entre 15 y 20 segunos
    timer:sleep(Timer * 1000),
    loop(Id, TcpPort, Socket).