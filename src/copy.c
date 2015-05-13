#define _BSD_SOURCE  1
#define _SVID_SOURCE 1
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include"copy.h"

int main() {

    return 0;
}

int fd_open_out(int fd_in, struct dirent* file) {
    int fd_out;
    if (isDirectory(file, file->d_name)) {
        /*Create the directory */
    } else {

        if ((fd_out = creat(file->d_name, O_WRONLY)) == EXIT_FAILURE) {
            perror("copy_out");
            return EXIT_FAILURE;
        }

        if (copy(fd_out, fd_in) == EXIT_FAILURE) {
            perror("fd_open_in");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int fd_open_in(int fd_out, struct dirent* file) {
    int fd_in;
    if (isDirectory(file, file->d_name)) {
        /*Create the directory */
    } else {

        if ((fd_in = open(file->d_name, O_RDONLY)) == EXIT_FAILURE) {
            perror("fd_open_in");
            return EXIT_FAILURE;
        }

        if (copy(fd_out, fd_in) == EXIT_FAILURE) {
            perror("fd_open_in");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int copy(int fd_in, int fd_out) {
    int nb_reads;
    char buffer[10];


    while ((nb_reads = read(fd_in, buffer, 10)) == 10) {

        if (write(fd_out, buffer, 10) == EXIT_FAILURE) {
            perror("copy");
            return EXIT_FAILURE;
        }
    }

    if (nb_reads == -1) {
        perror("copy");
        return EXIT_FAILURE;
    } else {
        if (write(fd_out, buffer, nb_reads) == EXIT_FAILURE) {
            perror("copy");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int copy_all(int fd, Fd_open func) {
    struct dirent ** reader;
    int nb_file;
    int i;

    if ((nb_file = scandir("./", &reader, NULL, alphasort)) == EXIT_FAILURE) {
        perror("copy_all");
        return EXIT_FAILURE;
    }

    for (i = 0; i < nb_file; i++) {
        if (func(fd, reader[i]) == EXIT_FAILURE) {
            perror("copy_all");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int isDirectory(struct dirent* info, char* path) {
    struct stat fich;

    if (stat(path, &fich) == EXIT_FAILURE) {
        perror("isDirectory");
        return EXIT_FAILURE;
    }

    if (S_ISDIR(fich.st_mode))
        return 1;
    return 0;
}

