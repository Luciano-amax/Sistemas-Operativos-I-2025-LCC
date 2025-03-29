#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <omp.h>

int main(){
	int i = 10;
	#pragma omp parallel private(i)
	{
		printf("Thread %d: i = %d\n", omp_get_thread_num(), i);
		i = 1000;
	}	

	printf("Valor de i = %d\n", i);
	return 0;
}
/*
El valor de i no esta inicializado, por lo tanto muestra otra cosa.
En el master thread muestra un numero aleatorio, en el resto 0

*/
