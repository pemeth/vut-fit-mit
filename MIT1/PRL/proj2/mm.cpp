/**
 * Second PRL project: matrix mesh multiplication using OpenMPI.
 * @author Patrik Nemeth (xnemet04)
 */
#include <iostream>
#include <mpi.h>

#define FILE_MAT1 "mat1"
#define FILE_MAT2 "mat2"

int main(int argc, char *argv[])
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    MPI_Finalize();
    return 0;
}
