-module(main2).
-export([inicioP2P/1, atentoUDP/3, atentoNameRequester/5,getIP/0, atentoTCP/4, comandos/6, descargarArch/5, procesoDeDescargas/5, helloPeriodicos/3, decodeMsg/1, atentoHola/2]).
-export([buscarArchivoP2P/3,accept_loop/3, loop_receiveTCP/3, loop_receiveUDP/4]).
-define(PUERTO, 1235).
-define(CHUNK, 1048576).% tamno que usa el servidor para enviar por chuncks envia cada 1 MG


% getIp: ->{ok,FirstIp} || {error,ipNovalida}
% esta funcion mediante el getif ve las IP disponibles en la maquina
% Donde pasamos a la funcion sacarIpValidos
% donde devuelte una lista de las IP valides(Ipes)
% y de ahi sacamos la primera ip valida, en caso de no haber Ip validas
% ({127,0,0,1},{0,0,0,0},{192,168,0,255} son ip no validas) entonces retorna {error,ipNovlida}
getIP() -> 
    case inet:getif() of
        {ok, Interfaces} ->
            Ips = sacarIpValidos(Interfaces),
            case Ips of
                [FirstIP | _] -> {ok, FirstIP}; % retorno la primera IP valida
                [] -> {error, ipNoValida} % ninguna IP es valida ERROR 
            end;
        _ ->
            error % error a la hora del getIF
    end.


% sacarIpValidos: -> lists
% creo una lista con la Ip de interfaces, donde luego aplico
% la funcion IsValidIp
sacarIpValidos(Interfaces) ->
    lists:filter(fun isValidIp/1, [IP || {IP, _Name, _Broadcast} <- Interfaces]). % agarro interfaces y me creo una lista con las IP


isValidIp({127, 0, 0, 1}) -> false;  % porque esto seria para comunicasion locales, no quiero eso
isValidIp({0, 0, 0, 0}) -> false;    % inválida porque esto es culquier dirrecion
isValidIp({192, 168, 0, 255}) -> false; % broadcast típica
isValidIp(_IP) -> true.


% crecrearIdRandmon:(integrer>0, Lists) -> Lists
% dado un entero positivo y una lista agregamos en la ultima posicion  n elementos aleatorio
% usaremos esta funcion con n = 4 y Lists = [], para crear las claves, si queremos claves mas largas cambiamos el n.
crearIdRandmon(0, List)->List;
crearIdRandmon(N, List)->
    Palabras = "qazxswedcvfrtgbnhyujmkiolpQAZXSWEDCVFRTGBNHYUJMKIOLP0123456789",% 26+26+10=62
    Index = rand:uniform(62),
    Elem = lists:nth(Index, Palabras),
    crearIdRandmon(N-1, lists:append([List, [Elem]])).


% decodeMsg:(Binary)->
% funcion encargada de decodificar los distintos tipos de mensajes
% en caso de ser un mensaje no valido retornara otraPalabra
decodeMsg(Bin)->
    Msg = binary_to_list(Bin), 
    Msg_tokens = string:tokens(Msg," _\n"),
    Msg_tokens2 = string:tokens(Msg, "_"),
    case lists:nth(1,Msg_tokens) of
        "invalid" -> {invalid_name, lists:nth(3, Msg_tokens)}; 
        "name" -> {name_request, lists:nth(3, Msg_tokens)};
        "HELLO" -> {hello,lists:nth(2,Msg_tokens), list_to_integer(lists:nth(3,Msg_tokens))};
        "NOTFOUND" -> "NOTFOUND" ;
        "111" ->   case lists:nth(2,Msg_tokens) of 
                    "-1" -> Path = lists:nth(3,Msg_tokens),              % AGREGAR CASO SEQARG POSITIVO
                            SizeFile = list_to_binary(lists:nth(4,Msg_tokens)),
                            Datos = list_to_binary(lists:nth(4,Msg_tokens2)),
                            {111, -1, Path, SizeFile, Datos};     
                    _-> Indice = list_to_integer(lists:nth(2, Msg_tokens)),
                        Path = lists:nth(3,Msg_tokens), 
                        ChunkSize = list_to_integer(lists:nth(4, Msg_tokens)),
                        Datos = list_to_binary(lists:nth(4,Msg_tokens2)),
                        {111, Indice, Path, ChunkSize, Datos}
                    end;
        "101" -> SizeFile = list_to_binary(lists:nth(2, Msg_tokens)),
                                                ChunkSize = list_to_binary(lists:nth(3, Msg_tokens)),
                                                {101, SizeFile, ChunkSize};                                                 
        "112" ->    errorDescarga;                             

        "download" -> {download_request, lists:nth(3, Msg_tokens),lists:nth(4, Msg_tokens)};

        "SEARCH" -> case lists:nth(2, Msg_tokens) of
                        "REQUEST"->{search_request, lists:nth(3, Msg_tokens), lists:nth(4, Msg_tokens)};
                        "RESPONSE"-> {search_response, lists:nth(3, Msg_tokens), lists:nth(4, Msg_tokens), lists:nth(5, Msg_tokens)}
                    end
    end.


% genrarId:SOcket,Puerto:integre -> {ok,Mapadatos}|error
% Dado un socket de conexion UDP y un puerto
% busca crear una id para el nodo de la compu conectada
% en caso de haber problemas a la hora de querer acceder a las Ip
% retornamos error
generarId(Socket, Puerto,PidOrigen)->
    io:fwrite("creando ID~n"),
    Id = crearIdRandmon(4, []),
    case getIP() of
        {error, ipNoValida} ->error;
        error -> io:fwrite("error al acceder a las IP"),
                    error;
        {ok, Ip}->
            Msg = string:concat("name_request ", Id),
            MsgBin = list_to_binary(Msg),
            gen_udp:send(Socket, {255, 255, 255, 255}, ?PUERTO, MsgBin),
            spawn(?MODULE, atentoNameRequester, [Id, Socket, Puerto, Ip,PidOrigen])
    end.


atentoNameRequester(Id, Socket, Puerto, Ip,PidOrigen) ->
    receive 
        {invalid_name, Id} -> generarId(Socket, Puerto,PidOrigen)
    after 
        5000 ->     io:fwrite("ENTRÉ~p~n",[PidOrigen]),
                    PidOrigen ! {ok,Id,Ip,Puerto}
    end.  

% nameRequester:MapaMyDatos:map,SocketUDP
% Funcion encargada de que cuando un Nodo solicite la ID
% no use la Id de esta maquina, recibe el mensaje
% lo decodifica y se fija si es name_requester
% si lo es se fija en el Id solicitado
% si es igual que el que se encuntra en MapaMyDatos
% le envio invalid_name Id
% si es otro mensaje simplemente lo ignoro
loop_receiveUDP(MapaNodos,MapaTempos,SocketUDP,PidNameRequester)->
    receive 
        {udp, SocketUDP, IP, Port, Message}-> 
            case decodeMsg(Message) of
                {name_request, Id} ->
                    case maps:find(Id, MapaNodos) of 
                        {ok,_Valor} ->
                            Msg=string:concat("invalid_name ",Id),
                            BinMsg=list_to_binary(Msg),
                            gen_udp:send(SocketUDP,BinMsg),
                            loop_receiveUDP(MapaNodos,MapaTempos,SocketUDP,PidNameRequester);
                        error-> 
                            loop_receiveUDP(MapaNodos, MapaTempos,SocketUDP,PidNameRequester)
                    end;
                {hello, Id, Puerto} ->
                    %io:fwrite("PIRO: ~p~n",[Id]),
                    case maps:find(Id, MapaNodos) of
                        {ok, _Valor} ->
                                case maps:find(Id, MapaTempos) of
                                    {ok, PidAtentoHola} ->
                                    PidAtentoHola ! hello,
                                    loop_receiveUDP(MapaNodos, MapaTempos, SocketUDP, PidNameRequester);
                                error ->
                                    %% Si está en MapaNodos pero no en MapaTempos, lo creamos ahora
                                    PidAtentoHola = spawn(?MODULE, atentoHola, [Id, self()]),
                                    loop_receiveUDP(MapaNodos,maps:put(Id, PidAtentoHola, MapaTempos),SocketUDP,PidNameRequester)
                                end;
                    error ->
                    %% Nodo completamente nuevo: crear ambos
                    PidAtentoHola = spawn(?MODULE, atentoHola, [Id, self()]),
                    io:fwrite("agrego: ~p~n",[Id]),
                    loop_receiveUDP(maps:put(Id, {IP, Puerto}, MapaNodos),maps:put(Id, PidAtentoHola, MapaTempos),SocketUDP,PidNameRequester)
                    end;
                {invalid_name, Id} ->
                    PidNameRequester ! {invalid_name,Id},
                    loop_receiveUDP(MapaNodos,MapaTempos,SocketUDP,PidNameRequester);
                _ -> loop_receiveUDP(MapaNodos,MapaTempos,SocketUDP,PidNameRequester)
            end;
        {borrar, Id} -> 
            loop_receiveUDP(maps:remove(Id, MapaNodos), maps:remove(Id, MapaTempos),SocketUDP,PidNameRequester);
        {tengoNodo,Id,Pid}->
            io:fwrite("Entre ~p ~n ",[maps:find(Id, MapaNodos)]),
            case maps:find(Id,MapaNodos) of
                {ok,{Ip,Port}} ->   
                                    io:fwrite("~p~n",[Ip]),
                                    Pid!{okEnviado, Ip, Port},
                                    loop_receiveUDP(MapaNodos,MapaTempos,SocketUDP,PidNameRequester);
                error -> Pid !noConocido,
                        io:fwrite("Nodo no encontrado para clave ~p~n", [Id]),
                        loop_receiveUDP(MapaNodos,MapaTempos,SocketUDP,PidNameRequester)
            end;
        {mapaNodos, Pid} -> Pid ! {mapaNodos,MapaNodos},
                            loop_receiveUDP(MapaNodos,MapaTempos,SocketUDP,PidNameRequester);
        {ok,Id, Ip, Puerto} ->
            loop_receiveUDP(maps:put(Id,{Ip,Puerto}, MapaNodos), MapaTempos,SocketUDP,PidNameRequester)
    end.


atentoUDP(MapaNodos, MapaTempos,PidOriginal)-> %%si esta 
    {ok, SocketUDP} = gen_udp:open(?PUERTO,[binary , {broadcast, true},{active,true},{reuseaddr, true}]),
    % io:fwrite("~nSolicitud de nombre activado~n"),
    PidOriginal!SocketUDP,
    receive
        {pid,PidNameRequester}->
            loop_receiveUDP(MapaNodos,MapaTempos,SocketUDP,PidNameRequester)
    end.
    


atentoHola(Id, PidAtentoUDP) ->
    receive 
        hello -> atentoHola(Id, PidAtentoUDP)
    after
        45000 -> PidAtentoUDP ! {borrar, Id}
    end.


mandarPorPartes(IndeceChunck, RawFile, PosiDeLectura, NuevoSocket, Path)->
    case file:pread(RawFile,PosiDeLectura,(?CHUNK)) of
            {ok,BinMsg}-> Chunk=111,
                    % me formo el mensaje
                        NueMsg1 = string:concat(integer_to_list(Chunk), "_"), % 111_indice_Path_TAMCHUNK_DATOS
                        NueMsg2 = string:concat(NueMsg1, integer_to_list(IndeceChunck)),
                        NueMsg3 = string:concat(NueMsg2, "_"),
                        NueMsg4 = string:concat(NueMsg3, Path),
                        NueMsg5 = string:concat(NueMsg4, "_"),
                        NueMsg6 = string:concat(NueMsg5, integer_to_list((?CHUNK))),
                        NueMsg7 = string:concat(NueMsg6, "_"),
                        NueMsg8 = string:concat(NueMsg7, binary_to_list(BinMsg)),
                        BinMensaje = list_to_binary(NueMsg8),
                    % mensaje formado
                        case gen_tcp:send(NuevoSocket, BinMensaje) of
                            closed -> io:fwrite("EL ARCHIVO QUE LO PIDIO CERRO~n"),
                                        ok;
                            ok->
                        mandarPorPartes(IndeceChunck+1, RawFile, PosiDeLectura+(?CHUNK), NuevoSocket, Path)
                        end;
            eof->
                io:fwrite("TODO EL ARCHIVO FUE ENVIADO~n"),
                MensajeDeFinDeLectura = "111_-2",
                BinMensaje = list_to_binary(MensajeDeFinDeLectura),
                gen_tcp:send(NuevoSocket, BinMensaje);%%no me interesa si cerro
                
            _->Error = "112",
                MensajeError = list_to_binary(Error),
                gen_tcp:send(NuevoSocket, MensajeError) %%aca tampoco
    end.

envioDeArchivo(NombreArch, NuevoSocket, MiId, PidAtentoUDP)->
    PidAtentoUDP ! {tengoNodo, MiId, self()},
    io:fwrite("~p~n",[MiId]),
    receive
        {okEnviado, Ip, Port} -> case gen_tcp:connect(Ip, Port, [binary, {active, true}, {packet, 0}]) of
                                    {ok, NewSocket} ->
                                        io:fwrite("Enviando archvio a ~p~p~n",[Ip,NewSocket]),
                                        Path = string:concat("./compartida/", NombreArch),
                                        {ok,Raw} = file:open(Path, [binary, {raw, read}]),
                                        FileSize = filelib:file_size(Path),
                                        SizeBin = binary:encode_unsigned(FileSize, big),
                                        % formo mensaje
                                        io:fwrite("~p~p~n",[SizeBin,FileSize]),
                                        %Msg1 = string:concat("101_", binary_to_list(SizeBin)),
                                        %Msg2 = string:concat(Msg1, "_"),
                                        %Msg3 = string:concat(Msg2, integer_to_list(?CHUNK)),

                                        
                                        %Msg3 = io_lib:format("101_~s_~s",[binary_to_list(SizeBin),integer_to_list(?CHUNK)]),
                                        Msg1 = list_to_binary("101_"),
                                        Msg2 = list_to_binary("_"),
                                        Msg3 = integer_to_binary(?CHUNK),
                                        BinMsg = <<Msg1/binary,SizeBin/binary,Msg2/binary,Msg3/binary>>,    
                                        %BinMsg = list_to_binary(Msg4),
                                        % ya formado de la forma <<101_(binario del size del archivo)_TAMCHUNK>>
                                        NPath = string:concat("./descargas/", NombreArch),
                                        case gen_tcp:send(NewSocket,BinMsg) of % ya le mande el ok  con el tamano del archivo y del chunk
                                            {error, closed} -> io:fwrite("EL ARCHIVO QUE LO PIDIO CERRO~n"),
                                                    ok;
                                            ok -> 
                                        case FileSize < 4000000 of
                                            true -> % el archivo es menor a 4 Mg lo mando de una
                                                case file:pread(Raw, 0, (?CHUNK)*4) of
                                                {ok,Binario}-> 
                                                            Chunk = 111,
                                                            Indece = -1, % usamos el indice -1 pra indicar que se pasa todo el archivo
                                                        % me formo el mensaje
                                                            BinChunk = integer_to_binary(Chunk),
                                                            BInIndice = integer_to_binary(Indece),
                                                            BInGuionBajo = list_to_binary("_"),
                                                            BinPath = list_to_binary(NPath),% {101,-1,path,size,datos}
                                                            BinMensaje = <<BinChunk/binary,BInGuionBajo/binary,BInIndice/binary,BInGuionBajo/binary,BinPath/binary,BInGuionBajo/binary,SizeBin/binary,Binario/binary>>, % algo de la pinta <<"101_-1_path_(tamano)_(datos)">> 
                                                        % mensaje formado
                                                            gen_tcp:send(NewSocket, BinMensaje), % no me interea si en esta parte cerro
                                                            file:close(Raw);
                                                {error,Reason}-> 
                                                    io:fwrite ("error de pread ~p~n",Reason),
                                                    Error = "112",
                                                    MensajeError = list_to_binary(Error),
                                                    gen_tcp:send(NewSocket, MensajeError),%% aca tampoco
                                                    file:close(Raw)
                                                end;
                                            false ->% lo mando por parte
                                                mandarPorPartes(0, Raw, 0, NewSocket, NPath),
                                                file:close(Raw)
                                                % mandarPorPartes(indece=0, Raw(mi archivo))
                                        end
                                        end;
                                    {error, Reason} -> io:fwrite("~p~n",[Reason])
                                end
    end.

procesoDeDescargas(NuevoSocket, NombreArch, IdOrigen, PidAtentoUDP,MiId)->
                    io:fwrite("empezo proceso de descargaaaaaaaaaaaaaaa~n"),
                    Path = "compartida",
                    case file:list_dir(Path) of
                        {ok, ListArchivos}->
                            case lists:member(NombreArch, ListArchivos) of
                                false -> io:fwrite("No poseo el archivo ~p informo que no lo tengo~n", [NombreArch]),
                                        Msg = "NOTFOUND",
                                        BinMsg = list_to_binary(Msg),
                                        gen_tcp:send(NuevoSocket, BinMsg); % le aviso que no esta el arhivo, no llamo atentoTCP porque ya lo hice recursivo
                                true-> io:fwrite("Poseo el archivo, iniciendo envio~n"),
                                        envioDeArchivo(NombreArch, NuevoSocket, IdOrigen, PidAtentoUDP)
                                        % envioDeArchivo(SocketTCP(el que tiene el archivo), NombreArch, NuevoSocket(a quiense lo tengo que mandar))
                            end;

                        {error, Reason}->io:fwrite("Error al ver archivos razon:~p~n",[Reason])
                    end.

inicioDescargaTotal(BinDatos, Path)->
    {ok, Raw} = file:open(Path, [binary, read, write, raw]),
    file:pwrite(Raw, 0, BinDatos),
    file:close(Raw).

inicioCopiado(BinDatos, Path, ChunkSize, Indece)->
    {ok, Raw} = file:open(Path, [binary, read, write, raw]),
    file:pwrite(Raw, Indece * ChunkSize, BinDatos),
    file:close(Raw).

buscarMisArch(MyId,NombreArch,Id,PidAtentoUDP)->
    Path = "compartida",
    case file:list_dir(Path) of
        {ok, ListaMisArchivos} -> case lists:member(NombreArch,ListaMisArchivos) of 
                                true -> PidAtentoUDP ! {tengoNodo,Id,self()},
                                        receive         %MANDARTCP
                                            {okEnviado, Ip, Port} -> 
                                                 case gen_tcp:connect(Ip, Port, [binary, {packet, 0}, {active, true}]) of
                                                            {ok, NuevoSocket}->
                                                            Msg = list_to_binary(io_lib:format("SEARCH_RESPONSE ~s ~s ~s~n", [MyId, NombreArch,integer_to_list(filelib:file_size(string:concat("./compartida/",NombreArch)))])),
                                                            gen_tcp:send(NuevoSocket, Msg);
                                                            %gen_tcp:close(NuevoSocket);
                                                            {error,Reason} -> io:fwrite("Error en la conexion razon:~p~n", [Reason])   
                                                        end
                                        end;
                                false -> io:fwrite("No tengo el archivo: ~p ",[NombreArch])
                                end;                                                
        {error, Reason} -> io:fwrite("Error al ver archivo razon:~p~n", [Reason])
    end.


% funcion encargada de estar atento a los mensajes
% de download, para eso crea un proceso que se encarga de enviar los datos
loop_receiveTCP(SocketTCP, PidAtentoUDP, MyId)->
    receive 
        {tcp, SocketTCP, Msg}->
            io:fwrite("~p",[Msg]),
            case decodeMsg(Msg) of
                {download_request, NombreArch,NodoOrigen}->
                    io:fwrite("~p~p~n", [NombreArch,NodoOrigen]),
                    spawn(?MODULE, procesoDeDescargas, [SocketTCP, NombreArch, NodoOrigen, PidAtentoUDP,MyId]), % me llega un mensaje me fijo si es para descargar
                    loop_receiveTCP(SocketTCP,PidAtentoUDP,MyId);      % y creo un proceso q atiende a esa solicitud, y me llamo otra vez para antender mas

                "NOTFOUND"->io:fwrite("archivo no encontrado en el nodo~n"),
                            loop_receiveTCP(SocketTCP,PidAtentoUDP,MyId);

                {111, -1, Path, _SizeFile, Datos}->inicioDescargaTotal(Datos, Path),
                                            loop_receiveTCP(SocketTCP,PidAtentoUDP,MyId); % {101,-1,path,size,datos}

                {111, Indice, Path, ChunkSize, Datos}->inicioCopiado(Datos,Path,ChunkSize,Indice),
                                                    loop_receiveTCP(SocketTCP,PidAtentoUDP,MyId); % 111_indice_Path_TAMCHUNK_DATOS

                {101, _SizeFile, _ChunkSize}->io:fwrite("EL NODO POSEE el archivo Inicio la descarga~n"),
                                            loop_receiveTCP(SocketTCP,PidAtentoUDP, MyId);
                {search_request, Id, NombreArch}-> buscarMisArch(MyId, NombreArch, Id, PidAtentoUDP), 
                                                loop_receiveTCP(SocketTCP,PidAtentoUDP, MyId);
                {search_response,NodoId,NombreArch,Size}-> io:fwrite("El Nodo ~s posee el archivo ~s de tamaño ~s~n",[NodoId,NombreArch,Size]),
                                                        loop_receiveTCP(SocketTCP,PidAtentoUDP, MyId);
                errorDescarga -> io:fwrite("error en la descarga"),
                                 loop_receiveTCP(SocketTCP,PidAtentoUDP, MyId)
            end;
            {tcp_closed, SocketTCP} -> io:fwrite("CONEXIOM CERRADA"),
                                        loop_receiveTCP(SocketTCP,PidAtentoUDP, MyId);
            {tcp_error, SocketTCP, Reason}  -> io:fwrite("error socket tcp ~p~n", [Reason]),
                                            loop_receiveTCP(SocketTCP,PidAtentoUDP, MyId)
        end.

atentoTCP(PidAtentoUDP, MyId, Puerto, PidOrigen) ->
    {ok, ListenSocket} = gen_tcp:listen(Puerto, [binary, {active, true}, {reuseaddr, true}]),
    PidOrigen ! {pid, ListenSocket},
    accept_loop(ListenSocket, PidAtentoUDP, MyId).


% funcion encargada de imprimir los archivos que se encuentran en la carpeta compartir
verArch()->
    Path = "compartida",
    case file:list_dir(Path) of
        {ok, Archivos} -> io:fwrite("tus archivos son ~p~n", [Archivos]);
        {error, Reason} -> io:fwrite("Error al ver archivo razon:~p~n", [Reason])
    end.



    

% funcion encargada de descargar un archivo de nombre X en el Nodo con Y clave
% reviso para eso la lista e mis nodos conocidos, si no esta no puedo hacer nada
% caso contrario le pregunto si posee dicho archivo donde debo esperar a la respuesta
% si me responde que no tiene dicho archivo no lo descargo, caso contrario
% me envia el archivo y lo guardo en la carpeta de descargas mia

descargarArch(Clave,NombreArch,_SocketTCP,PidAtentoUDP,MiId)-> % primero le pregunto si tiene el archivo , desp espero respuesta
    io:fwrite("Empezo la funcion de descarga~n"),
    PidAtentoUDP ! {tengoNodo,Clave,self()},
    receive 
        noConocido->io:fwrite("No hay registros de un nodo con clave ~p~n",[Clave]);
        {okEnviado, Ip, Port}->
            case gen_tcp:connect(Ip, Port, [binary, {packet, 0}, {active, true}]) of
                {ok, NuevoSocket}->
                        Msg = io_lib:format("download_request ~s ~s", [NombreArch,MiId]),
                        MsgBin = list_to_binary(Msg),
                        gen_tcp:send(NuevoSocket, MsgBin);
                        % atentoDescargas(SocketTCP); %hago un nuevo proceso asi el cliente no se queda bloqueado>>> NO hace falta porque esto ya esta en un nuevo proceso
                 {error,Reason} -> io:fwrite("Error en la conexion razon:~p~n", [Reason])   
            end
        after 10000 ->
            io:fwrite("Nodo no encontrado para clave ~p~n", [Clave])
    end.  

buscarArchivoP2P(NombreArchivo, MiId, PidAtentoUDP) ->
    Path = "compartida",
    %Path2 = "descargas",
    {ok, Archivos_comp} = file:list_dir(Path),
    %Archivos_desc = file:list_dir(Path),
    case lists:member(NombreArchivo, Archivos_comp) of
        true ->
            io:fwrite("No es necesaria la busqueda, lo tengo! ~n");
        false ->
            PidAtentoUDP ! {mapaNodos, self()},
            receive 
                {mapaNodos,MapaNodos} -> 
                    maps:foreach(fun (Id, {Ip, Puerto}) -> busquedaMensaje(Id, {Ip, Puerto}, NombreArchivo, MiId ) end, MapaNodos)
            end    
    end.

busquedaMensaje(Id, _IpPuerto, _NombreArchivo, MiId) when Id =:= MiId ->
    % No enviar el mensaje a sí mismo
    ok;
busquedaMensaje(_Id, {Ip, Puerto}, NombreArchivo, MiId) ->
    MsgBin = list_to_binary(io_lib:format("SEARCH_REQUEST ~s ~s", [MiId, NombreArchivo])),
    case gen_tcp:connect(Ip, Puerto, [binary, {active, true}, {packet, 0}]) of
        {ok, NewSocket} ->
            gen_tcp:send(NewSocket, MsgBin),
            %gen_tcp:close(NewSocket),
            ok;
        {error, Reason} ->
            io:format("Error conectando a ~p:~p -> ~p~n", [Ip, Puerto, Reason]),
            {error, Reason}
    end.



% esta funcion se encarga de recibir comandos por teclado y ejecutarlos, en caso de un comando no valido
% avisa
comandos(MiId,SocketUDP, SocketTCP,PidAtentoUDP,PidHelloPeridiodico,PidAtentoTCP)->
    io:fwrite("Ingrese comando:"),
    Comando = io:get_line(""),
    Comando_tokens = string:tokens(Comando," \n"),
    case lists:nth(1,Comando_tokens) of 
        "ID" -> io:fwrite("El Id de esta maquina es ~p~n", [MiId]),
                comandos(MiId, SocketUDP, SocketTCP,PidAtentoUDP,PidHelloPeridiodico,PidAtentoTCP);
        "Lista_mis_archivos" -> verArch(),
                                comandos(MiId,SocketUDP,SocketTCP,PidAtentoUDP,PidHelloPeridiodico,PidAtentoTCP);
        
        "salir" ->   exit(PidHelloPeridiodico, kill),
                    exit(PidAtentoTCP, kill),
                    exit(PidAtentoUDP,kill),
                     gen_udp:close(SocketUDP),
                     gen_tcp:close(SocketTCP);
        
        "descargar" ->  Clave = lists:nth(2,Comando_tokens),
                        NombreArch = lists:nth(3,Comando_tokens),
                        io:fwrite("CLAVE:~p ~p~n",[Clave,NombreArch]),
                        spawn(?MODULE, descargarArch, [Clave, NombreArch, SocketTCP,PidAtentoUDP,MiId]),
                        comandos(MiId, SocketUDP, SocketTCP,PidAtentoUDP,PidHelloPeridiodico,PidAtentoTCP);

        "buscar" ->  NombreArch = lists:nth(2,Comando_tokens),
                        spawn(?MODULE,buscarArchivoP2P , [NombreArch, MiId, PidAtentoUDP]),
                        comandos(MiId, SocketUDP, SocketTCP,PidAtentoUDP,PidHelloPeridiodico,PidAtentoTCP);
        "nodos" -> PidAtentoUDP ! {mapaNodos, self()},
                    receive 
                        {mapaNodos, Mapa} -> io:fwrite("~p~n", [Mapa]),
                        comandos(MiId, SocketUDP, SocketTCP,PidAtentoUDP,PidHelloPeridiodico,PidAtentoTCP)
                    end;
        _-> io:fwrite("Los comandos válidos son:\nID \nLista_mis_archivos\ndescargar <IdNodoExterno> <Archivo>\nbuscar <Archivo>\nsalir\n"),
            comandos(MiId, SocketUDP, SocketTCP,PidAtentoUDP,PidHelloPeridiodico,PidAtentoTCP)

    end.


% esta funcion se encarga de hacer un HELLO periodicamente con este formato:
% HELLO <nodo_ID_X> <puerto_tcp>\n
helloPeriodicos(SocketUDP, Puerto, MiId)-> 
    MsgBin = list_to_binary(io_lib:format("HELLO ~s ~s", [MiId, integer_to_list(Puerto)])),
    gen_udp:send(SocketUDP, {255, 255, 255, 255}, ?PUERTO, MsgBin),
    timer:sleep(15000),
    helloPeriodicos(SocketUDP, Puerto, MiId).

accept_loop(ListenSocket, PidAtentoUDP, MyId) ->
    case gen_tcp:accept(ListenSocket) of
        {ok, Socket} ->
            ok = inet:setopts(Socket, [{active, true}]),
            Pid = spawn (?MODULE, loop_receiveTCP, [Socket, PidAtentoUDP, MyId]),
            gen_tcp:controlling_process(Socket, Pid),
            accept_loop(ListenSocket, PidAtentoUDP, MyId);
        Error ->
            io:format("Error aceptando conexión TCP: ~p~n", [Error]),
            ok
    end.


inicioP2P(Puerto)->
    PidAtentoUDP=spawn(?MODULE, atentoUDP, [maps:new(),maps:new(),self()]),
    receive
        SocketUDP-> io:fwrite("ENTRE~n"),
    PidNameRequester = generarId(SocketUDP,?PUERTO,self()),
    PidAtentoUDP!{pid,PidNameRequester},
    PidNameRequester!{ok,PidAtentoUDP},
    receive 
        {ok, MiId, Ip, _Port} -> PidAtentoUDP!{ok,MiId,Ip,Puerto},
                        PidHelloPeridiodico=spawn(?MODULE,helloPeriodicos,[SocketUDP, Puerto, MiId]),
                        PidAtentoTCP=spawn(?MODULE, atentoTCP, [PidAtentoUDP,MiId,Puerto,self()]),
                        receive 
                            {pid,SocketTCP}->
                        comandos(MiId, SocketUDP, SocketTCP,PidAtentoUDP,PidHelloPeridiodico,PidAtentoTCP)
                        end
    end
end.