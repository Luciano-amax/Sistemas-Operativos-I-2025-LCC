% --------------------------------------- INIT -------------------------------------------- %

-module(init_srv).
-include("defs.hrl").
-export([start_up/1, confirm_ID/4, check/5, is_my_id_ok/1]).


% Abre un Socket UDP específico para manejar el 'NAME_REQUEST' posterior.
start_up(PORT_TCP) ->
  Seed = {erlang:monotonic_time(), erlang:unique_integer(), erlang:phash2(self())},
  rand:seed(exsplus, Seed),

  ID = utils:get_random_id("", 4),
  IP = utils:get_local_ipv4(),
  BcastIP = utils:get_broadcast_ip(),
  timer:sleep(1000),
  io:format("\n"),  
  
  spawn(?MODULE, confirm_ID, [PORT_TCP, BcastIP, ID, IP]),
  cli:cli_loop().


% Se envia el 'NAME_REQUEST' de nuestra ID por Broadcast UDP.
% Llama a is_my_id_ok() e interpreta su retorno en caso de éxito o error.
% - invalidans  -> Un nodo le mandó algo diferente a 'INVALID_NAME'
% - idinuse     -> Un nodo le mandó 'INVALID_NAME'
% - ok          -> El ID está disponible, se cierra el Socket UDP específico,
%                  y se comienza el server.
confirm_ID(PORT_TCP, BcastIP, ID, IP) ->
  {ok, UDP_Socket} = gen_udp:open(?PORT_UDP, [binary, {ip, IP}, {broadcast, true}, {reuseaddr, true}]),
  check(PORT_TCP, UDP_Socket, BcastIP, ID, IP).

check(PORT_TCP, UDP_Socket, BcastIP, ID, IP) ->
  io:format("CHECKING IF " ++ ID ++ " IS ALREADY TAKEN, PLEASE WAIT...~n"),
  Msg = "NAME_REQUEST " ++ ID ++ "\n",
  Bin = list_to_binary(Msg),
  gen_udp:send(UDP_Socket, BcastIP, ?PORT_UDP, Bin),

  case is_my_id_ok(IP) of
    {error, ID} ->
      io:format("SERVER_ERROR: [~s] ALREADY TAKEN, TRIYING AGAIN WITH ANOTHER ID IN 5 SECONDS...~n", [ID]),
      timer:sleep(?RETRY_WAIT),
      NewID = utils:get_random_id("", 4),
      check(PORT_TCP, UDP_Socket, BcastIP, NewID, IP);
    ok ->
      io:format("ID OK~n~n"),
      gen_udp:close(UDP_Socket),
      srv_tcp:start_server(PORT_TCP, BcastIP, ID)
  end.

% - Si la respuesta viene desde nuestra propia IP, la ignoramos
%   y esperamos la respuesta de alguien más.
% - Si no, solo esperamos que alguien nos responda.
%
% * Si alguien responde 'INVALID_NAME', se notifica.
% * Si alguien responde otra cosa, se ignora.
% * Si no, se asume que la ID no está en uso y la tomamos.
is_my_id_ok(MyIP) ->
  receive
    {udp, _Sender_Socket, Sender_IP, _Sender_Port, Data} ->
      case Sender_IP of
        MyIP ->
          is_my_id_ok(MyIP);
        _ ->
          Msg = string:trim(binary_to_list(Data)),
          case string:tokens(Msg, " ") of
            ["INVALID_NAME", ID] ->
              {error, ID};
            _ ->
              is_my_id_ok(MyIP)
          end
      end
  after ?ID_WAIT -> ok
  end.
