-module(turnos).
-export([server/0, counter/1]).

counter(Count) ->
    receive
        From ->
            From ! (integer_to_list(Count) ++ "\n"),
            counter(Count + 1)
    end.

server() ->
    register(counter_process, spawn(turnos, counter, [0])),
    {ok, ListenSocket} = gen_tcp:listen(8000, [{reuseaddr, true}]),
    wait_connect(ListenSocket, 0).

wait_connect(ListenSocket, N) ->
    {ok, Socket} = gen_tcp:accept(ListenSocket),
    spawn(fun() -> wait_connect(ListenSocket, N + 1) end),
    io:fwrite("Se conecto un cliente!(~p)~n", [Socket]),
    get_request(Socket).

get_request(Socket) ->
    receive
        {_, _, "NUEVO\n"} ->
            ok,
            counter_process ! self(),
            receive
                Number ->
                    io:fwrite("~p solicito un numero ~n", [Socket]),
                    gen_tcp:send(Socket, Number)
            end,
            get_request(Socket);
        {_, _, "CHAU\n"} ->
            io:fwrite("~p se desconecto~n", [Socket]),
            gen_tcp:close(Socket);
        {tcp_closed, _} ->
            io:fwrite("~p se desconecto~n", [Socket]),
            gen_tcp:close(Socket)
    end.
