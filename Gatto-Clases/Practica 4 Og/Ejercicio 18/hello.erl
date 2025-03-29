-module(hello).
-export([init/0, levantar/0]).

hello() ->
    receive after 1000 -> ok end,
    io:fwrite("Hello ~p~n", [case rand:uniform(10) of 10 -> 1/uno; _ -> self() end]),
    levantar().

levantar() ->
    case catch hello() of
        {'EXIT', {badarith, _}} -> io:fwrite("Levanto proceso ~n"),
                                    levantar()
    end.

init() -> spawn(fun () -> levantar() end).