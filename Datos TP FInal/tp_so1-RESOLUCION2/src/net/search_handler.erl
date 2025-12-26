-module(search_handler).
-export([process/2]).

%% @spec strip_crlf(string()) -> string()
%% @doc
%% Elimina los caracteres `\n` y `\r` del final de una cadena.
strip_crlf(Str) -> strip_crlf_rev(lists:reverse(Str)).

strip_crlf_rev([$\n | T]) -> strip_crlf_rev(T);
strip_crlf_rev([$\r | T]) -> strip_crlf_rev(T);
strip_crlf_rev(L)         -> lists:reverse(L).

%% @spec parse(binary()) -> {string(), string()}
%% @doc
%% Convierte un binario de tipo `<<"SEARCH_REQUEST Id Patrón">>` en una tupla
%% `{Id, Patron}`. Quita los caracteres de salto de línea del patrón.
parse(Bin) ->
    %% <<"SEARCH_REQUEST Id Patrón\n">>
    [<<"SEARCH_REQUEST">>, IdBin, PatternBin] =
        binary:split(Bin, <<" ">>, [global]),

    Pattern = strip_crlf(binary_to_list(PatternBin)),
    {binary_to_list(IdBin), Pattern}.

%% @doc
%% @spec process(binary(), socket()) -> ok
%% Procesa una solicitud de búsqueda desde un cliente.
%% 1. Parsea la línea de entrada para obtener el patrón de búsqueda.
%% 2. Consulta archivos locales que coincidan con el patrón.
%% 3. Envía una respuesta por cada coincidencia al socket remoto.
%% 4. Cierra la conexión.
process(LineBin, Socket) ->
    
    %% 1) Parsear línea
    {_IdEmitter, Pattern} = parse(LineBin),

    %% 2) Buscar coincidencias locales
    Matches = shared_files:match(Pattern),
    
    MyId = self_id:get(),
    %% 3) Responder cada match
    lists:foreach(
        fun({Name, Size}) ->
              Packet = io_lib:format(
                         "SEARCH_RESPONSE ~s ~s ~B\n",
                         [MyId, Name, Size]),
              case gen_tcp:send(Socket, iolist_to_binary(Packet)) of
                  ok ->
                      %io:format("Enviado: ~s", [Packet]);
                      io:format("");
                  {error, Reason} ->
                      io:format("Fallo send: ~p~n>", [Reason])
              end
        end,
        Matches),

    %% 4) Cerrar la conexión
    gen_tcp:close(Socket),
    ok.