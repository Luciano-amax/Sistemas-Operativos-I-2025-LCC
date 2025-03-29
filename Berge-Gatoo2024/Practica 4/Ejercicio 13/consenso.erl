-module(consenso).
-export([main/1, call/3]).

send(SEND_TO, VALUE) ->
    if
        SEND_TO == no_pid ->
            VALUE;
        true ->
            SEND_TO ! {self(), VALUE}
    end.

call(I, J, SEND_TO) ->
    if
        J - I == 1 ->
            send(SEND_TO, I);
        true ->
            M = (I + J) div 2,
            PID_RIGHT = spawn_link(consenso, call, [M, J, self()]),
            CALC_LEFT = call(I, M, no_pid),
            receive
                {PID_RIGHT, CALC_RIGHT} ->
                    send(SEND_TO, CALC_LEFT + CALC_RIGHT)
            end
    end.

main(N) ->
    process_flag(trap_exit, true),
    CALC = call(0, N, no_pid),
    receive
        {'EXIT', _, normal} ->
            io:fwrite("Valor calculado: ~p\nValor esperado: ~p\n", [CALC, (N * (N - 1) div 2)]);
        {'EXIT', _, REASON} ->
            io:fwrite("El calculo fallo debido a un error, ~p", REASON)
    end.
