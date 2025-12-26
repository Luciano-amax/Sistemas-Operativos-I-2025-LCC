
-module(search_client).

-export([search_in_network/1]).

-export([send_search_request/4, collect_search_responses/2, 
         parse_search_response/1, display_search_results/2]).

%%--------------------------------------------------------------------
%% @doc
%% Realiza una bisqueda de archivos en toda la red P2P
%% Obtiene la lista de nodos registrados y enviia SEARCH_REQUEST a cada uno, luego recolecta y muestra los resultados
%%--------------------------------------------------------------------
search_in_network(Pattern) ->
    node_registry ! {get_all_nodes, self()},
    io:format("Buscando archivo: ~s...~n", [Pattern]),
    
    receive
        Nodes when is_list(Nodes) ->
            MyId = self_id:get(),
            %% Enviar SEARCH_REQUEST a cada nodo
            Results = lists:flatmap(
                fun({_, Ip, Port}) ->
                    send_search_request(MyId, Pattern, Ip, Port)
                end,
                Nodes
            ),
            %% Mostrar resultados
            display_search_results(Results, Pattern);
        _Other ->
            io:format("Error al obtener la lista de nodos~n>")
    after 1000 ->
        io:format("Timeout al consultar nodos~n>")
    end.

%%--------------------------------------------------------------------
%% @doc
%% Envia un SEARCH_REQUEST a un nodo específico y recolecta las respuestas
%%--------------------------------------------------------------------
send_search_request(MyId, Pattern, Ip, Port) ->
    case gen_tcp:connect(Ip, Port, [binary, {packet, line}, {active, false}], 5000) of
        {ok, Socket} ->
            %% Enviar SEARCH_REQUEST
            Message = io_lib:format("SEARCH_REQUEST ~s ~s~n", [MyId, Pattern]),
            gen_tcp:send(Socket, iolist_to_binary(Message)),
            
            %% Recolectar respuestas SEARCH_RESPONSE
            Results = collect_search_responses(Socket, []),
            gen_tcp:close(Socket),
            Results;
        {error, Reason} ->
            io:format("Error conectando a ~p:~p - ~p~n>", [Ip, Port, Reason]),
            []
    end.

%%--------------------------------------------------------------------
%% @doc
%% Recolecta todas las respuestas SEARCH_RESPONSE de un socket.
%%--------------------------------------------------------------------
collect_search_responses(Socket, Acc) ->
    case gen_tcp:recv(Socket, 0, 2000) of  %% Timeout de 2 segundos
        {ok, BinLine} ->
            case parse_search_response(BinLine) of
                {ok, Result} ->
                    collect_search_responses(Socket, [Result | Acc]);
                error ->
                    collect_search_responses(Socket, Acc)
            end;
        {error, timeout} ->
            lists:reverse(Acc);
        {error, closed} ->
            lists:reverse(Acc);
        {error, _Reason} ->
            lists:reverse(Acc)
    end.

%%--------------------------------------------------------------------
%% @doc
%% Parsea una linea SEARCH_RESPONSE
%% Formato esperado: "SEARCH_RESPONSE NodeId FileName FileSize\n"
%%--------------------------------------------------------------------
parse_search_response(BinLine) ->
    Line = string:trim(binary_to_list(BinLine)),
    case string:tokens(Line, " ") of
        ["SEARCH_RESPONSE", NodeId, FileName, SizeStr] ->
            case string:to_integer(SizeStr) of
                {Size, _} -> {ok, {NodeId, FileName, Size}};
                error -> error
            end;
        _ -> error
    end.

%%--------------------------------------------------------------------
%% @doc
%% Muestra los resultados de busqueda de forma organizada
%%--------------------------------------------------------------------
display_search_results([], Pattern) ->
    io:format("No se encontraron archivos que coincidan con '~s'~n>", [Pattern]);
display_search_results(Results, Pattern) ->
    io:format("~nResultados para '~s':~n", [Pattern]),
    lists:foreach(
        fun({NodeId, FileName, Size}) ->
            io:format(" - ~s (~p bytes) en nodo ~s~n", [FileName, Size, NodeId])
        end,
        Results
    ),
    io:format(">").