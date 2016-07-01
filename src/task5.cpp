//
// Created by Хирьянов on 30.06.2016.
// License: GPLv3
//

#include "mpi.h"
#include <iostream>
#include <vector>

using namespace std;

const int tag = 1;
int global_rank, num_proc;

// разбивает общий коммуниктор на две группы
void create_two_groups(MPI_Group *group1, MPI_Group *group2);

int task_5()
{
    /* Реализуйте разбиение процессов приложения на две группы,
     * в одной из которых осуществляется обмен по кольцевой топологии
     * при помощи сдвига в одномерной декартовой топологии,
     * а в другой – коммуникации по схеме «мастер – рабочие»,
     * реализованной при помощи топологии графа.*/

    // разбиение на две группы
    MPI_Group group1, group2;
    create_two_groups(&group1, &group2);

    // создание коммуникаторов
    MPI_Comm comm1, comm2;
    MPI_Comm_create(MPI_COMM_WORLD, group1, &comm1);
    MPI_Comm_create(MPI_COMM_WORLD, group2, &comm2);

    if (global_rank < num_proc/2) {
        // работа процессов первой группы в рамках коммуникатора comm1
        int comm1_rank, comm1_size;
        MPI_Comm_size(comm1, &comm1_size);
        MPI_Comm_rank(comm1, &comm1_rank);
        // создание одномерной декартовой топологии
        MPI_Comm topology;
        int dimension = comm1_size;
        int periodical = 1;
        MPI_Cart_create(comm1, 1, &dimension, &periodical, 0, &topology);

        // обмен данными по кольцу одномерной топологии:
        int source, dest;
        MPI_Cart_shift(topology, 0, +1, &source, &dest);
        int buffer = comm1_rank; // передаём свой собственный ранг
        MPI_Status status;
        MPI_Sendrecv_replace(&x, 1, MPI_INT, dest, tag, source, tag, topology, &status);

        cout << "Dekart topology process " << comm1_rank << " received: " << buffer << endl;
    } else {
        // работа процессов второй группы в рамках коммуникатора comm2
        int comm2_rank, comm2_size;
        MPI_Comm_size(comm2, &comm2_size);
        MPI_Comm_rank(comm2, &comm2_rank);

        /*if (rank != 0) {
            MPI_Send(&my_info, 1, struct_info_mpi_type, 0, tag, MPI_COMM_WORLD);
        } else {
            for (int process = 1; process < num_proc; process++) {
                MPI_Status status;
                MPI_Recv(&process_info, 1, struct_info_mpi_type, process, tag, MPI_COMM_WORLD, &status);
                cout << rank << " process info:" << process_info << endl;
            }
        }*/
        cout << "Graph topology process " << comm1_rank << " do nothing. " << endl;
    }

    // освобождаем коммуникаторы
    MPI_Comm_free(&comm1);
    MPI_Comm_free(&comm2);

    // освобождаем созданные группы
    MPI_Group_free(&group1);
    MPI_Group_free(&group2);

    return 0;
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
    task_5();
    cout << "Process rank=" << global_rank << " of " << num_proc << " finished.\n";

    MPI_Finalize();
#endif
    return 0;
}



void create_two_groups(MPI_Group *group1, MPI_Group *group2)
{
    MPI_Group world_group;
    // Получение группы world_group, соответствующей коммуникатору MPI_COMM_WORLD.
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);
    size1 = size/2;
    int ranks[size1];
    for(i=0; i < size1; i++)
        ranks[i] = i;
    MPI_Group_incl(world_group, size1, ranks, group1);
    MPI_Group_excl(world_group, size1, ranks, group2);
}
