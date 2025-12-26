-module(cli).
-include("defs.hrl").
-export([cli_loop/0,salir/0, id_nodo/0, listar_mis_archivos/0]).
-export([buscar/1, send_search/4, loop_receive/1, loop_tcp_recv/2]).
-export([descargar/2, descargar_archivo/4, recv_descarga/2, recv_archivo/3, recv_chunks/4]).

cli_loop() ->
  Comand = string:tokens(string:trim(io:get_line(">> "), trailing, "\n"), " "),
  case Comand of
    ["salir"] -> salir();

    ["id_nodo"] -> 
      id_nodo(),
      cli_loop();

    ["listar_mis_archivos"] ->
      listar_mis_archivos(),
      cli_loop();

    ["buscar"|Tl] ->
      FileName = utils:flatten_string("",Tl),
      buscar(FileName),
      cli_loop();

    ["descargar"| Tl] ->
      Len = length(Tl),
      NonString_FileName = lists:droplast(Tl),
      FileName = utils:flatten_string("",NonString_FileName),
      descargar(FileName, lists:nth(Len,Tl)),
      cli_loop();

    _ ->
      io:fwrite("Comando invalido\n"),
      cli_loop()
  end.

% Desconecta al nodo de la red.
salir() -> ok.

% Muestra la Id del nodo
id_nodo() ->
case utils:get_dicc() of
    {ok, _Dic_nodes, My_ID} ->
      io:fwrite("Id: ~p\n", [My_ID]);
    _ -> % Cualquier otra respuesta, incluyendo el 'ok' del timeout
      io:fwrite("Error: No se pudo obtener el ID del nodo. El proceso de descubrimiento no respondio a tiempo.\n")
  end.

% Crea una string con los nombres de
% los archivos a partir de una lista.
listar_mis_archivos() ->
  case utils:shared() of
    [] ->
      io:fwrite("Error, No hay archivos compartidos\n");
    Files ->
      lists:foreach(fun(X) -> io:fwrite("~p\n", [X]) end, Files)
  end.

% Envia un SEARCH_REQUEST a todos los nodos en la red.
% Luego lista por terminal los resultados enviados por los
% demas nodos.
buscar(FileName) ->
  {ok, List, My_ID} = utils:get_list(),
  %io:format("Nodes found: ~p~n", [List]),
  lists:foreach(fun(X) -> spawn(?MODULE, send_search, [X,My_ID,FileName,self()]) end, List),
  Search_Results = loop_receive([]),
  io:fwrite("Resultados de la Busqueda:\n"),
  lists:foreach(
		fun(X) -> io:format("~p~n", [X]), timer:sleep(500)	
  			end, Search_Results),
  ok.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Funciones para busqueda de archivos CLI %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
send_search(X,Id,Filename,Pid) -> 
  case X of
    {_Node_Id,{Node_IP,Node_TCP_Port}} -> % Node_Id es Wildcard por que no se usa xd
      Puerto = list_to_integer(Node_TCP_Port),
      case gen_tcp:connect(Node_IP,Puerto,[binary,{active,false},{packet, 0}]) of
        {ok, Sock} -> 
          Msg ="SEARCH_REQUEST " ++ Id ++ " " ++ Filename ++"\n",
          io:fwrite(Msg),
          Bin = list_to_binary(Msg),
          gen_tcp:send(Sock,Bin),
          loop_tcp_recv(Sock, Pid);
    {error, Reason} ->
      io:fwrite("Error en el search connect ~p~n", [Reason])
    end;
    _ -> ok
  end.

loop_tcp_recv(Sock,Pid) ->
  case gen_tcp:recv(Sock,0,5000) of
    {ok,Packet} -> 
      StringPacket = binary_to_list(Packet),
      ParsedString = utils:parse_String(StringPacket),
      case ParsedString of
        [_Res, _Id, _File, _Size] ->
          Pid ! {resultadoBusqueda, ParsedString};
        [Res, Id | Tl] ->
          {File, Size} = utils:flatten_string_space("", Tl),
          Pid ! {resultadoBusqueda, [Res, Id, File, Size]}
      end,
      loop_tcp_recv(Sock,Pid);
    {error,timeout} ->
      gen_tcp:close(Sock), 
      ok;
    {error,Reason}-> io:fwrite("~p\n",[Reason])
  end.

loop_receive(Search_Results)->
  receive 
    {resultadoBusqueda, Response} ->
      loop_receive([Response | Search_Results])
  after 3000 -> 
    lists:reverse(Search_Results)
  end.

% Envia DOWNLOAD_REQUEST al nodo indicado,
% Espera recibir el tamaño del archivo y lo descarga.
descargar(FileName, NodoOrigen) ->
{ok, Dic_nodes, _My_ID} = utils:get_dicc(),
case maps:find(NodoOrigen, Dic_nodes) of
  {ok, {Origin_IP, Origin_Port}} ->
    {Puerto , _} = string:to_integer(Origin_Port),
      case gen_tcp:connect(Origin_IP, Puerto, [binary, {packet, 0}, {active, false}]) of
        {ok, Sock} ->
          Msg = "DOWNLOAD_REQUEST " ++ FileName ++ "\n",
          gen_tcp:send(Sock, list_to_binary(Msg)),
          recv_descarga(Sock, FileName);
        {error, Reason} ->
          io:format("No se pudo conectar: ~p\n", [Reason])
      end;
  error ->
    io:format("Nodo no encontrado: ~p\n", [NodoOrigen])
  end.
    
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Funciones para descarga de archivos CLI %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
recv_descarga(Sock, FileName) ->
  case gen_tcp:recv(Sock, 1) of
    {ok,<<?OK:8>>} -> 
	{ok, <<Tam:32/big, ChunkSize:32/big>>} = gen_tcp:recv(Sock,8),
      	spawn(?MODULE, descargar_archivo, [Sock, FileName, Tam, ChunkSize]);
    {ok, <<?NOTFOUND:8>>} ->
      io:format("Archivo no encontrado\n");
    {error, closed} ->
      io:format("Conexion cerrada\n");
    {error, Reason} ->
      io:format("Error: ~p\n", [Reason]);
		Unexpected -> 
			io:format("Respuesta inesperada del socket ~p~n", [Unexpected])
  end.

descargar_archivo(Sock,Nombre_archivo,Cant_bytes,Chunk_size) ->
  case Cant_bytes of
    0 -> io:format("Archivo vacio.\n"), ok;
     % reconstruir archivo. 
     % [Aca podemos implementar la verificacion de integridad, recibiendo el hasheo del archivo y comparandolo con el reconstruido]
     % Si la comparacion da ok reconstruir, caso contrario tirar error.
    _ -> % formato <<CHUNK, indice:32/big, chunk:SizeChunk/big>>
      {ok, Mapa} = recv_archivo(Sock,Cant_bytes,Chunk_size),
      Archivo_reconstruido = reconstruir_archivo(Mapa, 0, []),
      file:write_file(Nombre_archivo, Archivo_reconstruido),
      io:format("Archivo recibido completo [~p bytes escritos]~n", [byte_size(list_to_binary(Archivo_reconstruido))])
  end.

recv_archivo(Sock, Cant_bytes, Chunk_size) ->
  recv_chunks(Sock,Cant_bytes,Chunk_size,#{}).

recv_chunks(_Sock, 0, _Chunk_size, Mapa) ->
  {ok, Mapa};
recv_chunks(Sock, Cant_bytes, Chunk_size, Mapa) ->
  To_read = min(Chunk_size, Cant_bytes),
  case { gen_tcp:recv(Sock, 5), gen_tcp:recv(Sock,To_read) } of
    { {ok, <<?CHUNK:8/big, Index:32/big>>}, {ok, Bin} } ->
      recv_chunks(Sock, Cant_bytes - To_read, Chunk_size, maps:put(Index, Bin, Mapa));
    {Error1, Error2} -> 
      io:format("Error el recibir: ~p ~p~n", [Error1,Error2]),
      {Error1, Error2}
  end.  

reconstruir_archivo(Mapa, Indice, Acc) ->
  case maps:get(Indice,Mapa, undefined) of
    undefined -> lists:reverse(Acc); % Cuando nos vamos de los indices disponibles, la damos vuelta y la devolvemos
    Bin -> reconstruir_archivo(Mapa, Indice + 1, [Bin | Acc])
  end.
