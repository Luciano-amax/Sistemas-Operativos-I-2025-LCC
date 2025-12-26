-module(main).
-export([inicioP2P/1, atentoUDP/3, getIP/0, atentoTCP/3, comandos/6, descargarArch/4, procesoDeDescargas/3, helloPeriodicos/3, decodeMsg/1, atentoHola/2]).
-export([buscarArchivoP2P/3,accept_loop/3]).
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
    Msg_tokens = string:tokens(Msg," _"),
    case lists:nth(1,Msg_tokens) of
        "invalid" -> {invalid_name, lists:nth(3, Msg_tokens)}; 
        "name" -> {name_request, lists:nth(3, Msg_tokens)};
        "HELLO" -> {hello,lists:nth(2,Msg_tokens), list_to_integer(lists:nth(3,Msg_tokens))};
        "NOTFOUND" -> "NOTFOUND" ;
        "111" ->   case lists:nth(2,Msg_tokens) of 
                    "-1" -> Path = lists:nth(3,Msg_tokens),              % AGREGAR CASO SEQARG POSITIVO
                            SizeFile = list_to_binary(lists:nth(4,Msg_tokens)),
                            Datos = list_to_binary(lists:nth(5, Msg_tokens)),
                            {111, -1, Path, SizeFile, Datos};     
                    _-> Indice = list_to_integer(lists:nth(2, Msg_tokens)),
                        Path = lists:nth(3,Msg_tokens), 
                        ChunkSize = list_to_integer(lists:nth(4, Msg_tokens)),
                        Datos = list_to_binary(lists:nth(5, Msg_tokens)),
                        {111, Indice, Path, ChunkSize, Datos}
                    end;
        "101" -> SizeFile = list_to_binary(lists:nth(2, Msg_tokens)),
                                                ChunkSize = list_to_binary(lists:nth(3, Msg_tokens)),
                                                {101, SizeFile, ChunkSize};                                                 
        "112" ->    errorDescarga;                             

        "download" -> {download_request, lists:nth(3, Msg_tokens)};

        "SEARCH" -> case lists:nth(2, Msg_tokens) of
                        "REQUEST"->{search_request, lists:nth(3, Msg_tokens), lists:nth(4, Msg_tokens)};
                        "RESPONSE"-> {search_response, lists:nth(3, Msg_tokens), lists:nth(4, Msg_tokens), lists:nth(5, Msg_tokens)}
                    end
    end.


% recividorNameRequester:(Id:str,Ip:ip,Puerto:integrer,Socket) -> {ok,Map}
% funcion encargada de recivir {invalid_name,Id},en caso de recivirlo tengo
% que crear otro Id entoncemos llamo a generarId
% si es otro mensaje me pongo en el receive nuevamente llamandome otra vez
% si pasan 5 segundos y no recivi un mensaje se entiende que el Id es valido entonces
% retorno {ok,Mapadato}, donde mapadato es un mapa cuya clave es el Id y dentro tiene
% otro mapa con las claves ip y port que guardan dicha informacion
recividorNameRequester(Id, Ip, Puerto, Socket)->
    receive 
        {udp, _OtroSocket, _IP, _Port, Message} ->
                case decodeMsg(Message) of
                        {invalid_name, Id} -> generarId(Socket, Puerto);
                        _ -> recividorNameRequester(Id, Ip, Puerto, Socket) %%recibo otro mensaje puede ser porq se coecto otro nodo
                end
    after 
        5000-> MapaDato = #{Id => {Ip, Puerto}},
        {ok, MapaDato, Id}
    end.


% genrarId:SOcket,Puerto:integre -> {ok,Mapadatos}|error
% Dado un socket de conexion UDP y un puerto
% busca crear una id para el nodo de la compu conectada
% en caso de haber problemas a la hora de querer acceder a las Ip
% retornamos error
generarId(Socket, Puerto)->
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
            recividorNameRequester(Id, Ip, Puerto, Socket)
    end.
    

% nameRequester:MapaMyDatos:map,SocketUDP
% Funcion encargada de que cuando un Nodo solicite la ID
% no use la Id de esta maquina, recibe el mensaje
% lo decodifica y se fija si es name_requester
% si lo es se fija en el Id solicitado
% si es igual que el que se encuntra en MapaMyDatos
% le envio invalid_name Id
% si es otro mensaje simplemente lo ignoro
atentoUDP(SocketUDP,MiId)->
    {ok,Message}=gen_udp:recv(SocketUDP,0),
    case decodeMsg(Message) of
        {name_request,MiId}-> Msg=list_to_binary(tring:concat("invalid_name ",Id)),
                            gen_udp:send(SocketUDP,Msg),
                            atentoUDP(SocketUDP,MiId);
        {hello,Id,Puerto}->
            



atentoUDP(MapaNodos, MapaTempos, SocketUDP)-> %%si esta  
    % io:fwrite("~nSolicitud de nombre activado~n"),
    {ok,Message}=gen_udp:recv(SocketUDP,0), 
             io:fwrite("Mensaje recibido: ~p~n", [Message]),
            case decodeMsg(Message) of
                {name_request, Id} ->
                    case maps:find(Id, MapaNodos) of 
                        {ok,_Valor} ->
                            Msg = string:concat("invalid_name ",Id),
                            MsgBin = list_to_binary(Msg),
                            gen_udp:send(SocketUDP, IP, Port,MsgBin),
                            atentoUDP(MapaNodos, MapaTempos,SocketUDP);
                        error-> 
                            atentoUDP(MapaNodos, MapaTempos, SocketUDP)
                    end;
                {hello, Id, Puerto} ->
                    io:fwrite("recivi un hola de ~p~n",[Id]),
                    case maps:find(Id, MapaNodos) of
                        {ok, _Valor} ->
                            PidAtentoHola = maps:get(Id, MapaTempos),
                            PidAtentoHola ! {hello},  %%
                            atentoUDP(MapaNodos, MapaTempos, SocketUDP);
                        error ->
                            PidAtentoHola = spawn(?MODULE, atentoHola, [Id, self()]),
                            atentoUDP(maps:put(Id,{IP,Puerto}, MapaNodos), maps:put(Id, PidAtentoHola, MapaTempos), SocketUDP)
                    end;
                _ -> atentoUDP(MapaNodos, MapaTempos, SocketUDP)
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

envioDeArchivo(NombreArch, NuevoSocket)->
    Path = string:concat("./compartida/", NombreArch),
    {ok,Raw} = file:open(Path, [binary, {raw, read}]),
    FileSize = filelib:file_size(Path),
    SizeBin = binary:encode_unsigned(FileSize, big),
    % formo mensaje
    Msg1 = string:concat("101_", binary_to_list(SizeBin)),
    Msg2 = string:concat(Msg1, "_"),
    Msg3 = string:concat(Msg2, integer_to_list(?CHUNK)),
    BinMsg = list_to_binary(Msg3),
    % ya formado de la forma <<101_(binario del size del archivo)_TAMCHUNK>>
    NPath = string:concat("./descargas/", NombreArch),
    case gen_tcp:send(NuevoSocket,BinMsg) of % ya le mande el ok  con el tamano del archivo y del chunk
        closed -> io:fwrite("EL ARCHIVO QUE LO PIDIO CERRO~n"),
                ok;
        ok->
    case FileSize < 4000000 of
        true -> % el archivo es menor a 4 Mg lo mando de una
            case file:pread(Raw, 0, (?CHUNK)*4) of
            {ok, BinMsg}-> Chunk = 111,
                        Indece = -1, % usamos el indice -1 pra indicar que se pasa todo el archivo
                    % me formo el mensaje
                        NueMsg1 = string:concat(integer_to_list(Chunk), "_"),
                        NueMsg2 = string:concat(NueMsg1, integer_to_list(Indece)),
                        NueMsg3 = string:concat(NueMsg2, "_"),
                        NueMsg4 = string:concat(NueMsg3, binary_to_list(SizeBin)),
                        NueMsg5 = string:concat(NueMsg4, "_"),
                        NueMsg6 = string:concat(NueMsg5, NPath),
                        NueMsg7 = string:concat(NueMsg6, "_"),
                        NueMsg8 = string:concat(NueMsg7, binary_to_list(BinMsg)),% {101,-1,path,size,datos}
                        BinMensaje = list_to_binary(NueMsg8), % algo de la pinta <<"101_-1_path_(tamano)_(datos)">> 
                    % mensaje formado
                        gen_tcp:send(NuevoSocket, BinMensaje), % no me interea si en esta parte cerro
                        file:close(Raw);
            _->Error = "112",
                MensajeError = list_to_binary(Error),
                gen_tcp:send(NuevoSocket, MensajeError),%% aca tampoco
                file:close(Raw)
            end;

        false ->% lo mando por parte
            mandarPorPartes(0, Raw, 0, NuevoSocket, NPath),
            file:close(Raw)
            % mandarPorPartes(indece=0, Raw(mi archivo))
    end
    end.

procesoDeDescargas(NuevoSocket, Msg, NombreArch)->
                    Path = "compartida",
                    case file:list_dir(Path) of
                        {ok, ListArchivos}->
                            case lists:member(NombreArch, ListArchivos) of
                                false -> io:fwrite("No poseo el archivo ~p informo que no lo tengo~n", [NombreArch]),
                                        Msg = "NOTFOUND",
                                        BinMsg = list_to_binary(Msg),
                                        gen_tcp:send(NuevoSocket, BinMsg); % le aviso que no esta el arhivo, no llamo atentoTCP porque ya lo hice recursivo
                                true-> io:fwrite("Poseo el archivo, iniciendo envio~n"),
                                        envioDeArchivo(NombreArch, NuevoSocket)
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
                                            {Ip,Port} -> case gen_tcp:connect(Ip, Port, [binary, {packet, 0}, {active, true}]) of
                                                            {ok, NuevoSocket}->
                                                            Msg = list_to_binary(io_lib:format("SEARCH_RESPONSE ~s ~s ~s~n", [MyId, NombreArch,integer_to_list(file:size(NombreArch))])),
                                                            gen_tcp:send(NuevoSocket, Msg),
                                                            gen_tcp:close(NuevoSocket);
                                                            {error,Reason} -> io:fwrite("Error en la conexion razon:~p~n", [Reason])   
                                                        end
                                        end
                                end;                                                
        {error, Reason} -> io:fwrite("Error al ver archivo razon:~p~n", [Reason])
    end.


% funcion encargada de estar atento a los mensajes
% de download, para eso crea un proceso que se encarga de enviar los datos
atentoTCP(SocketTCP,PidAtentoUDP,MyId)->
    {ok,Msg}=gen_tcp:recv(SocketTCP,1000000000),
            case decodeMsg(Msg) of
                {download_request, NombreArch}->
                    spawn(?MODULE, procesoDeDescargas, [SocketTCP, NombreArch]), % me llega un mensaje me fijo si es para descargar
                    atentoTCP(SocketTCP,PidAtentoUDP,MyId);      % y creo un proceso q atiende a esa solicitud, y me llamo otra vez para antender mas

                "NOTFOUND"->io:fwrite("archivo no encontrado en el nodo~n"),
                            atentoTCP(SocketTCP,PidAtentoUDP,MyId);

                {111, -1, Path, _SizeFile, Datos}->inicioDescargaTotal(Datos, Path),
                                            atentoTCP(SocketTCP,PidAtentoUDP,MyId); % {101,-1,path,size,datos}

                {111, Indice, Path, ChunkSize, Datos}->inicioCopiado(Datos,Path,ChunkSize,Indice),
                                                    atentoTCP(SocketTCP,PidAtentoUDP,MyId); % 111_indice_Path_TAMCHUNK_DATOS

                {101, _SizeFile, _ChunkSize}->io:fwrite("EL NODO POSEE el archivo Inicio la descarga~n"),
                                            atentoTCP(SocketTCP, PidAtentoUDP, MyId);
                {search_request, Id, NombreArch}-> buscarMisArch(MyId, NombreArch, Id, PidAtentoUDP), 
                                                atentoTCP(SocketTCP, PidAtentoUDP, MyId);
                {search_response,NodoId,NombreArch,Size}-> io:fwrite("El Nodo ~s posee el archivo ~s de tamaño ~s~n",[NodoId,NombreArch,Size]),
                                                        atentoTCP(SocketTCP, PidAtentoUDP, MyId)
            end.


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

descargarArch(Clave,NombreArch,_SocketTCP,PidAtentoUDP)-> % primero le pregunto si tiene el archivo , desp espero respuesta
    io:fwrite("Empezo la funcion de descarga~n"),
    PidAtentoUDP ! {tengoNodo,Clave,self()},
    receive 
        noConocido->io:fwrite("No hay registros de un nodo con clave ~p~n",[Clave]);
        {Ip,Port}->
            case gen_tcp:connect(Ip, Port, [binary, {packet, 0}, {active, true}]) of
                {ok, NuevoSocket}->
                        Msg = lists:append("download_request ", NombreArch),
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
    MsgBin = list_to_binary(io_lib:format("SEARCH_REQUEST ~s ~s~n", [MiId, NombreArchivo])),
    case gen_tcp:connect(Ip, Puerto, [binary, {active, true}, {packet, 0}]) of
        {ok, NewSocket} ->
            gen_tcp:send(NewSocket, MsgBin),
            gen_tcp:close(NewSocket),
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
                        spawn(?MODULE, descargarArch, [Clave, NombreArch, SocketTCP,PidAtentoUDP]),
                        comandos(MiId, SocketUDP, SocketTCP,PidAtentoUDP,PidHelloPeridiodico,PidAtentoTCP);

        "buscar" ->  NombreArch = lists:nth(2,Comando_tokens),
                        spawn(?MODULE,buscarArchivoP2P , [NombreArch, MiId, PidAtentoUDP]),
                        comandos(MiId, SocketUDP, SocketTCP,PidAtentoUDP,PidHelloPeridiodico,PidAtentoTCP);
        _-> io:fwrite("Los comandos válidos son:\nID \nLista_mis_archivos\ndescargar <IdNodoExterno> <Archivo>\nbuscar <Archivo>\nsalir\n"),
            comandos(MiId, SocketUDP, SocketTCP,PidAtentoUDP,PidHelloPeridiodico,PidAtentoTCP)

    end.


% esta funcion se encarga de hacer un HELLO periodicamente con este formato:
% HELLO <nodo_ID_X> <puerto_tcp>\n
helloPeriodicos(SocketUDP, Puerto, MiId)-> 
    MsgBin = list_to_binary(io_lib:format("HELLO ~s ~s", [MiId, integer_to_list(Puerto)])),
    gen_udp:send(SocketUDP, {255, 255, 255, 255}, ?PUERTO, MsgBin),
    io:fwrite("mande Un Hola~n"),
    timer:sleep(15000),
    helloPeriodicos(SocketUDP, Puerto, MiId).

accept_loop(SocketTCP, PidAtentoUDP, MyId) ->
    case gen_tcp:accept(SocketTCP) of
        {ok,Socket}->
    spawn(?MODULE, atentoTCP, [Socket, PidAtentoUDP, MyId]),
    accept_loop(SocketTCP, PidAtentoUDP, MyId);
    _-> listo
    end.


inicioP2P(Puerto)->
    {ok, SocketUDP} = gen_udp:open(?PUERTO,[binary , {broadcast, true},{active,false},{reuseaddr, true}]),
    {ok, Map, MiId} = generarId(SocketUDP, ?PUERTO),% arreglar
    {ok, SocketTCP} = gen_tcp:listen(Puerto, [binary, {active, false},{reuseaddr, true}]),
    PidHelloPeridiodico=spawn(?MODULE,helloPeriodicos,[SocketUDP, Puerto, MiId]),
    PidAtentoUDP=spawn(?MODULE, atentoUDP, [Map,maps:new(),SocketUDP]),% se encarga de estar atento a las solisitudes de nombre
    PidAtentoTCP=spawn(?MODULE, atentoTCP, [SocketTCP,PidAtentoUDP,MiId]),
    spawn(?MODULE, accept_loop, [SocketTCP, PidAtentoUDP, MiId]), 
    comandos(MiId, SocketUDP, SocketTCP,PidAtentoUDP,PidHelloPeridiodico,PidAtentoTCP), %
    ok.