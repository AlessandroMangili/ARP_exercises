#include <stdio.h>
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <sys/wait.h>

#define NUM_CLIENTS 4

int main() {
    int pipes[NUM_CLIENTS][2][2];
    // Create pipes for each client
    for (int i = 0; i < NUM_CLIENTS; i++) {
        // Create the pipe for client to server communication
        if (pipe(pipes[i][0]) == -1) {
            perror("Client pipe");
            exit(EXIT_FAILURE);
        }

        // Create the pipe for server to client communication
        if (pipe(pipes[i][1]) == -1) {
            perror("Server pipe");
            exit(EXIT_FAILURE);
        }
    }

    char fd_str[NUM_CLIENTS * 4][10]; // NUM_CLIENTS * 4 stringhe di 10 caratteri ciascuna
    int index = 0;
    for (int i = 0; i < NUM_CLIENTS; i++) {
        snprintf(fd_str[index++], sizeof(fd_str[0]), "%d", pipes[i][0][0]);
        snprintf(fd_str[index++], sizeof(fd_str[0]), "%d", pipes[i][0][1]);
        snprintf(fd_str[index++], sizeof(fd_str[0]), "%d", pipes[i][1][0]);
        snprintf(fd_str[index++], sizeof(fd_str[0]), "%d", pipes[i][1][1]);
    }

    pid_t pid, pids[NUM_CLIENTS];
    pid = fork();
    if (pid < 0) {
        perror("Failed to execute the fork command");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        if (execl("./server",
              fd_str[0], fd_str[1], fd_str[2], fd_str[3],
              fd_str[4], fd_str[5], fd_str[6], fd_str[7],
              fd_str[8], fd_str[9], fd_str[10], fd_str[11],
              fd_str[12], fd_str[13], fd_str[14], fd_str[15],
              (char *)NULL) == -1) {
            perror("Failed to execute the execl command");
            exit(EXIT_FAILURE);
        }
        exit(0);
    }

    sleep(1);

    for (int i = 0; i < NUM_CLIENTS; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("Failed to execute the fork command");
            exit(EXIT_FAILURE);
        }
        if (pids[i] == 0) {
            char id[10];
            if (i < 2) {
                sprintf(id, "%d", i+1);      
                if (execl("./writer", fd_str[i * NUM_CLIENTS], fd_str[1 + i * NUM_CLIENTS], fd_str[2 + i * NUM_CLIENTS], fd_str[3 + i * NUM_CLIENTS], id,(char *)NULL) == -1) {
                    perror("Failed to execute the execl command");
                    exit(EXIT_FAILURE); 
                }
                exit(EXIT_SUCCESS);
            } else {
                sprintf(id, "%d", (i%2)+1);  
                if (execl("./reader", fd_str[i * NUM_CLIENTS], fd_str[1 + i * NUM_CLIENTS], fd_str[2 + i * NUM_CLIENTS], fd_str[3 + i * NUM_CLIENTS], id, (char *)NULL) == -1) {
                    perror("Failed to execute the execl command");
                    exit(EXIT_FAILURE); 
                }
                exit(EXIT_SUCCESS);
            }
            exit(0);
        }
        sleep(1);
    }

    for (int i = 0; i < NUM_CLIENTS + 1; i++) {
        wait(NULL);
    }

    return 0;
}