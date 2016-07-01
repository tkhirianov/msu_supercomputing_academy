#include "mpi.h"
#include <stdio.h>

#define INPUT_FILENAME "d1_task1.dat"
#define VECTOR_SIZE 1000
#define TAG 1

int global_rank, num_proc;

void read_vectors(char *filename, double *A, double *B);
double part_scalar_production(double *A, double *B);
double summarize_scalar_production();

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &global_rank);

    double A[VECTOR_SIZE], B[VECTOR_SIZE];
    read_vectors(INPUT_FILENAME, A, B);

    if (global_rank == 0)
    {
    	double time0 = MPI_Wtime();
        double sum = part_scalar_production(A, B) + summarize_scalar_production();
	    double time1 = MPI_Wtime();
        printf("scalar production = %f\n", sum);
	    printf("time = %f\n", time1 - time0);
    }
    else
    {
        double part_sum = part_scalar_production(A, B);
        MPI_Send(&part_sum, 1, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
    }

    printf("Process %d of %d finished.\n", global_rank, num_proc);
    MPI_Finalize();

    return 0;
}

void read_vectors(char *filename, double *A, double *B)
{
    FILE *fin = fopen(INPUT_FILENAME, "r");
    int i;
    for(i = 0; i < VECTOR_SIZE; i++)
        fscanf(fin, "%lf", A+i);
    for(i = 0; i < VECTOR_SIZE; i++)
        fscanf(fin, "%lf", B+i);
    fclose(fin);
}

double part_scalar_production(double *A, double *B)
{
    double part_sum = 0.;
    int i;
    for (i = global_rank; i < VECTOR_SIZE; i += num_proc)
    {
        part_sum += A[i]*B[i];
    }
    return part_sum;
}

double summarize_scalar_production()
{
    double sum = 0., partsum;
    MPI_Status status;
    int i;
    for(i = 1; i < num_proc; i++)
    {
        MPI_Recv(&partsum, 1, MPI_DOUBLE, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status);
        sum += partsum;
    }

    return sum;
}

