-module(main).
-export([start/0]).

start() ->
    Port = 12345,
    io:format("Iniciando TCP en ~p~n", [Port]),
    {ok, _} = tcp_server:start_link(Port),
    node_registry:start_link(),
    name_request:start(),
    cli:start_cli().

