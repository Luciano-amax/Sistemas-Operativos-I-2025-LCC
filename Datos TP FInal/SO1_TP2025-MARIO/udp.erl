% --------------------------------------
% Módulo "udp" de conexiones para HELLO.
% --------------------------------------

-module (udp).
-export([start/2, hello_loop/2, receive_hello_loop/3, actualizar_nodos/3, borrar_nodos_viejos/1]).
-include("config.hrl").


% El uso de monotonic_time es porque segun la informacion de erlang es confiable, creciente
% y el valor devuelto ya esta en segundos.
% Como solo nos interesa la diferencia de segundos, no nos importa en cuanto esta el contador devuelto.

start(NodoID,TablaEts)->
            Msg = io_lib:format("HELLO ~s ~w~n", [NodoID,?PUERTO_TCP]),
            case gen_udp:open(?UDP_PUERTO, [{active, true},{broadcast, true},{ip, ?UDP_BROADCAST_OPEN}, {reuseaddr, true}]) of
                {ok, Socket1} ->
                    Pid = spawn(?MODULE,hello_loop,[Socket1,Msg]),
                    gen_udp:controlling_process(Socket1,Pid),
                    spawn(?MODULE,borrar_nodos_viejos,[TablaEts]);
                {error, Reason1} ->
                    io:format("Error al abrir socket UDP U01: ~p. Reintentando...~n", [Reason1])
                end,
            case gen_udp:open(?UDP_PUERTO, [{active, true},{broadcast, true},{ip, ?UDP_BROADCAST_OPEN}, {reuseaddr, true}]) of
                {ok, Socket2} ->
                    receive_hello_loop(Socket2,TablaEts,NodoID);
                {error, Reason2} ->
                    io:format("Error al abrir socket UDP U02: ~p. Reintentando...~n", [Reason2])
                end.

% Función que envía un loop de mensajes HELLO
hello_loop(Socket, Msg)->
    gen_udp:send(Socket,?UDP_BROADCAST_SEND,?UDP_PUERTO,Msg),
    timer:sleep(?UDP_INTERVALO_HELLO),
    hello_loop(Socket,Msg).

% Función que recive mensajes HELLO
receive_hello_loop(Socket,TablaEts,NodoID) ->
    receive
        {udp,_,Ip,_,Msg} -> 
            % string:trim saca el caracter \n del Msg, hacemos token con las palabras del Msg
            MsgTrim = string:trim(Msg),
            ToList = string:tokens(MsgTrim," "),
            Type = lists:nth(1,ToList),
            Id = lists:nth(2,ToList),
            case Type of
                "NAME_REQUEST" ->
                    if
                        (Id == NodoID)   ->
                            InvalidName = io_lib:format("INVALID_NAME ~s~n",[Id]),
                            gen_udp:send(Socket,Ip,?UDP_PUERTO,InvalidName);
                        true -> ok
                    end;
                "HELLO" ->
                    if
                        (Id /= NodoID)   ->
                            actualizar_nodos(ToList,Ip,TablaEts);
                        true -> ok
                    end;
                true -> ok
            end,
            receive_hello_loop(Socket,TablaEts,NodoID)
    end.

% Funcion que agrega nodos a tabla ETS
actualizar_nodos(List,Ip,TablaEts)->
    % Asignamos cada una al dato a actualizar o en todo caso a insertar como nuevo
    Id = lists:nth(2,List),
    PortSring = lists:nth(3,List),
    {Port, _} = string:to_integer(PortSring),
    Time = erlang:monotonic_time(second),
    ets:insert(TablaEts,{Id,{Ip,Port,Time}}).
    
% Funcion que borra nodos a tabla ETS
borrar_nodos_viejos(TablaEts)->
    ToList = ets:tab2list(TablaEts),
    TimeNow = erlang:monotonic_time(second),
    lists:foreach(
        fun({Id,{_,_,Time}})->
            if
                ((TimeNow - Time) > ?UDP_BORRAR_NODO_DIFERENCIA) -> ets:delete(TablaEts,Id);
                true -> ok
            end
        end
        ,ToList),
    timer:sleep(?UDP_BORRAR_NODO_ESPERA),
    borrar_nodos_viejos(TablaEts).
