% ---------------------------------------------------------------------
% Módulo "name" de confirmacion de NodoID (NAME_REQUEST e INVALID_NAME).
% ---------------------------------------------------------------------

-module(name).
-export([inicio_nombre/0, confimar_nombre/1, nombrar_nodo/0, esperar_nodo/2]).
-include("config.hrl").

% Proceso que nombra no rechazado por algún nodo.
inicio_nombre() ->
    case gen_udp:open(?UDP_PUERTO, [{active, true},{broadcast, true},{ip, ?UDP_BROADCAST_OPEN}, {reuseaddr, true}]) of
        {ok, Socket} ->
            {ok, NodoID} = confimar_nombre(Socket),
            gen_udp:close(Socket),
            io:format("NodoID confirmado: ~s~n", [NodoID]),
            {ok, NodoID};
        {error, Reason} ->
            io:format("Error al abrir socket UDP N01: ~p. Reintentando...~n", [Reason]),
            timer:sleep(?UDP_NOMBRE_RESTART),
            inicio_nombre()
    end.

% Confirma el nombre de un nodo.
confimar_nombre(Socket) ->
    NodoID = nombrar_nodo(),
    Msg = io_lib:format("NAME_REQUEST ~s~n",[NodoID]),
    case gen_udp:send(Socket, ?UDP_BROADCAST_SEND, ?UDP_PUERTO, Msg) of
        ok ->
            Time = erlang:monotonic_time(second),
            case esperar_nodo(NodoID,Time) of
                {nombreOcupado, _} -> 
                    io:format("Nombre ~p ocupado. Generando nuevo...~n", [NodoID]),
                    timer:sleep(2000 + rand:uniform(8000)),
                    confimar_nombre(Socket);
                {error, ErrorCode} -> 
                    {error, ErrorCode};
                {ok, _} -> 
                    {ok, NodoID}  % Confirma NodoID.
            end;
        {error, Reason} ->
            io:format("Error al enviar nombre N02: ~p~n", [Reason]),
            confimar_nombre(Socket)
    end.

% Crea nombre de nodo.
nombrar_nodo() ->
    rand:seed(exsplus),
    Chars = ?NOMBRE_NODO_CARACTERES,
    Len = ?NOMBRE_NODO_LONGITUD,
    [lists:nth(rand:uniform(length(Chars)), Chars) || _ <- lists:seq(1,Len)].

% Espera mensaje de nombre de nodo.
esperar_nodo(NodoID,Time) ->
    TimeNow = erlang:monotonic_time(second),
    if
        (TimeNow - Time >= ?NOMBRE_NODO_ESPERA) ->
            {ok, NodoID};
        true ->
            receive 
                {udp, _, _, _, Msg} -> 
                    ToList = string:tokens(Msg ," "),
                    Elem = lists:nth(1, ToList),
                    Id = lists:nth(2, ToList),
                    if
                        ((Elem == "INVALID_NAME") and (Id == NodoID))->
                            {nombreOcupado, NodoID}; % Retorna nombre ocupado
                        true ->
                            esperar_nodo(NodoID,Time) 
                    end
            after 10000 ->
                {ok, NodoID} % Retorna nombre de nodo disponible.
            end
    end.

