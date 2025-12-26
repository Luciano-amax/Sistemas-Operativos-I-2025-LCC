% ----------------------------- LIBRERIA DE FUNCIONES AUXILIARES ----------------------------------- %

-module(utils).
-export([get_random_id/2, get_broadcast_ip/0, get_local_ipv4/0]).
-export([flatten_string/2, flatten_string_space/2, shared/0, parse_String/1, get_dicc/0, get_list/0]).

% Reducir una lista de strings a un unico string
flatten_string(NewString, List_string) ->
  case List_string of
    [] -> string:slice(NewString, 1); % Elimino el espacio inicial (En la primera ejecución)
                                      % Ejemplo: testflatten:flatten_string("",["Hola","Como","Estas"]). -> String: "Hola Como Estas"
                                      % Sin slice seria -> String: " Hola Como Estas"
    [S | Rest] -> flatten_string(NewString ++ " " ++ S, Rest)
  end.
  
flatten_string_space(NewString, List_string) ->
  case List_string of
    [Last] -> 
      {string:slice(NewString, 1), Last};
    [S | Rest] -> 
      flatten_string_space(NewString ++ " " ++ S, Rest)
  end.

% Genera un ID aleatorio conformado por 4 caracteres ASCII alfanuméricos.
get_random_id(ID, 0) -> ID;
get_random_id(ID, Count) ->
      Char =
        case rand:uniform(3) of
          1 -> rand:uniform(10) + 47;
          2 -> rand:uniform(26) + 64;
          3 -> rand:uniform(26) + 96
        end,
      get_random_id(ID ++ [Char], Count-1).

% Es equivalente a correr el comando 'ifconfig' e ir
% filtrando hasta encontrar la IP de Broadcast de la red.
get_broadcast_ip() ->
  case inet:getifaddrs() of
    {ok, IfAddrs} ->
      hd([ Addr || {_, Opts} <- IfAddrs,
                   {broadaddr, Addr} <- Opts ]);
    {error, _} ->
      io:format("INTERFACE NAME/ADDRS COULD NOT BE ADQUIRED.\n")
  end.

% Es equivalente a correr el comando 'ifconfig' e ir
% filtrando hasta encontrar la IPV4 de la red.
get_local_ipv4() ->
  case inet:getifaddrs() of
    {ok, IfAddrs} ->
      hd([ Addr || {Name, Opts} <- IfAddrs,
                   Name =/= "lo",
                   Name =/= "docker0",
                   {addr, Addr} <- Opts,
                   size(Addr) == 4,
                   Addr =/= {127,0,0,1} ]);
    {error, _} ->
      io:format("INTERFACE NAME/ADDRS COULD NOT BE ADQUIRED.\n")
  end.

% Adiviná que hace.
shared() ->
  case file:list_dir_all("./shared") of
  {ok, Files} -> Files;
  {error, Reason} ->
    io:format("FAILED TO LIST ./shared: ~p~n", [Reason]),
    []
  end.

parse_String(Packet) -> 
  string:tokens(string:trim(Packet)," ").

get_dicc() ->
  %io:format("CLI [~p]: Enviando petición {get_list} a udp_Pid.~n", [self()]), % <-- AÑADIR ESTO
  udp_Pid ! {get_list, self()},
  receive 
    {ok, Dic_nodes, My_ID} ->
      {ok, Dic_nodes, My_ID}
  after 5000 -> io:format("No se pudo conseguir la lista de Nodos"), error
  end.

get_list() -> 
  {ok, Dic, My_ID} = get_dicc(),
  {ok,maps:to_list(Dic),My_ID}.
