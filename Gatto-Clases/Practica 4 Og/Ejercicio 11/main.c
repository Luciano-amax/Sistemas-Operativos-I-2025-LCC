#include <stdio.h>
#include <mpi.h>

void main(int argc, char **argv){
    int rank, size, pasosTotales = 0;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int suma = rank, Valor = rank;
    int procesoAnterior = (rank - 1 + size) % size;
    int procesoSiguiente = (rank + 1) % size;

    for(int i = 0; i < size - 1; i++){
        pasosTotales++;
        
        MPI_Send(&Valor, 1, MPI_INT, procesoSiguiente, 0, MPI_COMM_WORLD);
        MPI_Recv(&Valor, 1, MPI_INT, procesoAnterior, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        suma += Valor;
    }

    printf("La suma total es %d (en el thread %d)\n", suma, rank);

    MPI_Finalize();
}
