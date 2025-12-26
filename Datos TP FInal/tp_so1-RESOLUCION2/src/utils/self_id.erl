%% src/utils/self_id.erl
-module(self_id).
-export([set/1, get/0]).

-define(TABLE, self_id).

set(Id) ->
    %Creamos una tabla en la ets (eralng term storage), para poder acceder 
    % rápidamente y segura a concurrencias
    ets:new(?TABLE, [named_table, public, set, {read_concurrency, true}]),
    ets:insert(?TABLE, {id, Id}),
    ok.

get() ->
    case ets:lookup(?TABLE, id) of
        [{id, Id}] -> Id; %si está el id lo devolvemos
        []         -> "????" %en caso contrario devuelve signos de pregunta
    end.
