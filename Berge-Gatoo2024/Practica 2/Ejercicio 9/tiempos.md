# Tiempos

Tiempo promedio tras 500 iteraciones del jardin ornamental

Sin sincronizacion: 0.013144\
Usando el algoritmo de Peterson: 0.089486\
Usando incl: 0.018142\
Usando un pthread_mutex_t: 0.043224\
Usando un solo molinete sin multithreading: 0.000959

## Explicaciones

De las implementaciones que usan hilos, la versión sin sincronización es más rápida ya que los hilos pueden ejecutarse libremente sin preocuparse por regiones críticas.

El algoritmo de Peterson es más lento que pthread_mutex,
esto se debe a que los mutex están implementado por hardawre y el
algoritmos de peterson por software (haciendo a este mas rápido).

incl es más rápido que las dos previas ya que esta es una solución optima
para incrementar un contador.

Y cuando hay un solo molinete es mas rápido directamente porque
hay un solo programa corriendo y no hay que hacer "pausas" entre programas.
