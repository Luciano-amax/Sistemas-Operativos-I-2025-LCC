-module(hello).
-export([init/0, worker/0, supervisor/1]).

init() ->
    spawn(hello, supervisor, [self()]).

worker() ->
    receive
    after 1000 -> ok
    end,
    io:fwrite("Hello ~p~n", [
        case rand:uniform(10) of
            10 -> 1 / uno;
            _ -> self()
        end
    ]),
    worker().

supervisor(Parent) ->
    process_flag(trap_exit, true),
    WorkerPid = spawn_link(hello, worker, []),
    Parent ! {supervisor_started, self(), WorkerPid},
    loop(WorkerPid).

loop(WorkerPid) ->
    receive
        {shutdown, Reason} ->
            WorkerPid ! {exit, Reason},
            receive
                {'EXIT', WorkerPid, Reason} ->
                    io:fwrite("Hello termino\n"),
                    io:fwrite("Terminando el supervisor\n")
            end;
        {'EXIT', WorkerPid, _} ->
            io:fwrite("Hello termino por un error desconocido\n"),
            % Restart worker
            NewWorkerPid = spawn_link(hello, worker, []),
            loop(NewWorkerPid)
    end.
