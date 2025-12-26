%%--------------------------------------------------------------------
%% @doc
%% Módulo para obtener la lista de archivos (archivos regulares) en un directorio,
%% junto con sus tamaños en bytes.
%%
%% Este módulo define la función principal `files_with_sizes/1` la cual:
%%  - Toma como parámetro la ruta de un directorio.
%%  - Utiliza `file:list_dir/1` para obtener el listado de nombres (archivos y directorios)
%%    en la carpeta.
%%  - Para cada nombre, construye la ruta completa y obtiene la información del archivo mediante
%%    `file:read_file_info/1`.
%%  - Si el elemento es un archivo regular (no un subdirectorio, enlace, etc.), extrae su tamaño
%%    y lo almacena en una tupla `{NombreArchivo, TamañoEnBytes}`.
%%  - En caso de error al listar el directorio, devuelve `{error, Reason}`.
%%
%% La inclusión del fichero "file.hrl" mediante -include_lib se realiza para tener acceso a la
%% definición del record `file_info`, el cual contiene la información detallada de cada archivo.
%%--------------------------------------------------------------------
-module(dir_utils).

%% Exportamos la función files_with_sizes/1 para poder usarla desde otros módulos.
-export([files_with_sizes/1]).

%% Incluimos el header file.hrl de la librería kernel para obtener la definición del record file_info.
-include_lib("kernel/include/file.hrl").

%%--------------------------------------------------------------------
%% @doc
%% Función principal que recibe la ruta de un directorio y retorna la lista de archivos
%% junto con sus tamaños.
%%
%% @spec files_with_sizes(Dir :: string()) -> [{string(), integer()}] | {error, term()}.
%%
%% Se utiliza file:list_dir/1 para obtener la lista de nombres de elementos contenidos en el
%% directorio. En caso de éxito, se delega a la función auxiliar get_file_sizes/2 para procesar
%% cada elemento; en caso de error, se devuelve la razón del fallo.
%%--------------------------------------------------------------------
files_with_sizes(Dir) ->
    case file:list_dir(Dir) of
        {ok, Files} ->
            %% Si el listado fue exitoso, se procesan los nombres obtenidos.
            get_file_sizes(Dir, Files);
        {error, Reason} ->
            %% Si ocurre un error al listar el directorio, se devuelve la tupla {error, Reason}.
            {error, Reason}
    end.

%%--------------------------------------------------------------------
%% @doc
%% Función auxiliar que recibe el directorio y una lista de nombres de archivos y/o
%% subdirectorios. Recorre recursivamente la lista y construye las tuplas {Archivo, Tamaño}
%% solo para aquellos elementos que sean archivos regulares.
%%
%% @spec get_file_sizes(Dir :: string(), Files :: [string()]) -> [{string(), integer()}].
%%
%% - Caso base: cuando la lista está vacía se retorna una lista vacía.
%% - Caso recursivo: se procesa el primer elemento de la lista (cabeza) y luego se llama recursivamente
%%   para el resto (cola). Se utiliza filename:join/2 para unir la ruta del directorio y el nombre
%%   del archivo, y file:read_file_info/1 para obtener la información del archivo.
%%
%% Se comprueba que el elemento sea de tipo "regular" (archivo normal) usando el patrón
%% FileInfo#file_info.type == regular; si es así, se extrae el tamaño (FileInfo#file_info.size) y se
%% agrega a la lista. En cualquier otro caso (por ejemplo, si el elemento es un directorio) se omite.
%%--------------------------------------------------------------------
get_file_sizes(_, []) ->
    %% Caso base: no quedan elementos por procesar.
    [];
get_file_sizes(Dir, [File | Rest]) ->
    %% Se construye la ruta completa al elemento usando filename:join/2.
    Path = filename:join(Dir, File),
    %% Se obtiene la información del archivo; file:read_file_info/1 retorna {ok, FileInfo} o error.
    case file:read_file_info(Path) of
        %% Se verifica que la lectura fue exitosa y que el tipo de archivo sea regular.
        {ok, FileInfo} when FileInfo#file_info.type == regular ->
            %% Si es un archivo regular, se crea la tupla {NombreArchivo, Tamaño} y se concatena
            %% con el resultado de procesar el resto de la lista.
            [{File, FileInfo#file_info.size} | get_file_sizes(Dir, Rest)];
        %% Para cualquier otro resultado (por ejemplo, archivos no regulares o errores de lectura),
        %% se omite el elemento y se continúa la recursión.
        _Other ->
            get_file_sizes(Dir, Rest)
    end.
