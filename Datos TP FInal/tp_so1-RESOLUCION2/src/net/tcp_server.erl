-module(tcp_server).
-export([start_link/1]).
%% @doc
%% Inicia el servidor TCP y lo pone a escuchar en el puerto especificado.
%%
%% @spec start_link(integer()) -> {ok, pid()}
%% @param Port El puerto en el cual el servidor debe escuchar conexiones.
%% @return Una tupla {ok, Pid} donde Pid es el identificador del proceso creado.
start_link(Port) ->
    Pid = spawn_link(fun() -> listen(Port) end),
    {ok, Pid}.

%% @doc
%% Crea un socket de escucha en el puerto dado con opciones adecuadas
%% para recibir líneas en binario. Llama a accept_loop/1 para iniciar
%% la aceptación de conexiones.
%%
%% @spec listen(integer()) -> ok
listen(Port) ->
    {ok, LSock} = gen_tcp:listen(
                    Port,
                    [binary, {packet, line}, {active, false},
                     {reuseaddr, true}, {ip, {0,0,0,0}}]),
    io:format("TCP server escuchando en ~p~n", [Port]),
    accept_loop(LSock).

%% @doc
%% Bucle principal de aceptación de conexiones. Espera una conexión TCP,
%% y al recibirla lanza un proceso separado que maneja esa conexión
%% mediante handle/1. Luego continúa aceptando nuevas conexiones.
%%
%% @spec accept_loop(listen_socket()) -> ok
accept_loop(LSock) ->
    {ok, Sock} = gen_tcp:accept(LSock), %Bloquea hasta que llega una conexión entrante, devuelve Sock
    spawn(fun() -> handle(Sock) end),   % proceso por conexión
    accept_loop(LSock).

%% @doc
%% Maneja una conexión TCP individual. Espera una línea del cliente,
%% la analiza, y despacha según el tipo de solicitud. Se reconocen
%% comandos que comienzan con "SEARCH_REQUEST " o "DOWNLOAD_REQUEST ".
%% Cualquier otra entrada se ignora y se cierra la conexión.
%%
%% @spec handle(socket()) -> ok
handle(Sock) ->
    %% io:format("Aceptando TCP~n"),
    case gen_tcp:recv(Sock, 0) of
        {ok, BinLine} ->
            %io:format("Request: ~p~n", BinLine),
            case BinLine of
                <<"SEARCH_REQUEST ", _/binary>> ->
                    search_handler:process(BinLine, Sock); %Llamo al handler de solicitud de archivos
                <<"DOWNLOAD_REQUEST ", _/binary>> ->
                    %% io:format("Request de descarga recibida~n"),
                    download_handler:process(BinLine, Sock);
                _ ->
                    gen_tcp:close(Sock)
            end;
        {error, closed} ->
            io:format("Error al manejar la solicitud~n>"),
            ok
    end.
