#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <ctype.h>


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ./dev_access [int]\n");
        return -1;
    }
    if (strcmp(argv[1], "1") == 0) {
        // on my system, mouse0 does not do anything, but mouse2 does

        // int fd = open("/dev/input/mouse0", O_RDONLY);
        // if (fd < 0) {
        //     perror("Could not open /dev/input/mouse0");
        //     exit(EXIT_FAILURE);
        // }
        // unsigned char buf;
        // ssize_t bytes_read;
        
        // while (1) {
        //     bytes_read = read(fd, &buf, 1);
        //     if (bytes_read < 0) {
        //         perror("Could not read mouse0.");
        //         exit(EXIT_FAILURE);
        //     } else if (bytes_read == 0) {
        //         break; // EOF
        //     }
        //     printf("%d\n", buf);
        // }

        // open mouse2
        int fd = open("/dev/input/mouse2", O_RDONLY);
        if (fd < 0) {
            perror("Could not open /dev/input/mouse2");
            exit(EXIT_FAILURE);
        }
        unsigned char buf;
        ssize_t bytes_read;
        
        // keep on reading mouse2
        while (1) {
            bytes_read = read(fd, &buf, 1);
            if (bytes_read < 0) {
                perror("Could not read mouse2.");
                exit(EXIT_FAILURE);
            } else if (bytes_read == 0) {
                break; // EOF
            }
            printf("%d\n", buf);
        }
    } else if (strcmp(argv[1], "2") == 0) {
        // open urandom
        int fd_in = open("/dev/urandom", O_RDONLY);
        if (fd_in < 0){
            perror("Could not open urandom");
            exit(EXIT_FAILURE);
        };


        // open null
        int fd_out = open("/dev/null", O_WRONLY);
        if (fd_out < 0) {
            perror("Could not open null");
            exit(EXIT_FAILURE);
        };

        // reserve 10 mb for buffer
        unsigned char *buffer = malloc(10000000);
        if (!buffer) {
            close(fd_in);
            close(fd_out);
            perror("Malloc failed");
            exit(EXIT_FAILURE);;
        }

        // time start
        struct timeval start, end;
        if (gettimeofday(&start, NULL) != 0){
            perror("Time of day start error");
            exit(EXIT_FAILURE);
        }

        //read until 10mbs have been read
        ssize_t total_read = 0;
        while (total_read < 10000000) {
            ssize_t n = read(fd_in, buffer + total_read, 10000000 - total_read);
            if (n < 0) {
                free(buffer);
                close(fd_in);
                close(fd_out);
                perror("Could not read urandom");
                exit(EXIT_FAILURE);
            }
            total_read += n;
        }

        // write buffer out to null
        ssize_t total_written = 0;
        while (total_written < 10000000) {
            ssize_t n = write(fd_out, buffer + total_written, 10000000 - total_written);
            if (n < 0) {
                free(buffer);
                close(fd_in);
                close(fd_out);
                perror("Could not write to null");
                exit(EXIT_FAILURE);
            }
            total_written += n;
        }

        // time end
        if (gettimeofday(&end, NULL) != 0){
            perror("Time of day end error");
            exit(EXIT_FAILURE);
        }

        // calculate how long has passed
        double elapsed = (end.tv_sec - start.tv_sec) +
                        (end.tv_usec - start.tv_usec) / 1000000.0;

        printf("Elapsed time: %.6f seconds\n", elapsed);

        free(buffer);
        close(fd_in);
        close(fd_out);
    } else {
        printf("Usage: ./dev_access [int]\nMake sure int is 1 or 0.\n");
        return -1;
    }

    return EXIT_SUCCESS;
}
