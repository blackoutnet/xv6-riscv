#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/param.h"
#include "kernel/stat.h"
#include "user/defs.h"
#include "user/user.h"

void find(char* search_dir, char* search_file);

int main(int argc, char** argv)
{
    if (argc < 3) {
        fprintf(STDERR, "usage: find directory expression\n");
        exit(EXIT_FAILURE);
    }

    char* search_dir = argv[1];
    char* search_file = argv[2];

    int search_dir_fd;
    if ((search_dir_fd = open(search_dir, 0)) < 0) {
        fprintf(STDERR, "find: cannot open %s\n", search_dir);
        exit(EXIT_FAILURE);
    }

    struct stat search_dir_stat;
    if (fstat(search_dir_fd, &search_dir_stat) < 0) {
        fprintf(STDERR, "find: cannot stat %s\n", search_dir);
        close(search_dir_fd);
        exit(EXIT_FAILURE);
    }
    close(search_dir_fd);

    switch (search_dir_stat.type) {
    case T_DIR:
        find(search_dir, search_file);
        break;
    case T_DEVICE:
    case T_FILE:
        printf("find : `%s`: not a directory\n", search_dir);
        break;
    }

    exit(EXIT_SUCCESS);
}

void find(char* search_dir, char* search_file)
{
    int dir_fd;
    if ((dir_fd = open(search_dir, 0)) < 0) {
        fprintf(STDERR, "find: cannot open %s\n", search_dir);
        exit(EXIT_FAILURE);
    }

    char path[MAXPATH];
    char* path_end = path + strlen(search_dir);
    strcpy(path, search_dir);

    struct dirent de;
    while (read(dir_fd, &de, sizeof(de)) == sizeof(de)) {
        char* filename = de.name;
        if (de.inum != 0 && strcmp(filename, ".") && strcmp(filename, "..")) {
            if (*(path_end - 1) != '/') {
                strcpy(path_end, "/");
                path_end += 1;
            }
            *path_end = '\0';
            strcpy(path_end, filename);

            int fd;
            if ((fd = open(path, 0)) < 0) {
                fprintf(STDERR, "find: cannot open %s\n", path);
                continue;
            }

            struct stat file_stat;
            if (fstat(fd, &file_stat) < 0) {
                fprintf(STDERR, "find: cannot stat %s\n", path);
                close(fd);
                continue;
            }

            switch (file_stat.type) {
            case T_DIR:
                find(path, search_file);
                break;
            case T_DEVICE:
                break;
            case T_FILE:
                if (!strcmp(search_file, filename)) {
                    printf("%s\n", path);
                }
                break;
            }
            close(fd);
        }
    }

    close(dir_fd);
}
