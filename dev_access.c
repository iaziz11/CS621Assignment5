#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <ctype.h>


int main(int argc, char *argv[]) {
    if (argv[1] == "1") {
        int fd = open("/dev/input/mouse0", O_RDONLY);
        if (fd < 0) {
            perror("Could not open /dev/input/mouse0");
            exit(EXIT_FAILURE);
        }
        unsigned char buf;
        ssize_t bytes_read;

        while (1) {
            bytes_read = read(fd, &buf, 1);
            if (bytes_read < 0) {
                perror("Could not read mouse0.");
                exit(EXIT_FAILURE);
            } else if (bytes_read == 0) {
                break; // EOF
            }
            printf("%d\n", buf);
        }
    } else if (argv[1] == "2") {
        int fd_in = open("/dev/urandom", O_RDONLY);
        if (fd_in < 0){
            perror("Could not open urandom");
            exit(EXIT_FAILURE);
        };

        int fd_out = open("/dev/null", O_WRONLY);
        if (fd_out < 0) {
            perror("Could not open null");
            exit(EXIT_FAILURE);
        };

        unsigned char *buffer = malloc(10000000);
        if (!buffer) {
            close(fd_in);
            close(fd_out);
            perror("Malloc failed");
            exit(EXIT_FAILURE);;
        }

        struct timeval start, end;
        if (gettimeofday(&start, NULL) != 0){
            perror("Time of day start error");
            exit(EXIT_FAILURE);
        }

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

        ssize_t total_written = 0;
        while (total_written < TEN_MB) {
            ssize_t n = write(fd_out, buffer + total_written, TEN_MB - total_written);
            if (n < 0) {
                free(buffer);
                close(fd_in);
                close(fd_out);
                perror("Could not write to null");
                exit(EXIT_FAILURE);
            }
            total_written += n;
        }

        if (gettimeofday(&end, NULL) != 0){
            perror("Time of day end error");
            exit(EXIT_FAILURE);
        }

        double elapsed = (end.tv_sec - start.tv_sec) +
                        (end.tv_usec - start.tv_usec) / 1000000.0;

        printf("Elapsed time: %.6f seconds\n", elapsed);

        free(buffer);
        close(fd_in);
        close(fd_out);
    } else {
        fprintf(stderr, "Usage: %s [1]\n", argv[0]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
