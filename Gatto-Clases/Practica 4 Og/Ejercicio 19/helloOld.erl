-module(hello).
-export([init/0, cambiar_idioma/0]).

hello(Msg) ->
    receive 
        cambio -> io:fwrite("Cambio idioma~n"),
                    hello("Hola")
    after 
        1000 -> ok end,
    io:fwrite("~p ~p~n", [Msg, case rand:uniform(10) of 10 -> 1/uno; _ -> self() end]),
    levantar(Msg).

levantar(Msg) ->
    case catch hello(Msg) of
        {'EXIT', {badarith, _}} -> io:fwrite("Levanto proceso ~n"),
                                    hello(Msg)
    end.

cambiar_idioma() ->
    self() ! cambio.

init() -> spawn(fun () -> levantar("Hello") end).