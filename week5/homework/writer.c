#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

void start_writer(int pipe[], char *message, int rate) {
    // Close useless file descriptor
    close(pipe[0]);
    int time = rate;
    while(1) {
        write(pipe[1], message, 1);
        usleep(time);
        
        // Decrease the delay by 10% each iteration
        time = (int)(time * 0.9);
        // Stop decreasing time if it’s very low to avoid going too fast
        if (time <= 10000) time = 10000;
    }
    close(pipe[1]);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Invalid number of parameters for writer\n");
        exit(EXIT_FAILURE);
    }

    int pipe[2];
    for (int i = 0; i < 2; i++) {
        pipe[i] = atoi(argv[i]);
    }
    
    start_writer(pipe, argv[2], atoi(argv[3]));
    return 0;
}