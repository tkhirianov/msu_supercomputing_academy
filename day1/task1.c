#include "mpi.h"
#include <stdio.h>

#define INPUT_FILENAME "d1_task1.dat"
#define VECTOR_SIZE 1000
#define TAG 1

int rank, numproc;

void read_vectors(char *filename, double *A, double *B);

double part_scalar_production(double *A, double *B);

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);
	
	MPI_Comm_size(MPI_COMM_WORLD, &numproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    read_vectors(INPUT_FILENAME, A, B);

    if (rank == 0)
    {
        double sum = part_scalar_production(A, B) + summarize_scalar_production();
        printf("scalar production=%f\n", sum);
    }
    else
    {
        double part_sum = part_scalar_production(A, B);
        MPI_Send(&part_sum, 1, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
    }

	printf("Process %d of %d finished.\n", rank, numproc);
	MPI_Finalize();

	return 0;
}

void read_vectors(char *filename, double *A, double *B)
{
    FILE *fin;
    fopen(fin, INPUT_FILENAME, "r");
    int i;
    for(i = 0; i < VECTOR_SIZE; i++)
        scanf("%lf", A+i);
    for(i = 0; i < VECTOR_SIZE; i++)
        scanf("%lf", B+i);
    fclose();
}

double part_scalar_production(double *A, double *B)
{
    double part_sum = 0.;
    for (i = rank; i < N; i += size)
    {
        part_sum += A[i]*B[i];
    }
    retuern part_sum;
}

double summarize_scalar_production()
{
    double sum = 0., partsum;
    MPI_Status status;
    int i;
    for(i = 1; i < numproc; i++)
    {
        MPI_Recv(&partsum, 1, MPI_DOUBLE, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status);
        sum += partsum;
    }

    return sum;
}

