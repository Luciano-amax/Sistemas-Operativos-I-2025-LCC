===========================================
# TRABAJO PRÁCTICO DE SISTEMA P2P EN ERLANG
===========================================

## Descripción general

  Este sistema P2P está desarrollado en Erlang como parte de un trabajo práctico de Sistemas Operativos I. Cada nodo actúa como cliente y servidor, ofreciendo y solicitando archivos dentro de una LAN mediante sockets TCP para transferencia de archivos y UDP para descubrimiento de nodos. El sistema está compuesto por módulos especializados: nodo, servidor, udp, name, archivos, cli, entre otros. Se hace uso de concurrencia con spawn, sincronización de nodos con ets, y mensajes formateados según un protocolo definido.


##  Como ejecutar el programa

  Para ejecutar el programa primero compilar con "make all". Luego abrir erlang con "erl" y ejecutar mediante "nodo:inicio()."
  
  Se deben almacenar archivos a compartir en una carpeta "CarpetaCompartida", y debe existir una carpeta "CarpetaDescargas".
   

## Comandos para Makefile
  
  make all -> compila todos los .erl
  
  make clean -> borra los .beam

  make dumpclean -> borra los .dump


## Comandos para de interacción con sistema
Comandos disponibles para  el "ClienteP2P>":
    nodoID                       - Nombre del nodo
    lista                        - Lista archivos locales compartidos
    buscar <patrón>              - Busca archivos en la red
    descargar <archivo> <nodo>   - Descarga un archivo de otro nodo
    ayuda                        - Muestra esta ayuda
    exit                         - Finaliza el nodo


## Lista de códigos de error

Este sistema P2P en Erlang permite compartir archivos en una LAN. Cada nodo cumple roles de servidor y cliente, usando sockets TCP para transferencias y UDP para descubrimiento y negociación de nombre. A continuación se listan errores detectables en tiempo de ejecución, categorizados por subsistema.

------------------------------------------------------------
[MODULO: archivos.erl]
------------------------------------------------------------
  
  A30 - Error 30 al leer Carpeta Compartida
  > Fallo al listar archivos compartidos (file:list_dir).

------------------------------------------------------------
[MODULO: name.erl]
------------------------------------------------------------
  
  N01 - Error al abrir socket UDP N01
  > Fallo al abrir socket UDP para NAME_REQUEST (gen_udp:open).

  N02 - Error al enviar nombre N02
  > Fallo al enviar mensaje NAME_REQUEST por UDP.

------------------------------------------------------------
[MODULO: servidor.erl]
------------------------------------------------------------

    S01 - Error de conexión S01
    > Fallo al iniciar el servidor TCP (gen_tcp:listen).

    S02 - Error de conexión S02
    > Fallo al aceptar una conexión TCP entrante (gen_tcp:accept).

    S03 - Error de envío S03
    > Fallo al enviar el mensaje <<112>> (NOTFOUND) por archivo no encontrado.

    S04 - Error de envío S04
    > Fallo al enviar una respuesta SEARCH_RESPONSE.

    S05 - Error TCP S05
    > Error genérico del socket TCP (ej. desconexión inesperada del cliente).

    S06 - Error de mensaje no reconocido S06
    > Mensaje malformado o no esperado recibido en handlerCliente.

    S07 - Error de envío S07
    > Fallo al enviar encabezado de archivo grande (> 4MB) con código 101.

    S08 - Error de envío S08
    > Fallo al enviar archivo completo en una sola operación (<= 4MB).

    S09 - Error de envío S09
    > Fallo al enviar un chunk parcial del archivo.

    S10 - Error de envío S10
    > Fallo al enviar el último chunk del archivo.

------------------------------------------------------------
[MODULO: udp.erl]
------------------------------------------------------------

    U01 - Error al abrir socket UDP (HELLO)
    > gen_udp:open falló para HELLO broadcaster o receptor.

    U02 - Error inesperado en mensaje UDP
    > Se recibió un mensaje UDP malformado o con tipo desconocido.

------------------------------------------------------------
[MODULO: cli.erl]
------------------------------------------------------------

    C01 - Error: Comando desconocido
    > El usuario ingresó un comando inválido.

    C02 - Error: Línea malformada
    > No se pudo parsear correctamente una línea de entrada en CLI.

    C03 - Error al iniciar búsqueda/descarga
    > Fallo al ejecutar comando de búsqueda o descarga (por falta de argumentos o error interno).

============================================================
