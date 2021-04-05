#include <iostream>
#include <mpi.h>

#define VALUES_CNT 16

int main(int argc, char *argv[])
{
    int rank;
    unsigned char values[VALUES_CNT];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int c, i=0;
    if (rank == 0) {
        // Load 16 unsorted values from stdin
        while ((c = getc(stdin)) != EOF && i < VALUES_CNT) {
            values[i] = (unsigned char) c;
            printf("%u ", values[i]); // cout wouldn't cooperate
            i++;
        }
        std::cout << '\n';
    }

    MPI_Finalize();
    return 0;
}