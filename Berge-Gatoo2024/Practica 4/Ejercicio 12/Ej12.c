#include <stdio.h>
#include <mpi.h>

int ilog(int n) {
    int ans = 0;
    for(int i=0;i<32;i++)
        if(n & (1<<i))
            ans = i;
    return ans;
}

int main(int argc, char **argv)
{
    int num_procs, my_id;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    // Sumo los rangos de todos los procesos
    int sum = my_id;

    /*
        Ejemplo del esquema de paso de mensajes
        Los procesos siempre envian su suma a procesos
        de rango menor
        El proceso 0 recibe la suma final y la imprime
        
         _______
        |___    |___
        |   \   |   \
        |\  |\  |\  |\
        0 1 2 3 4 5 6 7

        Alternativamente, se podria implementar usando mpi_reduce
        MPI_Reduce(&my_id,&sum,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD)
    */
   
    for(int i=0;i<=ilog(num_procs);i++)
    {
        // Recibo el mensaje de un proceso
        if(!(my_id & (1<<i)))
        {
            int val=0;
            int source = my_id + (1<<i);
            if(source < num_procs)
            {
                MPI_Recv(&val,1,MPI_INT,source,MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                sum+=val;
            }
        }
        // Envio el mensaje al proceso, el proceso 0 es el unico que no envia mensajes
        else
        {
            int dest = my_id - (1<<i);
            MPI_Send(&sum,1,MPI_INT,dest,0,MPI_COMM_WORLD);
            break;
        }
    }
    if(my_id == 0)
        printf("La suma es %d(valor esperado: %d)\n",sum,num_procs*(num_procs-1)/2);
    MPI_Finalize();
}