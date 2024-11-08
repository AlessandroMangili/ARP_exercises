#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

// Number of processes
#define NUM_CLIENTS 2
#define RATE_P1 "1000000"  // 1sec
#define RATE_P2 "500000" // 0.5sec

void server(int pipes[][2]) {
    printf("SERVER STARTED\n\n");
    // Close useless file descriptor
    for (int i = 0; i < NUM_CLIENTS; i++) {
        close(pipes[i][1]);
    }

    int output_fd = open("file.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    // Open the file for writing
    if (output_fd < 0) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int max_fd = -1;
    // Find the maximum file descriptor to set the select
    for (int i = 0; i < NUM_CLIENTS; i++) {
        if (pipes[i][0] > max_fd) {
            max_fd = pipes[i][0];
        }
    }

    fd_set read_fds; // Set of file descriptor for the 'select'
    struct timeval timeout;
    while(1) {  
        FD_ZERO(&read_fds);
        // Add all file descriptors to monitor requests from processes
        for (int i = 0; i < NUM_CLIENTS; i++) {
            FD_SET(pipes[i][0], &read_fds);
        }

        // Timeout of 1 second
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int activity = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);
        if (activity < 0) {
            perror("Error in the select");
            close(output_fd);
            exit(EXIT_FAILURE);
        } else if (activity) {
            for (int i = 0; i < NUM_CLIENTS; i++) {        
                if(FD_ISSET(pipes[i][0],&read_fds)) {
                    char buffer;
                    read(pipes[i][0], &buffer, 1);
                    write(output_fd, &buffer, 1);
                    printf("Received %c from P%d\n", buffer, i);
                }
            }
        } else {
            printf("No process are ready to write\n");
            sleep(2);
        }
    }
    close(output_fd);
    for (int i = 0; i < NUM_CLIENTS; i++) {
        close(pipes[i][0]);
    }
}

int main(int argc, char *argv[]) {
    int pipes[NUM_CLIENTS][2];
    // Create pipes for each client
    for (int i = 0; i < NUM_CLIENTS; i++) {
        // Create the pipe for client to server communication
        if (pipe(pipes[i]) == -1) {
            perror("Pipe");
            exit(EXIT_FAILURE);
        }
    }

    char fd_str[NUM_CLIENTS][10];
    int index = 0;
    for (int i = 0; i < NUM_CLIENTS; i++) {
        snprintf(fd_str[index++], sizeof(fd_str[0]), "%d", pipes[i][0]);
        snprintf(fd_str[index++], sizeof(fd_str[0]), "%d", pipes[i][1]);
    }

    pid_t pid, pids[NUM_CLIENTS];
    pid = fork();
    if (pid < 0) {
        perror("Failed to execute the fork command");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        server(pipes);
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
            if (execl("./writer", 
            fd_str[i * NUM_CLIENTS], fd_str[1 + i * NUM_CLIENTS], 
            i == 0 ? "A" : "B", 
            i == 0 ? RATE_P1 : RATE_P2,
            (char *)NULL) == -1) {
                perror("Failed to execute the execl command");
                exit(EXIT_FAILURE); 
            }
            exit(0);
        }
    }

    for (int i = 0; i < NUM_CLIENTS + 1; i++) {
        wait(NULL);
    }

    return 0;
}