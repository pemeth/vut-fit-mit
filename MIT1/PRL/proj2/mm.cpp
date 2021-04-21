/**
 * Second PRL project: matrix mesh multiplication using OpenMPI.
 * @author Patrik Nemeth (xnemet04)
 */
#include <iostream>
#include <fstream>
#include <mpi.h>

#define FILE_MAT1 "mat1"
#define FILE_MAT2 "mat2"
#define ROOT 0

/**
 * Discards `n` number of integer values from the input stream.
 * Returns true if successful, false on EOF.
 * @param file a pointer to the file input stream.
 * @param n is the number of values to be discarded.
 * @returns true on success, false on EOF.
 */
bool move_by_n(std::ifstream *file, int n) {
    for (int i = 0; i < n; i++) {
        int tmp;
        (*file) >> tmp;

        if (file->eof()) {
            return true;
        }
    }

    return false;
}

int main(int argc, char *argv[])
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rows, cols;
    int pos_f1 = 0, pos_f2 = 0; // File positions.
    std::ifstream file1, file2;
    if (rank == 0) {
        // TODO Maybe check if the input was parsed correctly as
        //      an int by checking the failbit with file1.fail().

        // Get the number of rows and columns from the files
        //  and save the seek position within those files.
        file1.open(FILE_MAT1, std::ios_base::in);
        file1 >> rows;
        pos_f1 = file1.tellg();

        file2.open(FILE_MAT2, std::ios_base::in);
        file2 >> cols;
        pos_f2 = file2.tellg();
    }

    // Broadcast the cols and rows and the positions, from where to
    //  start reading the matrices themselves (i.e. without the
    //  first line specifying the number of rows/columns).
    int sndr[] = {rows, cols, pos_f1, pos_f2};
    MPI_Bcast(&sndr, 4, MPI_INT, ROOT, MPI_COMM_WORLD);
    rows = sndr[0];
    cols = sndr[1];
    pos_f1 = sndr[2];
    pos_f2 = sndr[3];

    if (rank != ROOT) {
        // Open the files on the rest of the processes and seek
        //  to correct position.
        file1.open(FILE_MAT1, std::ios_base::in);
        file2.open(FILE_MAT2, std::ios_base::in);

        file1.seekg(pos_f1);
        file2.seekg(pos_f2);
    }

    int a, b, c = 0;
    int cnt = 0;
    while (true) {
        // The main loop...

        if (rank == ROOT) {
            // The root of the process mesh - the only one, that
            //  directly receives data from both files.
            file1 >> a;
            file2 >> b;

            c += (a * b);
            std::cout << "val1: " << a << "|val2: " << b << "\n";

            bool end = move_by_n(&file2, cols - 1);

            if (end) {
                break;
            }
        } else {
            break;
        }
    }

    // Close the files.
    file1.close();
    file2.close();

    MPI_Finalize();
    return 0;
}
