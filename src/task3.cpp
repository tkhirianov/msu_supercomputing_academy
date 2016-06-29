#include "mpi.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

const char input_filename[] = "task3.in";
const char output_filename[] = "task3.out";
const int tag = 1;

int vector_size = 1000000;
int rank, num_proc;

void linear_sum(const vector<double> &A, double &sum, double &time_elapsed);
void reduce_sum(const vector<double> &A, double &sum, double &time_elapsed);

double part_vector_sum(const vector<double> &A, int start, int stop);
vector<double> generate_vector(int vector_size);
int block_distribution(int rank);
int block_distribution_test();


int task_3()
{
    vector<double> A = generate_vector(vector_size);
    double sum1, dt_1, sum2, dt_2;

    linear_sum(A, sum1, dt_1);
    reduce_sum(A, sum2, dt_2);

    if (rank == 0) {
        ofstream fout(output_filename);
        fout << setprecision(16);
        fout << "sum1 = " << sum1 << " sum2 = " << sum2
            << " dt_1 = " << dt_1 << " dt_2 = " << dt_2 << endl;
    }
    return 0;
}

int main(int argc, char **argv)
{
#ifdef DEBUG
    block_distribution_test();
#else
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    cout << "Process rank=" << rank << " of " << num_proc << " started.\n";
    task_3();
    cout << "Process rank=" << rank << " of " << num_proc << " finished.\n";

    MPI_Finalize();
#endif
    return 0;
}

vector<double> generate_vector(int vector_size)
{
    vector<double> A(vector_size);
    for(int i = 0; i < vector_size; i++)
        A[i] = i+1;
    return A;
}

void linear_sum(const vector<double> &A, double &sum, double &time_elapsed)
{
    double time0 = MPI_Wtime();
    int start = block_distribution(rank);
    int stop = block_distribution(rank+1);

    if (rank == 0) {
        double full_sum = part_vector_sum(A, start, stop);
        double part_sum;
        for (int proc = 1; proc < num_proc; proc += 1) {
            MPI_Status status;
            MPI_Recv(&part_sum, 1, MPI_DOUBLE, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
            full_sum += part_sum;
        }
        sum = full_sum;
    } else {
        double part_sum = part_vector_sum(A, start, stop);
        MPI_Status status;
        MPI_Send(&part_sum, 1, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
        sum = -1;  // for non-zero process
    }

    time_elapsed = MPI_Wtime() - time0;
}

void reduce_sum(const vector<double> &A, double &sum, double &time_elapsed)
{
    double time0 = MPI_Wtime();

    int start = block_distribution(rank);
    int stop = block_distribution(rank+1);
    double part_sum = part_vector_sum(A, start, stop);

    double full_sum = -1;
    MPI_Reduce(&part_sum, &full_sum, 1, MPI_DOUBLE, MPI_SUM, 0 /*root*/, MPI_COMM_WORLD)
    sum = full_sum;

    time_elapsed = MPI_Wtime() - time0;
}

double part_vector_sum(const vector<double> &A, int start, int stop)
{
    double part_sum = 0.;
    for (int i = start; i < stop; i += 1)
    {
        part_sum += A[i];
    }
    return part_sum;
}

int block_distribution(int rank)
{
    int block_size = vector_size/num_proc;
    int remain_elements = vector_size % num_proc;

    if (remain_elements == 0) // получается равномерное распределение вектора по процессам
        return rank*block_size;
    else if (rank < remain_elements)
        return rank*(block_size+1);
    else
        return remain_elements*(block_size+1) + (rank-remain_elements)*block_size;
}

int block_distribution_test()
{
    vector_size = 12;
    num_proc = 3;

    cout << "vector_size= " << vector_size << " num_proc= " << num_proc << ": ";
    for (int i = 0; i <= num_proc; i += 1)
        cout << i << ":" << block_distribution(i);
    cout << endl;

    vector_size = 17;
    num_proc = 3;

    cout << "vector_size= " << vector_size << " num_proc= " << num_proc << ": ";
    for (int i = 0; i <= num_proc; i += 1)
        cout << i << ":" << block_distribution(i);
    cout << endl;

    vector_size = 12;
    num_proc = 5;

    cout << "vector_size= " << vector_size << " num_proc= " << num_proc << ": ";
    for (int i = 0; i <= num_proc; i += 1)
        cout << i << ":" << block_distribution(i);
    cout << endl;

    return 0;
}
