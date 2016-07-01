//
// Created by Хирьянов on 01.07.2016.
// License: GPLv3
//
#define MATRIX_SIZE 8192

void generate_matrix(int A[][]);


void final_task_pdgemm()
{
    double A[matrix_size]
    generate_matrix(matrix_size, A);

    pdgemm (char *transa , char *transb , MKL_INT *m , MKL_INT *n , MKL_INT *k , double *alpha , double *a , MKL_INT *ia , MKL_INT *ja , MKL_INT *desca , double *b , MKL_INT *ib , MKL_INT *jb , MKL_INT *descb , double *beta , double *c , MKL_INT *ic , MKL_INT *jc , MKL_INT *descc );

}


int main(int argc, char **argv)
{
#ifdef DEBUG
    // NO TESTING -- do nothing
#else
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &global_rank);

    cout << "Process rank=" << global_rank << " of " << num_proc << " started.\n";
    final_task_pdgemm();
    cout << "Process rank=" << global_rank << " of " << num_proc << " finished.\n";

    MPI_Finalize();
#endif
    return 0;
}

