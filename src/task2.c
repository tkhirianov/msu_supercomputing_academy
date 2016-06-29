#include "mpi.h"
#include <stdio.h>

#define INPUT_FILENAME "d1_task1.dat"
#define VECTOR_SIZE 1000
#define TAG 1

#define MAX_DATA_SIZE 1000000

int rank, num_proc;
double trash[MAX_DATA_SIZE];

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status status;
    int data_size;

    if (rank == 0)
    {
        for (data_size = 1; data_size < MAX_DATA_SIZE; data_size *= 2)
        {
            double time0 = MPI_Wtime();
            //PING
            MPI_Send(trash, data_size, MPI_DOUBLE, 1, TAG, MPI_COMM_WORLD);
            //PONG
            MPI_Recv(trash, data_size, MPI_DOUBLE, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status);

            double time1 = MPI_Wtime();
            printf("data size =%d \ttime = %f\t speed = %f B/s\n", data_size, time1-time0, data_size*sizeof(double)/(time1-time0));
        }
    }
    else if (rank == 1)
    {
        for (data_size = 1; data_size < MAX_DATA_SIZE; data_size *= 2)
        {
            //PONG
            MPI_Recv(trash, data_size, MPI_DOUBLE, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status);
            //PING
            MPI_Send(trash, data_size, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
        }
    }

    printf("Process %d of %d finished.\n", rank, num_proc);
    MPI_Finalize();

    return 0;
}
