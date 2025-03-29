-module(cliente).
-export([cliente/0, mensajes/1]).

cliente() ->
    SomeHost = "localhost",
    {ok, ClienteSocket} = gen_tcp:connect(SomeHost, 8000, [binary, {packet, 0}]),
    mensajes(ClienteSocket).

mensajes(ClienteSocket) ->
    {ok, Opcion} = io:read("Ingrese opcion: "),
    case Opcion of
        0 -> gen_tcp:close(ClienteSocket);
        1 -> gen_tcp:send(ClienteSocket, "Hola servidor");
        2 -> gen_tcp:send(ClienteSocket, "Hello server");
        _ -> gen_tcp:send(ClienteSocket, "Error") 
    end,
    mensajes(ClienteSocket).