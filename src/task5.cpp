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

        // узнаём целевой узел по кольцу одномерной топологии:
        int source, dest;
        MPI_Cart_shift(topology, 0, +1, &source, &dest);

        int buffer = comm1_rank; // передаём свой собственный ранг
        MPI_Status status;
        MPI_Sendrecv_replace(&buffer, 1, MPI_INT, dest, tag, source, tag, topology, &status);

        cout << "Dekart topology process " << comm1_rank << " received: " << buffer << endl;

        // освобождаем коммуникатор
        MPI_Comm_free(&comm1);
    } else {
        // работа процессов второй группы в рамках коммуникатора comm2
        int comm2_rank, comm2_size;
        MPI_Comm_size(comm2, &comm2_size);
        MPI_Comm_rank(comm2, &comm2_rank);

        // создание топологии графа
        MPI_Comm graph_topology;
        int index[comm2_size];
        int edges[comm2_size*2-1];

        // построение структуры графа -- интегрального списка смежности
        index[0] = comm2_size - 1;
        for (int i = 0; i < comm2_size; i++)
            index[i] = 1;
        for (int i = 0; i < comm2_size - 1; i++)
            edges[i] = i + 1;
        for (int i = comm2_size; i < comm2_size*2 - 1; i++)
            edges[i] = 0;

        MPI_Graph_create(comm2, comm2_size, index, edges, 0, &graph_topology);
        int graph_size, graph_rank;
        MPI_Comm_size(graph_topology, &graph_size);
        MPI_Comm_rank(graph_topology, &graph_rank);

        // запрашиваем у графа количество своих соседей и их ранги в графе
        int neighbors_number;
        MPI_Graph_neighbors_count(graph_topology, graph_rank, &neighbors_number);
        int neighbours[graph_size];
        MPI_Graph_neighbors(graph_topology, graph_rank, graph_size, neighbours);

        // обмениваемся со всеми своими соседями
        for (int i = 0; i < neighbors_number; i++) {
            int destination = neighbours[i];

            int buffer = graph_rank; // передаём свой собственный ранг в графе
            MPI_Status status;
            MPI_Sendrecv_replace(&buffer, 1, MPI_INT, destination, tag, destination, tag, graph_topology, &status);
            cout << "Graph topology process " << graph_rank << " received " << buffer << "from " << destination << " process. " << endl;
        }

        // освобождаем коммуникатор
        MPI_Comm_free(&comm2);
    }

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
    int ranks[num_proc/2];
    for(int i=0; i < num_proc/2; i++)
        ranks[i] = i;
    MPI_Group_incl(world_group, num_proc/2, ranks, group1);
    MPI_Group_excl(world_group, num_proc/2, ranks, group2);
}
