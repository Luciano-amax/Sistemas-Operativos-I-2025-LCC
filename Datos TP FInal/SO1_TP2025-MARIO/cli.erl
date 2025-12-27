% ----------------------------------------------------------
% Módulo "cli.erl" para manejos de shell de clientes
% ----------------------------------------------------------

-module(cli).
-export([shell/3, imprimir/1, recibir_archivo/2,recibir_chunks/2, iniciar_busqueda/5, iniciar_descarga/3]).

-include("config.hrl").

% Inicio de shell
shell(NodoID, TablaEts, ListaArchivos) ->
    io:format("~s> ", [?SHELL]),
    case io:get_line("") of
        eof ->
            io:format("Entrada cerrada. Saliendo.~n"),
            ok;
        {error, ErrorDescription} ->
            io:format("Error de argumentos C01: ~p ~n", [ErrorDescription]),
            shell(NodoID, TablaEts, ListaArchivos);
        Linea ->
            MsgTrim = string:trim(Linea),
            ToList = string:tokens(MsgTrim," "),
            Comando = lists:nth(1,ToList),
            case Comando of
                "nodoID" -> 
                    io:format("NodoID: ~s~n",[NodoID]),
                    shell(NodoID, TablaEts, ListaArchivos);
                "ayuda" ->
                    ayuda(),
                    shell(NodoID, TablaEts, ListaArchivos);
                "lista" ->
                    imprimir(ListaArchivos),
                    shell(NodoID, TablaEts, ListaArchivos);
                "exit" ->
                    io:format("Terminando cliente.~n");
                "descargar" ->
                    if
                        length(ToList) == 3 ->
                            NodoOrigen =  lists:nth(3,ToList),
                            Archivo  = lists:nth(2,ToList),
                            Elem = ets:lookup(TablaEts,NodoOrigen),
                            case Elem of 
                                [] -> 
                                    io:format("El NodoID es incorrecto~n");
                                [{_,{IP,Port,_}}] ->
                                    spawn(cli,iniciar_descarga,[IP,Port, Archivo])
                            end;
                        true -> io:format("Los argumentos son incorrectos~n")
                    end,
                    timer:sleep(1000),
                    shell(NodoID, TablaEts, ListaArchivos);
                    
                "buscar" ->
                    if
                        length(ToList) == 2 ->
                            NombreArchivo = lists:nth(2,ToList),
                            EtsList = ets:tab2list(TablaEts),
                            lists:foreach(
                            fun({IdConsulta,{Ip,Port,_}}) ->
                                spawn(cli,iniciar_busqueda,[NodoID,NombreArchivo,IdConsulta,Ip,Port])
                            end
                            ,EtsList),
                            timer:sleep(1000);
                        true ->
                            io:format("Error argumentos erroneos.~n")
                    end,
                    shell(NodoID, TablaEts, ListaArchivos);
                _ ->
                    io:format("Error comando desconocido.~n"),
                    shell(NodoID, TablaEts, ListaArchivos)
            end
    end.

ayuda() ->
    io:format("Comandos disponibles:~n"),
    io:format("  nodoID                       - Muestra el NodoID propio~n"),
    io:format("  lista                        - Lista archivos locales compartidos~n"),
    io:format("  buscar <patrón>              - Busca archivos en la red~n"),
    io:format("  descargar <archivo> <nodo>   - Descarga un archivo de otro nodo~n"),
    io:format("  ayuda                        - Muestra esta ayuda~n"),
    io:format("  exit                         - Finaliza el nodo~n").

% Muestra lista de archivos
imprimir(ListaArchivos) ->
    io:format("Lista de archivos:~n"),
    lists:map(
        fun(Nombre) -> 
        io:format("~s~n", [Nombre]) end, 
        ListaArchivos).



iniciar_busqueda(NodoID,NombreArchivo,IdConsulta,Ip,Port) ->
    case gen_tcp:connect(Ip,Port,[{reuseaddr, true}, {active,false}], 1000) of
        {ok, Socket} ->
            Msg = io_lib:format("SEARCH_REQUEST ~s ~s~n",[NodoID, NombreArchivo]),
            gen_tcp:send(Socket,Msg),
            case gen_tcp:recv(Socket,0,1000) of 
                {ok, Response} ->
                    ResponseTrim = string:trim(Response),
                    List = string:tokens(ResponseTrim, " "),
                    Archivo = lists:nth(3, List),
                    io:format("-Archivo: ~s    -Nodo: ~s~n", [Archivo, IdConsulta]);
                {error, _} ->
                    ok
            end,
            gen_tcp:close(Socket);
        _ -> 
            ok
    end.

    
iniciar_descarga(IP,Port, NombreArchivo) ->
    case gen_tcp:connect(IP, Port,[binary, {reuseaddr, true}, {active,false}], 1000) of
        {ok, Socket} ->
            Msg = io_lib:format("DOWNLOAD_REQUEST ~s~n",[NombreArchivo]),
            case gen_tcp:send(Socket,Msg) of
                ok ->
                    recibir_archivo(Socket,NombreArchivo);
                {error, Reason} ->
                    io:format("Error en send: ~s~n",[Reason])
            end,
            gen_tcp:close(Socket);
        {error, Reason} ->
            io:format("Error en connect: ~s~n",[Reason])
    end.




recibir_archivo(Socket,NombreArchivo) ->
    case gen_tcp:recv(Socket, 1, 2000) of 
        {ok, <<101>>} -> 
            MB = 1024*1024,
            {_,SizeBin} = gen_tcp:recv(Socket, 4),
            <<Size:32/integer-big>>  = SizeBin,
            if 
                Size > (MB*4) ->
                    gen_tcp:recv(Socket, 4), 
                    recibir_chunks(Socket,NombreArchivo);
                true -> 
                    {ok, Data} = gen_tcp:recv(Socket, Size),
                    Path = io_lib:format("./CarpetaDescargas/~s",[NombreArchivo]),
                    file:write_file(Path, Data),
                    io:format("~nLa descarga ha finalizado~n")
            end;
        {ok, <<112>>} ->
            io:format("El archivo solicitado no esta disponible~n");
        {ok, _} -> 
            io:format("Los datos recibidos son incorrenctos~n");
        {error,Reason} ->  io:format("Error~s~n", [Reason])
    end.


recibir_chunks(Socket,NombreArchivo) ->
    Path = io_lib:format("./CarpetaDescargas/~s",[NombreArchivo]),
    case gen_tcp:recv(Socket, 1, 1000) of
        {ok, <<111>>} ->
            gen_tcp:recv(Socket,2),  %Es el indice del chunk, que no nos interesa
            {ok, SizeChunkBin} = gen_tcp:recv(Socket,4),
            <<SizeChunk:32/integer-big>> = SizeChunkBin,
            {ok, Data} = gen_tcp:recv(Socket, SizeChunk),
            file:write_file(Path, Data,[append]),
            recibir_chunks(Socket,NombreArchivo);
        {ok, _} ->
            io:format("Los datos recibidos son incorrenctos~n");
        {error, timeout} ->
            io:format("~nLa descarga ha finalizado~nClienteP2P> ");
        _ ->
            io:format("Error en la descarga del archivo~n")
    end.
    


                    
