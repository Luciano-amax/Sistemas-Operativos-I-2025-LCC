%%--------------------------------------------------------------------
%% @doc
%% Módulo para verificar si un archivo está descargado en el directorio local.
%%
%% Provee:
%%   - is_file_downloaded/2: Verifica si un archivo específico existe en el directorio de descargas.
%%   - get_downloaded_files/1: Obtiene la lista de archivos descargados en un directorio.
%%--------------------------------------------------------------------
-module(file_checker).

-include_lib("kernel/include/file.hrl").

%% Exportamos las funciones principales
-export([is_file_downloaded/1, get_downloaded_files/0]).

%%--------------------------------------------------------------------
%% @doc
%% Determina si un archivo específico está descargado en el directorio dado.
%%
%% @spec is_file_downloaded(FileName :: string(), DownloadsDir :: string()) ->
%%          {found, string()} | not_found.
%%--------------------------------------------------------------------
is_file_downloaded(FileName) ->
    FilePath = filename:join("descargas", FileName),
    case file:read_file_info(FilePath) of
        {ok, FileInfo} when FileInfo#file_info.type == regular ->
            {found, FilePath};
        _Other ->
            not_found
    end.

%%--------------------------------------------------------------------
%% @doc
%% Obtiene la lista de archivos descargados en un directorio.
%% Es un wrapper sobre dir_utils:files_with_sizes/1 que solo devuelve los nombres.
%%
%% @spec get_downloaded_files() ->
%%          {ok, [string()]} | {error, term()}.
%%--------------------------------------------------------------------
get_downloaded_files() ->
    case dir_utils:files_with_sizes("descargas") of
        {error, Reason} ->
            {error, Reason};
        FilesList when is_list(FilesList) ->
            {ok, FilesList}
    end.
