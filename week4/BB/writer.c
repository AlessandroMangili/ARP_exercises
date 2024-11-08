#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

void write_process(int pipe[], int id) {
    // Close useless file descriptor
    close(pipe[0]);
    close(pipe[3]);

    int random_number;
    while(1) {
        // Send a '?' character to the server to request writing the number
        write(pipe[1], "?", 1);
        printf("[WRITER%d]: character '?' sent to the server\n", id);

        // Waits for the response from the server
        char response[256];
        read(pipe[2], response, sizeof(response));

        random_number = rand() % 100;
        char ascii_num[256];
        sprintf(ascii_num, "%d", random_number);

        // Send the converted number to the server
        write(pipe[1], ascii_num, sizeof(ascii_num));
        printf("[WRITER%d]: sent the random number %d to the server\n", id, random_number);
        sleep(rand() % 10 + 1);
    }
    close(pipe[1]);
    close(pipe[2]);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    if (argc != 5) {
        fprintf(stderr, "[WRITER]: invalid number of parameters for writer\n");
        exit(EXIT_FAILURE);
    }

    int pipe[4];
    for (int i = 0; i < 4; i++) {
        pipe[i] = atoi(argv[i]);
    }

    write_process(pipe, atoi(argv[4]));
    return 0;
}