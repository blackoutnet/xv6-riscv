#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/defs.h"
#include "user/user.h"

#define SIEVE_LIMIT 35

void sieve(int pipe_in);

int main(int argc, char** argv)
{
    int sieve_pipe[2];

    if (pipe(sieve_pipe) < 0) {
        fprintf(STDERR, "error: pipe failed\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 2; i < SIEVE_LIMIT; i += 1) {
        write(sieve_pipe[1], &i, sizeof(int));
    }
    close(sieve_pipe[1]);

    sieve(sieve_pipe[0]);

    exit(EXIT_SUCCESS);
}

void sieve(int pipe_in)
{
    int child = fork();

    if (child == 0) {
        int next_pipe[2];
        if (pipe(next_pipe) < 0) {
            fprintf(STDERR, "error: pipe failed\n");
            exit(EXIT_FAILURE);
        }

        int prime;
        read(pipe_in, &prime, sizeof(int));
        fprintf(STDOUT, "prime %d\n", prime);

        int next = -1;
        while (read(pipe_in, &next, sizeof(int)) > 0) {
            if (next % prime != 0) {
                write(next_pipe[1], &next, sizeof(int));
            }
        }
        close(next_pipe[1]);
        close(pipe_in);

        if (next != -1) {
            sieve(next_pipe[0]);
        } else {
            close(next_pipe[0]);
        }
    } else if (child > 0) {
        close(pipe_in);
        wait(0);
    } else {
        fprintf(STDERR, "error: fork failed\n");
        exit(EXIT_FAILURE);
    }
}
