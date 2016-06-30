//
// Created by Хирьянов on 29.06.2016.
// License: GPLv3
//

#include "mpi.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

const char output_filename[] = "task4.out";
const int tag = 1;
int rank, num_proc;

struct Info{
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int rank;
};

ostream& operator << (ostream& out, Info info);
MPI_Datatype create_struct_info_mpi_type();


int task_4()
{
    Info my_info;
    int length;
    MPI_Datatype struct_info_mpi_type;

    // заполняем информацию о себе
    MPI_Get_processor_name(my_info.processor_name, &length);
    my_info.rank = rank;

    // создаём новый MPI тип
    struct_info_mpi_type = create_struct_info_mpi_type();

    if (rank != 0) {
        MPI_Send(&my_info, 1, struct_info_mpi_type, 0, tag, MPI_COMM_WORLD);
    } else {
        ofstream fout(output_filename);
        fout << "My own info:" << my_info << endl;
        Info process_info;
        for (int process = 1; process < num_proc; process++) {
            MPI_Status status;
            MPI_Recv(&process_info, 1, struct_info_mpi_type, process, tag, MPI_COMM_WORLD, &status);
            fout << process << " process info:" << process_info << endl;
        }
    }

    // освобождаем свой MPI тип
    MPI_Type_free(&struct_info_mpi_type);
    return 0;
}

int main(int argc, char **argv)
{
#ifdef DEBUG
    // NO TESTING -- do nothing
#else
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    cout << "Process rank=" << rank << " of " << num_proc << " started.\n";
    task_4();
    cout << "Process rank=" << rank << " of " << num_proc << " finished.\n";

    MPI_Finalize();
#endif
    return 0;
}

ostream& operator << (ostream& out, Info info)
{
    out << "processor_name=\'" << info.processor_name << "\', rank=" << info.rank << ".";
    return out;
}


MPI_Datatype create_struct_info_mpi_type()
{
    Info x;
    MPI_Datatype struct_info_mpi_type;
    MPI_Datatype types[2] = {MPI_CHAR, MPI_INT};
    int block_lens[2] = {MPI_MAX_PROCESSOR_NAME, 1};
    MPI_Aint displacements[2] = {(char*)&x.processor_name - (char*)&x, (char*)&x.rank - (char*)&x};

    MPI_Type_create_struct(2, block_lens, displacements, types, &struct_info_mpi_type);
    MPI_Type_commit(&struct_info_mpi_type);

    return struct_info_mpi_type;
}
