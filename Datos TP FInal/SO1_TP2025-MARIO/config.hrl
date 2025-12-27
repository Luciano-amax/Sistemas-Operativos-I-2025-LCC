% ----------------------------
% Configuracion de Sistema P2P
% ----------------------------

% Archivos y carpetas de "archivos.erl"
-define(CARPETA_COMPARTIDA, "./CarpetaCompartida").
-define(CARPETA_DESCARGAS, "./CarpetaDescargas").

% Configuracion de "name.erl"
-define(NOMBRE_NODO_CARACTERES, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"). % Caracteres disponibles para nombrar nodo.
-define(NOMBRE_NODO_LONGITUD, 4). % Cantidad de caracteres para nombrar nodo.
-define(NOMBRE_NODO_ESPERA, 10). % Tiempo de espera para confirmar ID de nodo.

% Configuracion UDP de "name.erl"
-define(UDP_PUERTO, 12346). % Puerto UDP
-define(UDP_BROADCAST_OPEN, {0, 0, 0, 0}). % IP para abrir socket para broadcast.
-define(UDP_BROADCAST_SEND, {255, 255, 255, 255}). % IP para enviar por broadcast.
-define(UDP_NOMBRE_RESTART, 10). % Espero 10 segundos hasta intentar abrir

% Configuracion TCP de "servidor.erl" y "udp.erl"
-define(LOCALHOST, "localhost").
-define(PUERTO_TCP, 12345).

% Configuracion UDP para "udp.erl"
-define(UDP_INTERVALO_HELLO, 17000). % Tiempo entre segundos para enviar mensaje de HELLO.
-define(UDP_BORRAR_NODO_DIFERENCIA, 40). % Tiempo entre segundos para borrar un nodo del ets.
-define(UDP_BORRAR_NODO_ESPERA, 40000). % Tiempo de espera segundos para comparar diferencia de tiempo entre mensajes HELLO

% Configuracion SHELL de "cli.erl"
-define(SHELL, "ClienteP2P").

