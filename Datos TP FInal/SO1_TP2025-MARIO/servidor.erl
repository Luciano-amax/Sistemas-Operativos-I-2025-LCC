% ---------------------------------------------------------------
% Modulo "servidor" de conexiones TCP (SEARCH, DOWNLOAD, CHUNKS).
% ---------------------------------------------------------------

-module(servidor).
-export([inicio/1, escuchar_cliente/1, handler_occurrence/2, enviar_archivo/3, enviar_chunks/3]).
-include("config.hrl").

inicio(NodoID) -> 
    escuchar_cliente(NodoID).

% Servidor en escucha de clientes.
escuchar_cliente(NodoID) ->
    case gen_tcp:listen(?PUERTO_TCP, [{active, true}, {reuseaddr, true}]) of 
        {ok, ListenSocket} ->
            io:format("Servidor esperando conexiones...Completo~n"),
            esperar_cliente(ListenSocket, NodoID);
        {error, Reason} -> 
            io:format("Error de connexión S01: ~p~n", [Reason]),
            escuchar_cliente(NodoID)
    end.


% Conección con clientes.
esperar_cliente(ListenSocket, NodoID) ->
    case gen_tcp:accept(ListenSocket) of
    {ok, Socket} ->
        Pid = spawn(?MODULE, handler_occurrence, [Socket, NodoID]),
        gen_tcp:controlling_process(Socket, Pid),
        esperar_cliente(ListenSocket, NodoID);
    {error, Reason} -> 
        io:format("Error de conexión S02: ~p~n", [Reason]),
        esperar_cliente(ListenSocket, NodoID)
    end. 

% Recibir mensajes de clientes.
handler_occurrence(Socket, NodoID) ->
    receive
        {tcp,Socket,Msg} ->
            StringTrim = string:trim(Msg),
            ToList = string:tokens(StringTrim, " "),
            
            case ToList of
                ["DOWNLOAD_REQUEST", ArchivoNombre] ->
                    {Result, File, Size} = archivos:buscar_archivo(ArchivoNombre),
                    if 
                        Result == error -> 
                            Response = <<112>>,
                            case gen_tcp:send(Socket, Response) of
                                ok -> ok;
                                {error, Reason} -> 
                                io:format("Error de envío S03: ~p~n", [Reason])
                            end;
                        Result == ok ->
                            enviar_archivo(Socket,File,Size)
                    end;
                ["SEARCH_REQUEST", _ ,Wildcard] ->
                    ArchivosEncontrados = archivos:listar_por_wildcard(Wildcard), 
                    lists:foreach(
                        fun({Archivo,Tamaño}) ->
                            Response = io_lib:format("SEARCH_RESPONSE ~s ~s ~p~n",[NodoID, Archivo, Tamaño]),
                            case gen_tcp:send(Socket, Response) of
                                ok -> 
                                    ok;
                                {error, Reason} -> 
                                    io:format("Error de envío S04: ~p~n", [Reason])
                            end
                        end
                    ,ArchivosEncontrados);
                _ -> 
                    io:format("Error de mensaje no reconocido S06: ~p~n", [Msg])
            end;
       {tcp_closed, Socket} ->
            io:format("Cliente desconectado.~n"),
            ok;
        {tcp_error, Socket, Reason} ->
            io:format("Error TCP S05: ~p~n", [Reason]),
            ok
    end,
    timer:sleep(2000),
    gen_tcp:close(Socket).
    

% Envia archivos.
enviar_archivo(Socket,File,Size) ->
    MB = 1024 * 1024,
    Code = <<101>>,
    BinarySize = <<Size:32/integer-big>>,
    case (Size >= MB*4) of
        true ->
            Msg = <<Code/binary, BinarySize/binary, MB:32/integer-big>>,
            case gen_tcp:send(Socket, Msg) of
                ok -> enviar_chunks(Socket,File,0);
                {error, Reason} -> io:format("Error de envío S07: ~p~n", [Reason])
            end;
        false->
            Msg = <<Code/binary, BinarySize/binary, File/binary>>,
            case gen_tcp:send(Socket, Msg) of
                ok -> ok;
                {error, Reason} -> io:format("Error de envío S08: ~p~n", [Reason])
            end
    end.
            
% Envia chunks.
enviar_chunks(Socket,File,N) ->
    SizeChunk = 1024 * 1024,
    ActualSize = erlang:byte_size(File),
    case ActualSize >= SizeChunk of
        true ->
            <<Chunk:SizeChunk/binary, Rest/binary>> = File,
            Msg = <<111, N:16/integer-big , SizeChunk:32/integer-big, Chunk/binary>>,
            case gen_tcp:send(Socket, Msg) of
                ok ->
                    M = N + 1,
                    enviar_chunks(Socket,Rest,M);
                {error, Reason} -> io:format("Error de envío S09: ~p~n", [Reason])
            end;
        false ->
            if 
                File /= <<>> ->
                    Msg = <<111, N:16/integer-big , ActualSize:32/integer-big, File/binary>>,
                    case gen_tcp:send(Socket, Msg) of
                        ok -> ok;
                        {error, Reason} -> io:format("Error de envío S10: ~p~n", [Reason])
                    end;
                true -> ok
            end
    end.
