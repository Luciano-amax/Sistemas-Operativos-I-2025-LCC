-module(turnos).
-export([server/0, contador/1]).

contador(U) ->
    receive
        From -> From ! (integer_to_binary(U)),
                contador(U+1)
    end.

server() ->
    register(cant, spawn(turnos, contador, [0])),
    {ok, ListenSocket} = gen_tcp:listen(8000, [{reuseaddr, true}, binary, {packet, 2}, {active, false}]),
    wait_connect(ListenSocket, 0).

wait_connect(ListenSocket, N) ->
    {ok, Socket} = gen_tcp:accept(ListenSocket),
    spawn (fun () -> wait_connect (ListenSocket, N+1) end),
    get_request(Socket).

get_request(Socket) ->
    io:fwrite("Esperando mensajes de ~p~n", [Socket]),

    case gen_tcp:recv(Socket, 0) of
        {ok, Bin} -> 
            Msg = binary_to_list(Bin),

            case Msg of
                "NUEVO\n" -> io:fwrite("Llego mensaje deseado~n"),
                            cant ! self(),
                            receive
                                N -> gen_tcp:send(Socket, N),
                                     get_request(Socket)
                            end;
                "CHAU\n" -> gen_tcp:close(Socket);

                _ -> io:fwrite("Mensaje invalido~n"),
                     gen_tcp:close(Socket)
            end;

        _ -> io:fwrite("Error conexion~n"),
             gen_tcp:close(Socket)
    end.