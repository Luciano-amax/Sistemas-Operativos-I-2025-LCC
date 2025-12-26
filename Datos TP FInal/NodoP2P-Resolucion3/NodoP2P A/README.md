# Nodo - Red LAN Distribuida Peer-to-Peer
Trabajo Práctico Final - SO I

- Di Santis Franco 
- Cicerchia Juan Ignacio
- Federici Tomas
- Mareli Matias


## Instrucciones:

Corre el comando `make` dentro del directorio para conectarte a la red de nodos.

Comandos aceptables como cliente:
- id_nodo  - *Retorna la ID del nodo servidor al cual está conectado.*
- listar_archivos  - *Lista los archivos que el nodo servidor tiene para compartir.*
- salir  - *Cierra la conexión con el nodo servidor.*
- buscar `Nombre_Archivo`  - *Busca el archivo `Nombre_Archivo` en la red*
- descargar `Nombre_Archivo`  `ID_Nodo`  - *Descarga el archivo `Nombre_Archivo` desde el nodo `ID_Nodo`*

> [!TIP]
> El puerto default es "12345". Puede ser alterado corriendo el comando `make run PORT = numero_puerto_deseado`.
> Corre el comando `make clear` al finalizar la ejecución para limpiar los archivos generados automáticamente.
> Corre el comando `make rebuild` volver a ejecutar limpiando los archivos generados.
