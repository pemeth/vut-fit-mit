#include <deque>
#include <iostream>
#include <mpi.h>

#define VALUES_CNT 16
#define TAG 1

// TODO remove debug prints (stderr)

// Knuth's x^p
int pow(int x, unsigned int p)
{
    if (p == 0) return 1;
    if (p == 1) return x;
    int tmp = pow(x, p/2);
    if (p%2 == 0) {
        return tmp * tmp;
    } else {
        return x * tmp * tmp;
    }
}

/**
 * Receiver for the individual processes. Manages the correct assignemnt of
 * incoming values to the correct queues (`top` or `bot`). Receives from
 * the previous process (rank-1).
 *
 * @param top pointer to the top inbound queue of this process.
 * @param bot pointer to the bottom inbound queue of this process.
 * @param rank the rank of the current process - MUST NOT be less than 1.
 */
void receive(std::deque<unsigned char> *top, std::deque<unsigned char> *bot, int rank)
{
    static int q_picker = 0;
    static int received = 0; // Incremented after every received value.
    if (received >= VALUES_CNT) {
        // Prevents the process from stalling on MPI_Recv when there are
        //  no more values to be received.
        return;
    }

    unsigned char val;
    MPI_Status status;
    MPI_Recv(&val, 1, MPI_UNSIGNED_CHAR, rank-1, TAG, MPI_COMM_WORLD, &status);
    received++;

    // Decide to which queue the new value should go.
    if (q_picker < pow(2, rank-1)) {
        top->push_back(val);
        fprintf(stderr, "top ");
    } else {
        bot->push_back(val);
        fprintf(stderr, "bot ");
    }
    q_picker++;
    fprintf(stderr, "%d received %u ", rank, val);
    if (q_picker >= 2 * pow(2, rank-1)) {
        q_picker = 0; // reset
        fprintf(stderr, "| reset");
    }
    fprintf(stderr, "\n");
}

/**
 * Merge funcction for the last process. Instead of sending the sorted value
 * to the next process, it is pushed to the `out` queue.
 *
 * @param top pointer to the top inbound queue of this process.
 * @param bot pointer to the bottom inbound queue of this process.
 * @param out pointer to the outbound queue of this process - contains the sorted values.
 * @param rank the rank of the current process - MUST be last in MPI_COMM_WORLD.
 * @param cnt pointer to a counter variable, which is incremented after each value sorted.
 */
void merge(
    std::deque<unsigned char> *top,
    std::deque<unsigned char> *bot,
    std::deque<unsigned char> *out,
    int rank,
    int *cnt)
{
    static unsigned int from_top = 0, from_bot = 0;
    static bool let_me_through = false;

    if ((pow(2, top->size()-1) >= pow(2, rank-1) && bot->size() >= 1) || let_me_through) {
        let_me_through = true;
        unsigned char tmp = 0;

        if (from_top + from_bot >= (2 * pow(2, rank-1)) - 1) {
            // Last value - take it from whichever queue is non-empty.
            if (from_top < from_bot) {
                if (top->empty())
                    return; //active waiting
                tmp = top->front();
                out->push_back(tmp);
                fprintf(stderr, "SORTED: %u\n", tmp);
                if (!top->empty())
                    top->pop_front();
            } else {
                if (bot->empty())
                    return; //active waiting
                tmp = bot->front();
                out->push_back(tmp);
                fprintf(stderr, "SORTED: %u\n", tmp);
                if (!bot->empty())
                    bot->pop_front();
            }
            (*cnt)++;
            return;
        }

        if (from_top == pow(2, rank-1) || from_bot == pow(2, rank-1)) {
            if (from_top == pow(2, rank-1)) {
                // Bot remains
                if (bot->empty())
                    return; //active waiting
                tmp = bot->front();
                out->push_back(tmp);
                fprintf(stderr, "SORTED: %u\n", tmp);
                from_bot++;
                if (!bot->empty())
                    bot->pop_front();
            } else if (from_bot == pow(2, rank-1)) {
                // Top remains
                if (top->empty())
                    return; //active waiting
                tmp = top->front();
                out->push_back(tmp);
                fprintf(stderr, "SORTED: %u\n", tmp);
                from_top++;
                if (!top->empty())
                    top->pop_front();
            }
            (*cnt)++;
            return;
        }

        if (top->empty() || bot->empty()) {
            // A form of active waiting - nothing to compare right now.
            return;
        }

        if (top->front() < bot->front()) {
            tmp = top->front();
            out->push_back(tmp);
            fprintf(stderr, "SORTED: %u\n", tmp);
            from_top++;
            if (!top->empty());
                top->pop_front();
        } else {
            tmp = bot->front();
            out->push_back(tmp);
            fprintf(stderr, "SORTED: %u\n", tmp);
            from_bot++;
            if (!bot->empty())
                bot->pop_front();
        }
        (*cnt)++;
    }

}

/**
 * Merge function for processes 1...n-1, where n is the last one. The last process
 * has its own overloaded variant. This function manages the merge-sorting carried out
 * by the individual processes and sends the sorted value to the next process (rank+1).
 *
 * @param top pointer to the top inbound queue of this process.
 * @param bot pointer to the bottom inbound queue of this process.
 * @param rank the rank of the current process - MUST NOT be the last in MPI_COMM_WORLD.
 * @param cnt pointer to a counter variable, which is incremented after every value merged.
 */
void merge(std::deque<unsigned char> *top, std::deque<unsigned char> *bot, int rank, int *cnt)
{
    // Keep track of how many values taken from each queue.
    static unsigned int from_top = 0, from_bot = 0;
    // A flag set to true after first opportunity for value comparison.
    static bool let_me_through = false;

    // TODO Figure out a way to pull out the entire function from this if block.
    //      Same in the overloaded merge().
    if ((pow(2, top->size()-1) >= pow(2, rank-1) && bot->size() >= 1) || let_me_through) {
        let_me_through = true;
        unsigned char tmp = 0;

        if (from_top + from_bot >= (2 * pow(2, rank-1)) - 1) {
            // Last value - take it from whichever queue is non-empty.
            if (from_top < from_bot) {
                if (top->empty())
                    return; //active waiting
                tmp = top->front();
                MPI_Send(&tmp, 1, MPI_UNSIGNED_CHAR, rank+1, TAG, MPI_COMM_WORLD);
                if (!top->empty())
                    top->pop_front();
            } else {
                if (bot->empty())
                    return; //active waiting
                tmp = bot->front();
                MPI_Send(&tmp, 1, MPI_UNSIGNED_CHAR, rank+1, TAG, MPI_COMM_WORLD);
                if (!bot->empty())
                    bot->pop_front();
            }
            (*cnt)++;
            from_top = from_bot = 0;
            //if (rank == 2) fprintf(stderr, "[A]merge %d frm_top: %d, frm_bot: %d, sent: %u\n", rank, from_top, from_bot, tmp);
            return;
        }

        if (from_top == pow(2, rank-1) || from_bot == pow(2, rank-1)) {
            // Maximum number of values has been taken from `top` or `bot` queue,
            //  so take from the other one.
            // TODO Pop this below into a function, as this is 2x repeated
            //      code (in overloaded merge).
            if (from_top == pow(2, rank-1)) {
                // Bot remains
                if (bot->empty())
                    return; //active waiting
                tmp = bot->front();
                MPI_Send(&tmp, 1, MPI_UNSIGNED_CHAR, rank+1, TAG, MPI_COMM_WORLD);
                from_bot++;
                if (!bot->empty())
                    bot->pop_front();
            } else if (from_bot == pow(2, rank-1)) {
                // Top remains
                if (top->empty())
                    return; //active waiting
                tmp = top->front();
                MPI_Send(&tmp, 1, MPI_UNSIGNED_CHAR, rank+1, TAG, MPI_COMM_WORLD);
                from_top++;
                if (!top->empty())
                    top->pop_front();
            } else {
                fprintf(stderr, "ERROR\n");
            }
            (*cnt)++;
            //if (rank == 2) fprintf(stderr, "[B]merge %d frm_top: %d, frm_bot: %d, sent: %u\n", rank, from_top, from_bot, tmp);
            return;
        }

        if (top->empty() || bot->empty()) {
            // A form of active waiting - nothing to compare right now.
            return;
        }

        // Take the smaller value.
        if (top->front() < bot->front()) {
            tmp = top->front();
            MPI_Send(&tmp, 1, MPI_UNSIGNED_CHAR, rank+1, TAG, MPI_COMM_WORLD);
            from_top++;
            if (!top->empty());
                top->pop_front();
        } else {
            tmp = bot->front();
            MPI_Send(&tmp, 1, MPI_UNSIGNED_CHAR, rank+1, TAG, MPI_COMM_WORLD);
            from_bot++;
            if (!bot->empty())
                bot->pop_front();
        }
        (*cnt)++;
        //if (rank == 2) fprintf(stderr, "[C]merge %d frm_top: %d, frm_bot: %d, sent: %u\n", rank, from_top, from_bot, tmp);
    }
}

int main(int argc, char *argv[])
{
    int rank, size, last_rank;
    unsigned char values[VALUES_CNT];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    last_rank = size - 1;

    int c, i=0;
    if (rank == 0) {
        // Load 16 unsorted values from stdin.
        while ((c = getc(stdin)) != EOF && i < VALUES_CNT) {
            values[i] = (unsigned char) c;
            printf("%u ", values[i]); // cout wouldn't cooperate
            i++;
        }
        printf("\n");
    }

    std::deque<unsigned char> top, bot, out;
    int finished = 0;
    int cnt = 0; // This is to be incremented after each value processed.
    while (!finished) {
        if (rank == 0) {
            // Starting point.
            MPI_Send(&values[cnt], 1, MPI_UNSIGNED_CHAR, rank+1, TAG, MPI_COMM_WORLD);
            cnt++;
        } else if (rank > 0 && rank < last_rank) {
            // The middle processes have identical behaviour only governed by rank.
            receive(&top, &bot, rank);
            merge(&top, &bot, rank, &cnt);
        } else {
            // The last process.
            receive(&top, &bot, rank);
            merge(&top, &bot, &out, rank, &cnt);
        }

        for (int i = 0; i < 100000; i++) {} // TODO: possibly delete this delay?

        if (cnt >= VALUES_CNT) {
            // This process processed all 16 values and may exit.
            finished++;
        }
    }

    //fprintf(stderr, "%d DONE\n", rank);

    if (rank == last_rank) {
        for (int i = 0; i < out.size(); i++) {
            printf("%u\n", out[i]);
        }
    }

    MPI_Finalize();
    return 0;
}