#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/defs.h"
#include "user/user.h"

#define MAX_LINE_LENGTH 100

int get_line(char* buffer);
void parse_line(char* line, char** xargv, int* xargc, int argc);
void fork_exec(int* child, char** xargv, int xargc, int argc);
void free_xargv(char** xargv, int from, int to);

int main(int argc, char** argv)
{
    char* xargv[MAXARG];

    for (int i = 1; i < argc; i++) {
        uint arglen = strlen(argv[i]) + 1;
        xargv[i - 1] = malloc(arglen);
        strcpy(xargv[i - 1], argv[i]);
    }
    argc -= 1;

    int child = 0;
    char line[MAX_LINE_LENGTH];
    while (get_line(line) > 0) {
        int xargc;
        parse_line(line, xargv, &xargc, argc);
        fork_exec(&child, xargv, xargc, argc);
    }

    if (child > 0) {
        free_xargv(xargv, 0, argc);
    }

    exit(EXIT_SUCCESS);
}

int get_line(char* buffer)
{
    gets(buffer, MAX_LINE_LENGTH);
    return buffer[0] != 0;
}

void parse_line(char* line, char** xargv, int* xargc, int argc)
{
    *xargc = argc;
    int i = 0;
    while (line[i]) {
        const char* whitespaces = " \t\n\v";
        while (strchr(whitespaces, line[i])) {
            i++;
        }
        if (!line[i]) {
            break;
        }

        int j = 0;
        while (!strchr(whitespaces, line[i])) {
            j++;
            i++;
        }
        xargv[*xargc] = malloc(j * sizeof(char) + 1);
        strcpy(xargv[*xargc], line + i - j);
        xargv[*xargc][j] = '\0';

        line[i++] = '\0';
        (*xargc)++;
    }
}

void fork_exec(int* child, char** xargv, int xargc, int argc)
{
    *child = fork();
    if (*child == 0) {
        int result = exec(xargv[0], xargv);
        if (result < 0) {
            printf("exec: failed to execute: %s\n", xargv[0]);
            free_xargv(xargv, 0, xargc);
            exit(EXIT_FAILURE);
        }
    } else if (*child > 0) {
        free_xargv(xargv, argc, xargc);
        wait(0);
    } else {
        fprintf(STDERR, "error: fork faled\n");
        exit(EXIT_FAILURE);
    }
}

void free_xargv(char** xargv, int from, int to)
{
    for (int i = from; i < to; i++) {
        free(xargv[i]);
    }
}
