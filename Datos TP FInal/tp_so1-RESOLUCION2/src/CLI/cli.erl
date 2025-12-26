%%--------------------------------------------------------------------
%% @doc
%% Módulo de interfaz de línea de comandos (CLI) para el sistema P2P.
%%
%% Este módulo permite al usuario interactuar con el sistema mediante
%% comandos de texto. Provee funcionalidades como:
%%  - Mostrar archivos compartidos y descargados
%%  - Listar nodos registrados en la red
%%  - Buscar y descargar archivos (simulados)
%%  - Comando de ayuda
%%--------------------------------------------------------------------
-module(cli).
-export([start_cli/0]).

%Mensaje de bienvenida
%Mensaje de éxito al logearse (el log in es automatico)
%Mostrar lista de archivos descargados y tamaño
%Mostrar lista de archivos compartidos y tamaño
%Mostrar lista de nodos registrados en la red
%Buscar archivos por nombre y mostrar resultados de la busqueda
%Descargar archivos por nombre
%Comando de ayuda para ver todos los comandos

%%--------------------------------------------------------------------
%% @doc
%% Inicia la CLI mostrando un mensaje de bienvenida.
%%--------------------------------------------------------------------
start_cli() ->
    io:format("Bienvenido al cliente de descarga P2P~n"),
    io:format("Conectando...~n"),
    handler_cli().

%%--------------------------------------------------------------------
%% @doc
%% Loop principal que maneja la entrada de comandos del usuario.
%%
%% Cada comando válido invoca una función correspondiente.
%%--------------------------------------------------------------------
handler_cli() ->
    io:format("~nIngrese un comando (help para lista de comandos):~n> "),
    case string:trim(io:get_line("")) of
        "help" ->
            help();
        "descargados" ->
            show_downloads();
        "compartidos" ->
            show_shared();
        "nodos" ->
            show_nodes();
        "buscar" ->
            search_file();
        "descargar" ->
            download_file();
        Otro ->
            io:format("No existe el comando ~p~n>", [Otro])
    end,
    handler_cli().

%%--------------------------------------------------------------------
%% @doc
%% Muestra todos los comandos disponibles en la CLI.
%%--------------------------------------------------------------------
help() ->
    io:format("~nComandos:~n"),
    io:format("help             - Mostrar los comandos~n"),
    io:format("descargados      - Ver archivos descargados~n"),
    io:format("compartidos      - Ver archivos compartidos~n"),
    io:format("nodos            - Ver lista de nodos~n"),
    io:format("buscar           - Buscar archivos por nombre~n"),
    io:format("descargar        - Descargar un archivo por el nombre~n>").

%%--------------------------------------------------------------------
%% @doc
%% Muestra la lista de archivos descargados y sus tamaños.
%%--------------------------------------------------------------------
show_downloads() ->
    case file_checker:get_downloaded_files() of
        {error, Reason} ->
            io:format("Error al obtener archivos descargados: ~p~n>", [Reason]);
        Files ->
            io:format("~nArchivos descargados:~n"),
            {ok, List} = Files,
            lists:foreach(
              fun({Name, Size}) ->
                  io:format(" - ~s (~p bytes)~n", [Name, Size])
              end,
              List
            ),
            io:format(">")
    end.

%%--------------------------------------------------------------------
%% @doc
%% Muestra la lista de archivos compartidos por el nodo y sus tamaños.
%%--------------------------------------------------------------------
show_shared() ->
    case shared_files:get_shared_files() of
        {error, Reason} ->
            io:format("Error al obtener archivos compartidos: ~p~n", [Reason]);
        Files ->
            io:format("~nArchivos compartidos:~n"),
            lists:foreach(
              fun({Name, Size}) ->
                  io:format(" - ~s (~p bytes)~n", [Name, Size])
              end,
              Files
            ),
            io:format(">")
    end.

%%--------------------------------------------------------------------
%% @doc
%% Solicita al `node_registry` la lista de nodos y la imprime.
%%
%% Si el mensaje no llega en 1 segundo, imprime timeout.
%%--------------------------------------------------------------------
show_nodes() ->
    %% Pedimos el listado de nodos al registry
    node_registry ! {get_all_nodes, self()},

    receive
        %% Si llega una lista de nodos, la recorremos
        Nodes when is_list(Nodes) ->
            io:format("\nNodos registrados en la red:\n"),
            lists:foreach(
              fun({Id, Ip, Port}) ->
                  io:format(" - ~s en ~p:~p~n", [Id, Ip, Port])
              end,
              Nodes
            );

        %% Cualquier otro mensaje lo registramos
        Other ->
            io:format("Mensaje inesperado en show_nodes: ~p~n>", [Other])
    after 1000 ->
        %% Timeout
        io:format("Error: timeout al consultar nodos~n")
    end.

%%--------------------------------------------------------------------
%% @doc
%% Realiza una búsqueda de archivos en toda la red P2P
%%--------------------------------------------------------------------
search_file() ->
    io:format("Nombre del archivo a buscar: "),
    Name = string:trim(io:get_line("")),
    search_client:search_in_network(Name).

%%--------------------------------------------------------------------
%% @doc
%% Descarga archivo buscado en la red a partir de un nodo
%%--------------------------------------------------------------------
download_file() ->
    io:format("Nombre del archivo: "),
    FileName = string:trim(io:get_line("")),
    io:format("ID del nodo origen: "),
    NodeId = string:trim(io:get_line("")),
    download_client:download_from_node(FileName, NodeId).
