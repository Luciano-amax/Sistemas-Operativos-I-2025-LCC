%%--------------------------------------------------------------------
%% @doc
%% Módulo encargado de escuchar mensajes UDP entrantes.
%%
%% Se encarga de recibir mensajes `HELLO` y `NAME_REQUEST` en la red
%% P2P sobre el puerto 12346, procesarlos y delegar su manejo a los
%% módulos correspondientes.
%%
%% Este listener permanece activo durante toda la vida del nodo.
%%--------------------------------------------------------------------
-module(udp_listener).
-export([start/0]).

%%--------------------------------------------------------------------
%% @doc
%% Inicia el listener UDP abriendo el socket en el puerto 12346.
%%
%% El socket se abre en modo activo, de forma que los mensajes UDP se
%% entregan automáticamente al proceso como mensajes `{udp, ...}`.
%%
%% @spec start() -> ok
%%-------------------------------------------------------------------
start() ->
    {ok, Socket} = gen_udp:open(12346, [binary, {reuseaddr, true}, {broadcast, true}, {active, true}]),
    loop(Socket).

%%--------------------------------------------------------------------
%% @doc
%% Bucle principal que espera y procesa mensajes UDP.
%%
%% Identifica el tipo de mensaje (`HELLO` o `NAME_REQUEST`) y delega
%% su manejo a los respectivos módulos (`hello_handler` o `invalid_name`).
%%
%% @spec loop(Socket :: port()) -> no_return()
%%--------------------------------------------------------------------
loop(Socket) ->
    receive
        {udp, Socket, SrcIp, SrcPort, Bin} ->
            %% Convertimos el binary a string para analizarlo
            MsgBin = binary_to_list(Bin),
            %% io:format("[udp_listener] Mensaje UDP recibido~n"),
            %% Parseamos el mensaje por tokens
            case string:tokens(string:trim(MsgBin), " \t") of
                ["HELLO", Id, Port] ->
                    %% Procesamos un mensaje HELLO
                    hello_handler:process(Id, Port, SrcIp);

                ["NAME_REQUEST", Id] ->
                    %% Procesamos un mensaje NAME_REQUEST
                    %% Puede implicar responder INVALID_NAME si hay conflicto
                    invalid_name:handle_request(Id, SrcIp, SrcPort, Socket);
                _ ->
                    %% Mensaje no reconocido: lo ignoramos
                    ok
            end,
            loop(Socket)
    end.
