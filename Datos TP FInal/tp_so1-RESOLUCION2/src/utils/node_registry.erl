%%--------------------------------------------------------------------
%% @doc
%% Módulo que mantiene un registro local de nodos conocidos en la red P2P.
%%
%% Provee:
%%  - `start_link/0`: Inicia el proceso y lo registra con el nombre `node_registry`.
%%  - `add_node/3`: Agrega un nodo al registro.
%%  - `get_all_nodes/0`: Devuelve la lista completa de nodos registrados.
%%
%%--------------------------------------------------------------------
-module(node_registry).

%% Exportamos las funciones a usar desde otros módulos o tests.
-export([start_link/0, add_node/3, get_all_nodes/0, remove_node/1]).

%% Función interna principal que maneja las peticiones.
-export([loop/1]).

%%--------------------------------------------------------------------
%% @doc
%% Inicia el proceso que mantiene el registro de nodos.
%% El proceso se registra bajo el nombre `node_registry` para que pueda ser
%% accedido globalmente desde otros módulos.
%%
%% El estado inicial es un mapa vacío: `#{}`.
%%
%% @spec start_link() -> ok.
%%--------------------------------------------------------------------
start_link() ->
    io:format("[node_registry] Iniciando proceso de registro...~n"),
    State = #{},
    register(node_registry, spawn(fun() -> loop(State) end)).


%%--------------------------------------------------------------------
%% @doc
%% Agrega un nodo al registro local.
%% Si el nodo ya existía, se reemplaza su IP y puerto.
%% Este mensaje se envía de forma asincrónica (no espera respuesta).
%%
%% @spec add_node(Name :: string(), IP :: tuple(), Port :: integer()) -> ok.
%%--------------------------------------------------------------------
add_node(Name, IP, Port) ->
    node_registry ! {add_node, Name, IP, Port}.

%%--------------------------------------------------------------------
%% @doc
%% Devuelve la lista de todos los nodos registrados actualmente.
%% Cada nodo se representa como una tupla `{Name, IP, Port}`.
%% Esta función es sincrónica: espera una respuesta con la lista.
%%
%% @spec get_all_nodes() -> [{string(), tuple(), integer()}] | timeout.
%%-------------------------------------------------------------------
get_all_nodes() ->
    node_registry ! {get_all_nodes, self()},
    receive
      Response -> Response
    after 1000 ->
      timeout
    end.

remove_node(Name) ->
    node_registry ! {remove_node, Name}.

%%--------------------------------------------------------------------
%% @doc
%% Loop principal del proceso `node_registry`.
%%
%% Mantiene un mapa con entradas del tipo:
%%     #{ Name => {IP, Port} }
%%
%% Responde a los siguientes mensajes:
%%   - `{add_node, Name, IP, Port}`: agrega o actualiza un nodo.
%%   - `{remove_node, Name}`: elimina un nodo si existe.
%%   - `{get_all_nodes, Pid}`: responde al `Pid` con la lista de nodos registrados.
%%
%% @spec loop(Map :: map()) -> no_return().
%%-------------------------------------------------------------------
loop(Map) ->
    receive
      {add_node, Name, IP, Port} ->
        %% io:format("[node_registry] Agregando nodo ~p~n", [Name]),
        NewMap = Map#{Name => {IP, Port}},
        loop(NewMap);
      {get_all_nodes, Pid} ->
        %io:format("[node_registry] Enviando lista de nodos a ~p~n", [Pid]),
        NodeList = [{Name, IP, Port} || {Name, {IP, Port}} <- maps:to_list(Map)],
        Pid ! NodeList,
        loop(Map);
      {remove_node, Name} ->
        %% io:format("[node_registry] Eliminando nodo ~p~n", [Name]),
        NewMap = maps:remove(Name, Map),
        loop(NewMap)
    end.

