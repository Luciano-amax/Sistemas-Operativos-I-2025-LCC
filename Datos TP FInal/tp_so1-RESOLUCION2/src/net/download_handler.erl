-module(download_handler).
-export([process/2]).

-define(CODE_NOT_FOUND, 112).
-define(CODE_SINGLE, 101).
-define(CODE_CHUNK, 111).
-define(MAX_CHUNK_SIZE, 4*1024*1024).
%% @doc
%% Módulo encargado de manejar las solicitudes de descarga de archivos desde el servidor TCP.
%% Soporta envíos de archivos en bloque único o en chunks, según su tamaño.
%%
%% El formato esperado para la solicitud es:
%%     <<"DOWNLOAD_REQUEST NombreDeArchivo NodoId">>

%% @spec process(binary(), socket()) -> ok
%% @doc
%% Procesa una solicitud de descarga recibida por TCP.
%% 1. Parsea el binario recibido para obtener el nombre del archivo.
%% 2. Intenta leer el archivo desde la carpeta "compartida/".
%% 3. Si el archivo existe, lo envía al cliente (en bloque único o en chunks).
%% 4. Si el archivo no se encuentra, envía un código de error.
process(LineBin, Socket) ->
    case parse_request(LineBin) of
        {ok, FileName, _} ->
            io:format("Filename: ~p~n", [FileName]),
            SharedPath = filename:join("compartida", FileName),
            case file:read_file(SharedPath) of
                {ok, Data} ->
                    %io:format("Archivo encontrado, enviando...~n"),
                    send_file(Socket, Data);
                {error, _Reason} ->
                    io:format("Error al encontrar el archivo: ~p~n>", [_Reason]),
                    gen_tcp:send(Socket, <<?CODE_NOT_FOUND>>)
            end;
        {error, _Reason} ->
            gen_tcp:send(Socket, <<?CODE_NOT_FOUND>>)
    end.

%% @spec parse_request(binary()) -> {ok, string(), string()} | {error, atom()}
%% @doc
%% Parsea el binario recibido en formato `<<"DOWNLOAD_REQUEST Archivo NodoId">>`.
%% Devuelve `{ok, Archivo, NodoId}` si es exitoso; de lo contrario `{error, bad_format}`.
parse_request(LineBin) ->
    %% Esperamos ASCII: "DOWNLOAD_REQUEST " ++ FileName ++ " " ++ NodeId\n
    %io:format("~p~n", LineBin),
    case binary:split(LineBin, <<" ">>, [global]) of
        [<<"DOWNLOAD_REQUEST">>, FileBin, NodeBin] ->
            %io:format("request parseada: ~p~n", []),
            {ok, binary_to_list(FileBin), binary_to_list(NodeBin)};
        [<<"DOWNLOAD_REQUEST">>, FileBin] ->
            %io:format("request parseada: ~p~n", []),
            {ok, binary_to_list(FileBin),""};
        _ ->
            io:format("Error al parsear la request:"),
            io:format("~p~n>", [binary_to_list(binary:split(LineBin, <<" ">>, [global]))]),
            {error, bad_format}
    end.

%% @spec send_file(socket(), binary()) -> ok
%% @doc
%% Envía el contenido de un archivo al cliente por TCP.
%% Si el tamaño es menor o igual a 4MB, se envía en un solo bloque.
%% Si el archivo excede ese tamaño, se inicia negociación y se envía por partes (chunks).
send_file(Socket, Data) when byte_size(Data) =< ?MAX_CHUNK_SIZE ->
    %% io:format("Enviando file...~n"),
    %% Envío en un solo bloque: <CODE_SINGLE><Size:32><Data>
    Size = byte_size(Data),
    Header = <<?CODE_SINGLE:8, Size:32/big>>,
    gen_tcp:send(Socket, <<Header/binary, Data/binary>>);
send_file(Socket, Data) ->
    %% Envío por chunks
    Total = byte_size(Data),
    %% Primero Header de negociación: <CODE_SINGLE><Total:32><ChunkSize:32>
    Standard = ?MAX_CHUNK_SIZE,
    Header0 = <<?CODE_SINGLE:8, Total:32/big, Standard:32/big>>,
    gen_tcp:send(Socket, Header0),
    send_chunks(Socket, Data, 0).

%% @spec send_chunks(socket(), binary(), non_neg_integer()) -> ok
%% @doc
%% Fragmenta y envía el archivo en múltiples chunks al cliente,
%% cada uno con un índice de secuencia para reensamblado.
send_chunks(_Socket, <<>>, _Index) -> ok;
send_chunks(Socket, Rest, Index) ->
    Chunk = binary:part(Rest, 0, min_f(byte_size(Rest), ?MAX_CHUNK_SIZE)),
    Remaining = binary:part(Rest, byte_size(Chunk), byte_size(Rest) - byte_size(Chunk)),
    ChunkSize = byte_size(Chunk),
    %% Mensaje: <CODE_CHUNK><Index:16><ChunkSize:32><Chunk>
    Msg = <<?CODE_CHUNK:8, Index:16/big, ChunkSize:32/big, Chunk/binary>>,
    gen_tcp:send(Socket, Msg),
    send_chunks(Socket, Remaining, Index + 1).

%% @spec min_f(integer(), integer()) -> integer()
%% @doc
%% Devuelve el menor de los dos valores.
min_f(A, B) when A =< B -> A;
min_f(_, B) -> B.
