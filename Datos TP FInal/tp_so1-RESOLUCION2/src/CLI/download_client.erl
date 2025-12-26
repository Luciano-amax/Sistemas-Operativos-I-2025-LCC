
-module(download_client).
-export([download_from_node/2]).
-define(CODE_NOTFOUND, 112).
-define(CODE_SINGLE, 101).
-define(CODE_CHUNK, 111).
-define(MAX_CHUNK, 4*1024*1024).
%%--------------------------------------------------------------------
%% @doc
%% Descarga un archivo desde un nodo especifico
%%--------------------------------------------------------------------
download_from_node(FileName, NodeId) ->
    %% Obtener información del nodo desde el registry
    case get_node_info(NodeId) of
        {ok, {Ip, Port}} ->
            io:format("Conectando a nodo ~s (~p:~p)...~n", [NodeId, Ip, Port]),
            download_file_from_node(FileName, NodeId, Ip, Port);
        {error, not_found} ->
            io:format("Error: Nodo '~s' no encontrado en el registro~n>", [NodeId]);
        {error, Reason} ->
            io:format("Error al obtener información del nodo: ~p~n>", [Reason])
    end.

%%--------------------------------------------------------------------
%% @doc
%% Obtiene la IP y puerto de un nodo desde el registry
%%--------------------------------------------------------------------
get_node_info(NodeId) ->
    node_registry ! {get_all_nodes, self()},
    receive
        Nodes when is_list(Nodes) ->
            case lists:keyfind(NodeId, 1, Nodes) of
                {NodeId, Ip, Port} -> {ok, {Ip, Port}};
                false -> {error, not_found}
            end;
        _Other ->
            {error, registry_error}
    after 1000 ->
        {error, timeout}
    end.

%%--------------------------------------------------------------------
%% @doc
%% Realiza la descarga del archivo desde el nodo especificado
%%--------------------------------------------------------------------
download_file_from_node(FileName, NodeId, Ip, Port) ->
    PortValue = Port,
    {ok, Socket} = gen_tcp:connect(Ip, PortValue, [binary, {active, false}],5000),
    %% Construir y enviar solicitud
    FileNameBinary = list_to_binary(FileName),
    NodeIdBinary = list_to_binary(NodeId),
    Req = iolist_to_binary(["DOWNLOAD_REQUEST ", FileNameBinary, " ", NodeIdBinary, "\n"]),
    gen_tcp:send(Socket, Req),
    spawn(fun()->receive_file(Socket, FileName) end),
    io:format(">> enviados ~p bytes: ~p~n", [byte_size(Req), Req]).
    
receive_file(Socket, FileName) ->
    %% Leer primer byte para código
    <<Code:8>> = recv_exact(Socket, 1),
    io:format("Recibiendo archivo. Codigo: ~p~n", [Code]),

    case Code of
        112 -> 
            %% Si el código es 112, significa que el archivo no fue encontrado
            io:format("Error: Archivo no encontrado en el nodo.~n>"),
            {error, not_found};

        ?CODE_SINGLE -> 
            %% Si es código 101 (archivos completos), manejamos como archivo único
            handle_single(Socket, FileName);

        _ -> 
            %% Si recibimos un código inesperado, reportamos error
            io:format("Codigo inesperado recibido: ~p~n>", [Code]),
            {error, unexpected_code}
    end.

%%  Maneja 101 OK  → puede ser “single” (≤ 4 MB) o “chunked” (> 4 MB)
handle_single(Socket, FileName) ->
    %% Leemos SIEMPRE 4 bytes: tamaño total del archivo
    <<Total:32/big>> = recv_exact(Socket, 4),

    if  Total =< ?MAX_CHUNK ->
            %% -------  MODO SINGLE  -------
            io:format("Modo SINGLE. Tamaño total: ~p bytes~n", [Total]),
            Data = recv_exact(Socket, Total),
            Target = filename:join("descargas", FileName),
            file:write_file(Target, Data),
            io:format("Archivo guardado en ~s (~p bytes).~n>", [Target, Total]),
            ok;

        true ->
            %% -------  MODO CHUNKED  -------
            %% Leemos 4 bytes extra con el tamaño estándar del chunk
            <<StdChunk:32/big>> = recv_exact(Socket, 4),
            io:format("Modo CHUNKED. Total: ~p, stdChunk: ~p~n>", [Total, StdChunk]),
            gather_chunks(Socket, Total, [], FileName)
    end.


%%  Recibe los mensajes 111 <idx:16> <len:32> <data…>
gather_chunks(_, Remaining, Acc, FileName) when Remaining =< 0 ->
    assemble_chunks(Acc, FileName);
gather_chunks(Socket, Remaining, Acc, FileName) ->
    %% Leemos código del mensaje
    <<?CODE_CHUNK:8>> = recv_exact(Socket, 1),
    %% Metadatos del chunk
    <<Idx:16/big, ChunkLen:32/big>> = recv_exact(Socket, 6),
    Chunk = recv_exact(Socket, ChunkLen),
    io:format("   • Chunk ~p recibido (~p bytes)~n", [Idx, ChunkLen]),
    gather_chunks(Socket, Remaining - ChunkLen, [{Idx, Chunk}|Acc], FileName).

assemble_chunks(Chunks, FileName) ->
    Sorted = lists:keysort(1, Chunks),
    Binary = lists:foldl(fun({_I, C}, Acc) -> <<Acc/binary, C/binary>> end, <<>>, Sorted),
    Target = filename:join("descargas", FileName),
    ok = file:write_file(Target, Binary),
    io:format("Archivo completo guardado en ~s (~p bytes).~n>",
              [Target, byte_size(Binary)]).

recv_exact(Socket, N) ->
    %% Recibe exactamente N bytes desde el socket
    case gen_tcp:recv(Socket, N) of
        {ok, Data} when byte_size(Data) =:= N ->
            Data;
        {ok, Part} ->
            Rest = recv_exact(Socket, N - byte_size(Part)),
            <<Part/binary, Rest/binary>>;
        {error, Reason} ->
            io:format("Hubo un error durante la transferencia~n>"),
            exit({recv_error, Reason})
    end.
