% --------------------------------------- SERVER -------------------------------------------- %

-module(srv_tcp).
-include("defs.hrl").
-export([start_server/3, server/1, listen/1, manager/2,enviar_archivo/3,enviar_chunks/4]).

% Abre un Socket TCP, avisa que se abrió el server.
start_server(PORT_TCP, BcastIP, ID) ->
  {ok, TCP_Socket} = gen_tcp:listen(PORT_TCP, [binary,{packet, 0}, {active, false}, {reuseaddr, true}]),
  io:format("[~s]: SERVER IS UP on PORT ~p!\n", [ID, PORT_TCP]),
  Pid=spawn(discovery, recv_and_hello, [PORT_TCP, ID, BcastIP]),
  register(udp_Pid,Pid),
  server(TCP_Socket).

% Encargado de aceptar conexiones TCP entrantes
% y derivarlas cada una a un proceso apartado.
server(Socket) ->
  case gen_tcp:accept(Socket) of
    {ok, CSocket} ->
      spawn_link(?MODULE, listen, [CSocket]),
      io:format("\nCLIENT CONNECTED!...\n"),
      server(Socket);
    {error, Reason} ->
      io:format("SERVER_ERROR: ~p.~n", [Reason]),
      gen_tcp:close(Socket)
  end.

% Encargado de manejar la conexión TCP.
% Si la instrucción del cliente es 'compleja',
% (y no es un error) se la dejamos a la función
% manager() para que la interprete.

% Puede ser que no sirva para nada
listen(CSocket) ->
  case gen_tcp:recv(CSocket, 0) of
    {ok, Bin} ->
      Packet = binary_to_list(Bin),
      spawn(?MODULE, manager, [Packet, CSocket]),
      listen(CSocket);
    {error, closed} ->
      io:format("LISTEN_ERROR: conexión cerrada.\n"),
      gen_tcp:close(CSocket);
    {error, Reason} ->
      Error = "LISTEN_ERROR: " ++ atom_to_list(Reason) ++ "\n",
      io:format(Error),
      gen_tcp:close(CSocket)
  end.

% Maneja los pedidos de busqueda "SEARCH_REQUEST"
% y descarga "DOWNLOAD_REQUEST".
manager(Packet, CSocket) ->
  case string:tokens(string:trim(Packet), " ") of
  
    % Se busca el archivo en el directorio de archivos compartidos.
    % Si no se encuentra se envia NOTFOUND.
    ["DOWNLOAD_REQUEST" | Name] ->
      Filename = utils:flatten_string("",Name),
			Archivo = ?SHARED_DIR ++ "/" ++ Filename,
      case file:read_file(Archivo) of
        {ok, Bin} ->
          enviar_archivo(CSocket, Bin, ?BLOCKSIZE);
        % Aca deberiamos enviar la hash.
        _ ->
          gen_tcp:send(CSocket, <<?NOTFOUND:8>>)
      end;

    % Ver como mandar respuesta al nodo particular 
    ["SEARCH_REQUEST", _Node_Id, FileName] -> 
      case filelib:wildcard(FileName, "./shared") of
        [] -> gen_tcp:send(CSocket, <<?NOTFOUND:8/big>>);
        List ->
          {ok, _Dic_nodes, My_ID} = utils:get_dicc(),
          lists:foreach(fun(Res) -> Msg = "SEARCH_RESPONSE " ++ My_ID ++ " " ++ Res ++ " " ++ 
                                    integer_to_list(filelib:file_size("./shared/" ++ Res)) ++ "\n",
                                    gen_tcp:send(CSocket, list_to_binary(Msg)) end, List)
      end;

    % Excepcion: entrada inesperada 
    _ ->
      gen_tcp:send(CSocket, "Entrada incorrecta, intenta de nuevo.\n")
end.

enviar_archivo(Socket, Bin, Chunk_size) ->
  Tam = byte_size(Bin),
  Paquete_inicial = <<?OK:8, Tam:32/big, Chunk_size:32/big>>,
  gen_tcp:send(Socket,Paquete_inicial),
  enviar_chunks(Socket, Bin, Chunk_size, 0).

enviar_chunks(Socket, Bin, Chunk_size, Indice) ->
  case Bin of
    <<Chunk:Chunk_size/binary, Resto/binary>> ->
      Packet = <<?CHUNK:8, Indice:32/big, Chunk/binary>>,
      gen_tcp:send(Socket, Packet),
      enviar_chunks(Socket, Resto, Chunk_size, Indice + 1);
    _ -> 
      Packet = <<?CHUNK:8, Indice:32/big, Bin/binary>>,
      gen_tcp:send(Socket , Packet),
      ok
  end.
