#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <string.h>

void read_process(int pipe[], int id) {
    // Close useless file descriptor
    close(pipe[0]);
    close(pipe[3]);

    while(1) {
        // Send a '!' character to the server to request reading the number 
        write(pipe[1], "!", 1);
        printf("[READER%d]: character '!' sent to the server\n", id);

        // Waits the response from the server
        char response[256];
        read(pipe[2], response, sizeof(response));
        printf("[READER%d]: reading the number from the server after sending the '!' character: %s\n", id, response);
        sleep(rand() % 10 + 1);
    }
    close(pipe[1]);
    close(pipe[2]);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "[READER]: invalid number of parameters for reader\n");
        exit(EXIT_FAILURE);
    }

    int pipe[4];
    for (int i = 0; i < 4; i++) {
        pipe[i] = atoi(argv[i]);
    }

    read_process(pipe, atoi(argv[4]));
    return 0;
}