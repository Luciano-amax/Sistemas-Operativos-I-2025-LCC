#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 12

void main(int argc, char **argv){
    int a[] = {1,2,3,4,5,6,7,8,9,10,11,12};
    int local_sum = 0, global_sum = 0;
    int segment_size, index, end_index, rank, size;


    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    segment_size = N / size;
    int resto = N % size; 
    
    int* local_a = malloc(segment_size * sizeof(int));
    MPI_Scatter(a, segment_size, MPI_INT, local_a, segment_size, MPI_INT, 0, MPI_COMM_WORLD);
    
    for(int i = 0; i < segment_size; i++){
    	local_sum += local_a[i];
    }
    
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    free(local_a);
    
    if(rank == 0){
    	//Sumo los que falta, al usar scatter todos reciben lo mismo
    	//Hago que solo lo sume el proceso que recibe el resultado de reduce
    	for(int i = 0; i < resto; i++)
    	      global_sum += a[(segment_size*size) + i];
    
    	printf("La suma es %d\n", global_sum);
    }


    MPI_Finalize();
}
