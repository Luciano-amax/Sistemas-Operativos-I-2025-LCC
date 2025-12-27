% ------------------------------------
% Modulo "nodo" principal de programa.}
% ------------------------------------

-module(nodo).
-export([inicio/0]).
-include("config.hrl").

inicio() ->
    timer:sleep(1000),

    io:format("Leyendo carpeta compartida..."),
    {ok, ListaArchivos} = archivos:scan_compartida(),
    io:format("Completo~n"),
    
    io:format("Obteniendo nombre de Nodo..."),
    {ok, NodoID} = name:inicio_nombre(),
    
    io:format("Creando registro de nodos..."),
    TablaEts = ets:new(tabla_de_nodos, [public,ordered_set, named_table]),
    io:format("Completo~n"),
    
    io:format("Iniciando servidor..."),
    spawn(servidor,inicio, [NodoID]),
    io:format("Completo~n"),
    
    io:format("Iniciando saludo..."),
    spawn(udp,start, [NodoID,TablaEts]),
    io:format("Completo~n"),
    
    timer:sleep(1000),
    cli:shell(NodoID, TablaEts, ListaArchivos).