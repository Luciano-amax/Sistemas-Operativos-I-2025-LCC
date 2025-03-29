-module(turnos).
-export([server/0]).

server() ->
    {ok, ListenSocket} = gen_tcp:listen(8000, [binary, {packet, 0}, {active, true}]),
    wait_connect(ListenSocket, 0).

wait_connect(ListenSocket, N) ->
    {ok, Socket} = gen_tcp:accept(ListenSocket),
    spawn (fun () -> wait_connect (ListenSocket, N+1) end),
    get_request(Socket).

get_request(Socket) ->
    io:fwrite("Esperando mensajes de ~p~n", [Socket]),
    receive
        {tcp, Socket, 0} -> io:fwrite("Salimos ~n"),
                            gen_tcp:close(Socket);
        {tcp, Socket, Data} ->  io:fwrite("El mensaje recibido ~p~n", [Data]),
                                get_request(Socket);
        _ -> io:fwrite("Salimos ~n"),
             gen_tcp:close(Socket)
    end,
    get_request(Socket).