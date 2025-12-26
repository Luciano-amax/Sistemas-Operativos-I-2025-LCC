% -------------------- MECANISMO de DESCUBRIMIENTO / RECONOCIMIENTO ---------------------- %

-module(discovery).
-include("defs.hrl").
-export([recv_and_hello/3, loop_recv/4, send_hello/4, loop_send/3]).

% Prepara el mensaje 'HELLO' para ser enviado por loop_send().
send_hello(PORT_TCP, ID, UDP_Socket, BcastIP) ->
  Port = lists:flatten(io_lib:format(" ~p", [PORT_TCP])),
  Msg = "HELLO " ++ ID ++ Port ++ "\n",
  Bin = list_to_binary(Msg),
  loop_send(UDP_Socket, Bin, BcastIP).

% Envía un 'HELLO' cada HELLO_WAIT seg en loop.
loop_send(UDP_Socket, Msg, BcastIP) ->
	gen_udp:send(UDP_Socket, BcastIP, ?PORT_UDP, Msg),
	receive
	after ?HELLO_WAIT -> loop_send(UDP_Socket, Msg, BcastIP)
	end.

% Abre el Socket UDP principal y llama al proceso
% encargado de esperar por mensajes Broadcast
% entrantes por parte de otros nodos.
% Spawnea el proceso encargado de mandar los 'HELLO'.
recv_and_hello(PORT_TCP, ID, BcastIP) ->
	{ok, UDP_Socket} = gen_udp:open(?PORT_UDP, [binary, {broadcast, true}, {reuseaddr, true}]),
	spawn(?MODULE, send_hello, [PORT_TCP, ID, UDP_Socket, BcastIP]),
	loop_recv(UDP_Socket, maps:new(), ID, []).

% Mensaje entrante:
% HELLO ->
%   - Si la ID no está registrada en el mapa de
%     nodos conocidos (error), se registra y
%     vuelve a llamar con el nuevo mapa.
%   - Si la ID ya estaba registrada se vuelve a
%     llamar sin hacer ningún cambio.
% NAME_REQUEST ->
%   - Si la ID solicitada es igual a la propia,
%     le contesta 'INVALID_NAME'.
%   - Si la ID solicitada no es la propia,
%     se almacena temporalmente en Requested
%     (Requested almacena las ID en proceso
%      de ser aceptadas para contemplar repetidos).
%     Al mismo tiempo spawnea una función que tras ID_WAIT seg,
%     una vez fue aceptada la ID, nos recuerda eliminarla
%     de Requested.
% OTRO ->
%   - Se vuelve a llamar sin hacer ningún cambio.
loop_recv(UDP_Socket, NodeMap, MyID, Requested) ->
	receive
		{eliminar_id, ID} ->
			NewRequested = Requested -- [ID],
			loop_recv(UDP_Socket, NodeMap, MyID, NewRequested);

    {udp, Sender_Socket, Sender_IP, Sender_Port, Data} ->
      Msg = string:trim(binary_to_list(Data)),
      case string:tokens(Msg, " ") of
        ["HELLO", ID, TCP_Port] ->
          if ID == MyID -> loop_recv(UDP_Socket, NodeMap, MyID, Requested);
          true ->
            case maps:find(ID, NodeMap) of
              {ok, _} ->
                loop_recv(UDP_Socket, NodeMap, MyID, Requested);
              error ->
                NewNodeMap = maps:put(ID, {Sender_IP, TCP_Port}, NodeMap),
                io:format("Nuevo nodo registrado! -> ~p~n", [ID]),
                loop_recv(UDP_Socket, NewNodeMap, MyID, Requested)
            end
          end;
        ["NAME_REQUEST", ID] ->
          case ID of
            MyID ->
              Error = list_to_binary("INVALID_NAME " ++ ID ++ "\n"),
              gen_udp:send(Sender_Socket, Sender_IP, Sender_Port, Error),
              loop_recv(UDP_Socket, NodeMap, MyID, Requested);

            _ ->
              case lists:member(ID, Requested) of
                true ->
                  Error = list_to_binary("INVALID_NAME " ++ ID ++ "\n"),
                  gen_udp:send(Sender_Socket, Sender_IP, Sender_Port, Error),
                  loop_recv(UDP_Socket, NodeMap, MyID, Requested);

                false ->
                  NewRequested = Requested ++ [ID],
                  spawn(fun() -> timer:sleep(?ID_WAIT),
                                 self() ! {eliminar_id, ID}
                        end),
                  loop_recv(UDP_Socket, NodeMap, MyID, NewRequested)
              end
          end;

        _ ->
          loop_recv(UDP_Socket, NodeMap, MyID, Requested)
      end;

    {get_list,Pid} ->
      Pid ! {ok,NodeMap,MyID},
      loop_recv(UDP_Socket, NodeMap, MyID, Requested); %el CLI solicita buscar un archivo -> Necesita la lista de los nodos en la red y se la solicita al "main"

    _MensajeDesconocido ->
      loop_recv(UDP_Socket, NodeMap, MyID, Requested)
  end.
