#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/defs.h"
#include "user/user.h"

#define PING 7 // random bytes
#define PONG 8

int main(int argc, char **argv)
{
    int pingfd[2];
    int pongfd[2];
    if (pipe(pingfd) < 0 || pipe(pongfd) < 0)
    {
        fprintf(STDERR, "error: pipe failed\n");
        exit(EXIT_FAILURE);
    }

    int child = fork();

    if (child == 0)
    {
        uint8 ping;
        read(pingfd[0], &ping, sizeof(uint8));

        if (ping == PING)
        {
            uint8 pong = PONG;
            fprintf(STDOUT, "%d: received ping\n", getpid());
            write(pongfd[1], &pong, sizeof(uint8));
            close(pongfd[1]);
        }
        else
        {
            fprintf(STDERR, "error: failed to read ping\n");
            exit(EXIT_FAILURE);
        }

        close(pingfd[0]);
    }
    else if (child > 0)
    {
        uint8 ping = PING;
        write(pingfd[1], &ping, sizeof(uint8));
        close(pingfd[1]);

        uint8 pong;
        read(pongfd[0], &pong, sizeof(uint8));
        close(pongfd[0]);

        if (pong == PONG)
        {
            fprintf(STDOUT, "%d: received pong\n", getpid());
        }
        else
        {
            fprintf(STDERR, "error: failed to read pong\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        fprintf(STDERR, "error: fork failed\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
