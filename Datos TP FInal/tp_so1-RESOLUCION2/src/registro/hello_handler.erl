
%%--------------------------------------------------------------------
%% @doc
%% Módulo encargado de manejar mensajes HELLO entrantes.
%%
%% Su única función pública es `process/3`, que recibe el ID del nodo,
%% su puerto TCP y la IP desde la cual llegó el mensaje UDP
%%--------------------------------------------------------------------
-module(hello_handler).

%% Exportamos la función process/3 para poder usarla desde otros módulos.
-export([process/3]).

%%--------------------------------------------------------------------
%% @doc
%% Procesa un mensaje HELLO recibido de otro nodo.
%%
%% @spec process(Id :: string(), Port :: integer(), SrcIp :: tuple()) -> ok
%%
%% Elimina cualquier entrada previa con el mismo Id y luego registra
%% el nodo con los datos actualizados (Id, IP, Puerto).
%%--------------------------------------------------------------------
process(Id, PortStr, SrcIp) ->
    Port = list_to_integer(PortStr),
    %% 1. Eliminar nodo anterior con el mismo ID (si existe)
    node_registry ! {remove_node, Id},
    %% 2. Agregar el nodo actualizado
    node_registry ! {add_node, Id, SrcIp, Port},
    ok.

%% process({#hello{id=Id, port=Port}}, SrcIp) -> ok
