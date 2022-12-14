/**
 * Second PRL project: matrix mesh multiplication using OpenMPI.
 * @author Patrik Nemeth (xnemet04)
 */
#include <iostream>
#include <fstream>
#include <mpi.h>
#include <string.h> // malloc
#include <limits> // numeric_limits

#define FILE_MAT1 "mat1"
#define FILE_MAT2 "mat2"
#define ROOT 0
#define TAG 1
#define TAG_FIN_A 2
#define TAG_FIN_B 3
#define FIN 11
#define OK 0

/**
 * Checks if a process to the right of `rank` exists and if it
 * does, then send value `val` to it. The process to the right
 * is always `rank`+1, so it is up to the caller to check if
 * the calling process is or isn't at the rightmost edge of
 * the process mesh.
 * @param val the value to be sent.
 * @param rank the rank of the calling process.
 * @param max_rank the id of the last process in MPI_COMM_WORLD.
 */
void send_right(int val, int rank, int max_rank, int tag = TAG) {
    if (rank + 1 <= max_rank) {
        MPI_Send(&val, 1, MPI_INT, rank+1, tag, MPI_COMM_WORLD);
    }
}

/**
 * Checks if a process to the bottom of `rank` exists and if it does,
 * then send value `val` to it. The process to the bottom of
 * the current one is always `rank`+`stride`, so it is up to
 * the caller to check if the calling process is or isn't at
 * the bottommost edge of the process mesh.
 * @param val the value to be sent.
 * @param stride the number of processes to skip to land on the one
 *  below `rank` (generally the width of the process mesh).
 * @param rank the rank of the calling process.
 * @param max_rank the id of the last process in MPI_COMM_WORLD.
 */
void send_down(int val, int stride, int rank, int max_rank, int tag = TAG) {
    if (rank + stride <= max_rank) {
        MPI_Send(&val, 1, MPI_INT, rank+stride, tag, MPI_COMM_WORLD);
    }
}

/**
 * * This is a shared documentation comment with the functions
 * * propagate_first_col() and propagate_rest().
 * This function is meant to be used by the first row of processes in the mesh.
 * The function issues a send_right() and send_down() with respective
 * values `val_right` and `val_down` and MPI_Send tags `tag_right` and `tag_down`.
 * @param val_right the value to be sent to the right.
 * @param val_down the value to be sent downward.
 * @param cols the number of columns in the process mesh
 * @param rank the rank of the calling process.
 * @param max_rank the highest rank in MPI_COMM_WORLD.
 * @param tag_right (default = TAG) the tag for MPI_Send() when sending to the right.
 * @param tag_down (default = TAG) the tag for MPI_Send() when sending downward.
 */
void propagate_first_row(
    int val_right,
    int val_down,
    int cols,
    int rank,
    int max_rank,
    int tag_right = TAG,
    int tag_down = TAG)
{
    if (!(rank == cols - 1)) {
        // Not last in the first row - can send right.
        send_right(val_right, rank, max_rank, tag_right);
    }
    // send_down will not allow sending one row lower if this is the
    //  last row because of how the mesh is set up.
    send_down(val_down, cols, rank, max_rank, tag_down);
}

/**
 * * See documentation comment for function propagate_first_row().
 * This function is meant to be used by the first column of processes in the mesh.
 */
void propagate_first_col(
    int val_right,
    int val_down,
    int cols,
    int rank,
    int max_rank,
    int tag_right = TAG,
    int tag_down = TAG)
{
    if (cols > 1) {
        // There is someone to the right, send it.
        send_right(val_right, rank, max_rank, tag_right);
    }
    // send_down() will not allow sending to the bottom neighbour
    //  because of how the mesh is set up.
    send_down(val_down, cols, rank, max_rank, tag_down);
}

/**
 * * See documentation comment for function propagate_first_row().
 * This function is meant to be used by processes NOT in the first row or the first
 * column of the process mesh.
 */
void propagate_rest(
    int val_right,
    int val_down,
    int cols,
    int rank,
    int max_rank,
    int tag_right = TAG,
    int tag_down = TAG)
{
    if (!((rank + 1) % cols == 0)) {
        // Someone to the right, send value.
        send_right(val_right, rank, max_rank, tag_right);
    }
    send_down(val_down, cols, rank, max_rank, tag_down);
}

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

    if (rank != ROOT &&
        (rank < cols || rank % cols == 0)) {
        // Open the files on the rest of the processes that need them
        //  (i.e. the first row and column) and seek to correct position.
        file1.open(FILE_MAT1, std::ios_base::in);
        file2.open(FILE_MAT2, std::ios_base::in);

        file1.seekg(pos_f1);
        file2.seekg(pos_f2);
    }

    int a, b, c = 0;
    int cnt = 0;
    bool moved_to_init_pos = false;
    while (true) {
        // The main loop...

        if (rank == ROOT) {
            // The root of the process mesh - the only one, that
            //  directly receives data from both files.
            file1 >> a;
            file2 >> b;

            // Calculate next c value.
            c += (a * b);

            // Move to the next row in file2.
            bool end = move_by_n(&file2, cols - 1);

            // Send data to neighbours.
            if (!(rank == cols - 1)) {
                // Not last in the first row - can send right.
                send_right(a, rank, size-1);
            }
            if (!(rank == size - 1 - (size/cols))) {
                // Not last in the first column - can send down.
                send_down(b, cols, rank, size-1);
            }

            // Finish if no more characters are to be read.
            int next_char = file1.peek();
            if (end || next_char == '\n' || next_char == EOF) {
                break;
            }
        } else if (rank < cols && rank != ROOT) {
            // The first row of processes with the exception of ROOT.
            if (!moved_to_init_pos) {
                // The initial position in the file for this process.
                move_by_n(&file2, rank);
                // Green light for other processes to start receiving.
                propagate_first_row(OK, OK, cols, rank, size-1, TAG_FIN_A, TAG_FIN_B);
                moved_to_init_pos = true;
            }

            // Get a and b.
            file2 >> b;
            MPI_Status status;
            MPI_Recv(&a, 1, MPI_INT, rank-1, TAG, MPI_COMM_WORLD, &status);

            // Propagate the a and b values.
            propagate_first_row(a, b, cols, rank, size-1);

            // Calculate next c value.
            c += (a * b);

            bool end = move_by_n(&file2, cols-1);
            if (end) {
                // Send notice, that no further values will be arriving.
                propagate_first_row(FIN, FIN, cols, rank, size-1, TAG_FIN_A, TAG_FIN_B);
                break;
            } else {
                // Send notice, that there will be more values arriving.
                propagate_first_row(OK, OK, cols, rank, size-1, TAG_FIN_A, TAG_FIN_B);
            }
        } else if (rank % cols == 0 && rank != ROOT) {
            // The first column of processes with the exception of ROOT.
            if (!moved_to_init_pos) {
                file1 >> a;
                for (int i = 0; i < rank/cols; i++) {
                    // Skip rank/cols number of lines.
                    file1.ignore(std::numeric_limits<std::streamsize>::max(), (int) '\n');
                }
                // Green light for other processes to start receiving.
                propagate_first_col(OK, OK, cols, rank, size-1, TAG_FIN_A, TAG_FIN_B);
                moved_to_init_pos = true;
            }

            // Get a and b.
            file1 >> a;
            MPI_Status status;
            MPI_Recv(&b, 1, MPI_INT, rank-cols, TAG, MPI_COMM_WORLD, &status);

            // Propagate the a and b values.
            propagate_first_col(a, b, cols, rank, size-1);

            c += (a * b);

            int next_char = file1.peek();
            if (next_char == '\n' || next_char == EOF) {
                // Send notice, that no further values will be arriving.
                propagate_first_col(FIN, FIN, cols, rank, size-1, TAG_FIN_A, TAG_FIN_B);
                break;
            } else {
                // Send notice, that there will be more values arriving.
                propagate_first_col(OK, OK, cols, rank, size-1, TAG_FIN_A, TAG_FIN_B);
            }
        } else {
            // The rest of the processes.

            // Receive notices whether or not more values will be arriving.
            MPI_Status status;
            int end_a = OK, end_b = OK;
            MPI_Recv(&end_a, 1, MPI_INT, rank-1, TAG_FIN_A, MPI_COMM_WORLD, &status);
            MPI_Recv(&end_b, 1, MPI_INT, rank-cols, TAG_FIN_B, MPI_COMM_WORLD, &status);

            // Propagate whether or not more values will arrive.
            propagate_rest(end_a, end_b, cols, rank, size-1, TAG_FIN_A, TAG_FIN_B);

            if (end_a != OK && end_b != OK) {
                // No more values will arrive, exit.
                break;
            }
            // More values will arrive, receive them and propagate them further.
            MPI_Recv(&a, 1, MPI_INT, rank-1, TAG, MPI_COMM_WORLD, &status);
            MPI_Recv(&b, 1, MPI_INT, rank-cols, TAG, MPI_COMM_WORLD, &status);

            propagate_rest(a, b, cols, rank, size-1);

            c += (a * b);
        }
    }

    int *results;
    if (rank == ROOT) {
        // Allocate memory for response from every process.
        results = (int *) malloc(sizeof(int) * size);
    }
    MPI_Gather(&c, 1, MPI_INT, results, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    if (rank == ROOT) {
        std::cout << rows << ':' << cols;
        for (int i = 0; i < size; i++) {
            if (i % cols == 0) {
                std::cout << '\n';
            }
            if (i % cols != 0) {
                std::cout << ' ';
            }
            std::cout << results[i];
        }
        std::cout << '\n';

        free(results);
    }

    // Close the files.
    file1.close();
    file2.close();

    MPI_Finalize();
    return 0;
}
