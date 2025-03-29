# Tiempos

Tiempo promedio tras 500 iteraciones del jardin ornamental

Sin sincronizacion: 0.013144\
Usando el algoritmo de Peterson: 0.089486\
Usando incl: 0.018142\
Usando un pthread_mutex_t: 0.043224\
Usando un solo molinete sin multithreading: 0.000959\

---

Usando el algoritmo de la panaderia: 0.033541

Ademas, se usa mas memoria debido a las banderas que se usan para asignar el
"turno" maximo.
