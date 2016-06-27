#include "mpi.h"
#include <stdio.h>

int rank, numproc;

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);
	
	MPI_Comm_size(MPI_COMM_WORLD, &numproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	printf("Process %d of %d\n", rank, numproc);

	MPI_Finalize();

	return 0;
}
