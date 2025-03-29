#include <stdio.h>
#include <mpi.h>

void main(int argc, char **argv){
    int rank, size, pasosTotales = 0;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int suma = rank + 16, miValor = rank + 16, valorNuevo;
    int procesoAnterior = (rank - 1 + size) % size;
    int procesoSiguiente = (rank + 1) % size;

    int i;
    for(i = 1; i < size - 1; i *= 2){
        if(rank % (2*i) == 0){
            MPI_Recv(&valorNuevo, 1, MPI_INT, rank + i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            pasosTotales++;
            suma += valorNuevo;
        }
        else if(rank % i == 0){
            MPI_Send(&suma, 1, MPI_INT, rank-i,0,MPI_COMM_WORLD);
            break;
        }

    }

    for(int j = size / 2; j >= 1; j /= 2){
        if(rank % (2*j) == 0){
            MPI_Send(&suma, 1, MPI_INT, rank+j,0,MPI_COMM_WORLD);
        }
        else if(rank % j == 0){
            MPI_Recv(&suma, 1, MPI_INT, rank - j, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

    }

    printf("La suma total es %d (en %d pasos)\n", suma, pasosTotales);

    MPI_Finalize();
}