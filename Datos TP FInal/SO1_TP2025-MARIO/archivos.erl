% ----------------------------------------------------------
% Módulo "archivos.erl" para manejos de archivos y carpetas.
% ----------------------------------------------------------

-module(archivos).
-export([scan_compartida/0, buscar_archivo/1, listar_por_wildcard/1]).
-include("config.hrl").


% Retorna una lista de archivos en la carpeta compartida
scan_compartida() ->
    case file:list_dir(?CARPETA_COMPARTIDA) of
        {ok, ListaArchivos} -> 
            {ok, ListaArchivos};
        {error, Reason} -> 
            io:format("Error al leer Carpeta Compartida A01: ~p. Reiniciando...~n", [Reason]),
            scan_compartida()
    end.


listar_por_wildcard(Wildcard) -> 
    List = filelib:wildcard(Wildcard, "CarpetaCompartida"),
    ReturnList = lists:map(
        fun(Archivo) ->
        Path = io_lib:format("./CarpetaCompartida/~s", [Archivo]),
        Size = filelib:file_size(Path),
        {Archivo, Size}
        end
        ,List),
    ReturnList.



buscar_archivo(Archivo) ->
    Path =  io_lib:format("./CarpetaCompartida/~s",[Archivo]),
    {Result,File} = file:read_file(Path),
    if
        Result == ok ->
            Size = filelib:file_size(Path),
            {ok, File, Size};
        true ->
            {Result,File, 0}
    end.
