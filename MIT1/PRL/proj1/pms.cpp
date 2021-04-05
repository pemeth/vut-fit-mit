#include <iostream>
#include <vector>
#include <mpi.h>

#define VALUES_CNT 16
#define PASS_TAG 1

void receive(std::vector<unsigned char> *top, std::vector<unsigned char> *bot, int rank)
{
    static int recv_top = 0;
    unsigned char val;
    MPI_Status status;
    MPI_Recv(&val, 1, MPI_CHAR, 0, PASS_TAG, MPI_COMM_WORLD, &status);

    // Rank 0 has separate behaviour, so it is not handled in this if/else construct
    if (rank == 1) {
        if (recv_top < 1) {
            top->push_back(val);
            recv_top++;
        } else {
            bot->push_back(val);
            recv_top--;
        }
    } else if (rank == 2) {
        if (recv_top < 2) {
            top->push_back(val);
        } else {
            bot->push_back(val);
        }

        recv_top++;

        if (recv_top == 4) {
            recv_top = 0; // reset
        }
    } else if (rank == 3) {
        if (recv_top < 4) {
            top->push_back(val);
        } else {
            bot->push_back(val);
        }

        recv_top++;

        if (recv_top == 8) {
            recv_top = 0; // reset
        }
    } else if (rank == 4) {
        // last processor contains the sorted sequence in `top`
        top->push_back(val);
    }
}

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

    std::vector<unsigned char> top, bot;
    int finished = 0, recvr;
    int cnt = 0; // To be incremented after every MPI_Send()
    MPI_Status stat;
    while (!finished) {
        if (rank == 0) {
            MPI_Send(&values[cnt], 1, MPI_CHAR, 1, PASS_TAG, MPI_COMM_WORLD);
            cnt++;

        } else if (rank == 1) {
            receive(&top, &bot, rank);

        } else if (rank == 2) {
            receive(&top, &bot, rank);

        } else if (rank == 3) {
            receive(&top, &bot, rank);

        } else if (rank == 4) {
            receive(&top, &bot, rank);
            if (top.size() == VALUES_CNT) {
                finished++; // all are sorted
            }
        } else {/* Do nothing - program is built for 5 processors */}

        if (cnt == VALUES_CNT) {
            finished++; // nothing more to be sent
        }
    }

    if (rank == 4) {
        for (unsigned long int i = 0; i < top.size(); i++) {
            printf("%u|", top[i]);
        }
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}