%%--------------------------------------------------------------------
%% @doc
%% Módulo para manejo de mensajes INVALID_NAME.
%%
%% Se utiliza durante el proceso de registro de un nodo. Si un nodo
%% ya registrado en la red tiene el mismo ID solicitado, se responde
%% al nuevo nodo con un mensaje `INVALID_NAME <ID>`.
%%
%% Provee:
%%   - encode/1: Codifica el mensaje INVALID_NAME.
%%   - decode/1: Decodifica un mensaje INVALID_NAME recibido.
%%   - handle_request/1: Atiende un mensaje NAME_REQUEST y determina si debe responder.
%%--------------------------------------------------------------------
-module(invalid_name).

%% Exportamos las funciones a usar desde otros módulos o tests.
-export([encode/1, decode/1, handle_request/4]).

%% Incluir los records necesarios
-include("../header/records.hrl").

%%--------------------------------------------------------------------
%% @doc
%% Codifica un mensaje INVALID_NAME con un ID.
%%
%% @spec encode(Id :: string()) -> string().
%%
%% Devuelve un string del tipo:
%%     "INVALID_NAME <Id>\n"
%%--------------------------------------------------------------------
encode(Id) when is_list(Id) ->
    lists:flatten(io_lib:format("INVALID_NAME ~s~n", [Id])).

%%--------------------------------------------------------------------
%% @doc
%% Decodifica un mensaje INVALID_NAME recibido.
%%
%% @spec decode(Binary :: string()) ->
%%          {ok, #invalid_name{}} | {error, invalid_format}.
%%--------------------------------------------------------------------
decode(Binary) when is_list(Binary) ->
    case string:tokens(string:trim(Binary), " \t") of
        ["INVALID_NAME", RecvId] -> {ok, #invalid_name{id=RecvId}};
        _ -> {error, invalid_format}
    end.

%%--------------------------------------------------------------------
%% @doc
%% Maneja un NAME_REQUEST y responde con INVALID_NAME si el ID está en uso.
%%
%% @spec handle_request({#name_request{}, {Ip, Port}}) -> ok.
%%
%% - Consulta la lista de nodos en node_registry.
%% - Si ya hay un nodo con el mismo ID, responde con INVALID_NAME
%%   al IP y puerto de origen.
%%--------------------------------------------------------------------
handle_request(ReqId, SrcIp, SrcPort, Socket) ->
    %% Pedimos la lista actual de nodos
    node_registry ! {get_all_nodes, self()},
    receive
        Nodes when is_list(Nodes) ->
            Nodes;
        _ ->
            io:format("Hubo un error al solicitar la lista de nodos~n>"),
            Nodes = []
    end,
    %% Verificamos si ya existe un nodo con ese ID
    case lists:keyfind(ReqId, 1, Nodes) of
        false ->
            ok;  % No hay conflicto, no se responde
        {ReqId, _Ip, _TcpPort} ->
            %% Conflicto: enviamos INVALID_NAME al solicitante
            Bin = encode(ReqId),
            io:format("Invalid name found: ~s~n", [ReqId]),
            gen_udp:send(Socket, SrcIp, SrcPort, Bin)
    end.
