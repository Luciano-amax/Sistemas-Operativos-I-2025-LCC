-module(intro).
-export([wait/1, cronometro/1]).


wait(N) ->
    receive
        %%
    after
        N -> ok
    end.

cronometro(F) -> F.

