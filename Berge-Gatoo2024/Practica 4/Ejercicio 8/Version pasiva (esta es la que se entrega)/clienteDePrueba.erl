-module(cliente).
-export([cliente/0]).

cliente() ->
    SomeHost = "localhost",
    {ok, ClientSock} = gen_tcp:connect(SomeHost, 8000, [{reuseaddr, true}, binary, {packet, 0}, {active, false}]),
    mensajes(ClientSock).

mensajes(ClientSock) ->
    io:format("Ingrese opcion: "),
    Msg = io:get_line(""),

    case Msg of
        "NUEVO\n" -> gen_tcp:send(ClientSock, list_to_binary(Msg)),
        
                     case gen_tcp:recv(ClientSock, 0) of
                        {ok, Numero} -> io:fwrite("~p~n", [Numero]),
                                        mensajes(ClientSock);
                        {error, _} -> io:format("Hubo un error en la conexion~n"),
                                      gen_tcp:close(ClientSock)
                     end;

        "CHAU\n" -> gen_tcp:send(ClientSock, list_to_binary(Msg)),
                    gen_tcp:close(ClientSock);

        _ -> io:fwrite("Mensaje invalido~n"),
             mensajes(ClientSock)
    end.
