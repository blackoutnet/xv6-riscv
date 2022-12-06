#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/defs.h"
#include "user/user.h"

int main(int argc, char** argv) {
    int exit_code;
    if (argc < 2) {
        fprintf(STDERR, "usage: sleep ticks\n");
        exit_code = EXIT_FAILURE;
    } else {
        const char* input = argv[1];
        int ticks;
        if (strcmp(input, "0")) {
            ticks = atoi(input);
            if (ticks <= 0) {
                fprintf(STDERR, "sleep: ticks should be an integer value less than %d\n", INT_MAX);
                exit(EXIT_FAILURE);
            }
        } else {
            ticks = 0;
        }

        sleep(ticks);
    }

    exit(exit_code);
}

