%%--------------------------------------------------------------------
%% @doc
%% Módulo encargado de obtener los archivos compartidos por el nodo.
%%
%% Este módulo accede al directorio "compartida" y devuelve una lista
%% con los archivos y sus tamaños.
%%
%%-------------------------------------------------------------------
-module(shared_files).
-export([get_shared_files/0, match/1]).

%%--------------------------------------------------------------------
%% @doc
%% Devuelve la lista de archivos compartidos por el nodo.
%%
%% La función accede al directorio `"compartida"` y obtiene una lista
%% de tuplas `{Nombre, Tamaño}` para cada archivo.
%%
%% @spec get_shared_files() -> list({string(), integer()}).
%%--------------------------------------------------------------------
get_shared_files() ->
    dir_utils:files_with_sizes("compartida").

%%%--------------------------------------------------------------
%%% match/1  –  Patrón con comodines  (*  ?)  estilo shell
%%%--------------------------------------------------------------
match(Pattern) ->
    Regex = wildcard_to_re(Pattern),
    Files = get_shared_files(),
    [ {Name, Size}
      || {Name, Size} <- Files,
         re:run(Name, Regex, [{capture, none}]) =:= match ].

%%%--------------------------------------------------------------
%%% Convierte wildcards a regex:
%%%   * → .*
%%%   ? → .
%%%  Y transforma los caracteres para que sean válidos PCRE (Pearl Compatible Regular Expressions)
%%%--------------------------------------------------------------
wildcard_to_re(Str) ->
    Esc = lists:flatmap(fun escape_char/1, Str),
    "^" ++ Esc ++ "$".

escape_char($*)  -> ".*";
escape_char($?)  -> ".";
escape_char($.)  -> "\\.";
escape_char($+)  -> "\\+";
escape_char($")  -> "\\\"";
escape_char('(') -> "\\(";
escape_char(')') -> "\\)";
escape_char('[') -> "\\[";
escape_char(']') -> "\\]";
escape_char('^') -> "\\^";
escape_char('$') -> "\\$";
escape_char('|') -> "\\|";
escape_char($\\) -> "\\\\";
escape_char(C)   -> [C].
