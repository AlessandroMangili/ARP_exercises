#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>

#define N_PROCESSES 5
#define PIPE_NAME "/tmp/watchdog_pipe"
#define TIMEOUT 5

void process(char id) {
    int fd = open(PIPE_NAME, O_WRONLY);
    if (fd < 0) {
        perror("Could not open pipe for writing");
        exit(EXIT_FAILURE);
    }

    while (1) {
        write(fd, &id, 1);
        sleep((rand() % 5) + 1);
    }
    close(fd);
}

void watchdog() {
    int fd = open(PIPE_NAME, O_RDONLY);
    char id;
    time_t last_active[N_PROCESSES] = {0};
    time_t current_time;

    if (fd < 0) {
        perror("Could not open pipe for reading");
        exit(EXIT_FAILURE);
    }

    while (1) {
        read(fd, &id, 1);
        last_active[id - 'A'] = time(NULL);
        current_time = time(NULL);
        for (int i = 0; i < N_PROCESSES; i++) {
            if (current_time - last_active[i] > TIMEOUT) {
                printf("Alert! Process %c has been inactive for more than %d seconds.\n", 'A' + i, TIMEOUT);
                last_active[i] = current_time;
            }
        }
        sleep(1);
    }
    close(fd);
}

int main() {
    srand(time(NULL));
    int fd = mkfifo(PIPE_NAME, 0666);
    if (fd == -1) {
        perror("Could not create named pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Failed to execute the fork command");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        watchdog();
        exit(0);
    }

    for (int i = 0; i < N_PROCESSES; i++) {
        pid = fork();
        if (pid < 0) {
            perror("Failed to execute the fork command");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {
            char id = 'A' + i;
            process(id);
            exit(0);
        }
    }

    for (int i = 0; i < N_PROCESSES + 1; i++) {
        wait(NULL);
    }

    unlink(PIPE_NAME);
    return 0;
}
