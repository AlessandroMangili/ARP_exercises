#include <stdio.h>
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <sys/wait.h>

void write_fifo(char * myfifo) {
    int fd;
    char input_string[80];

    while (1) {
        fd = open(myfifo, O_WRONLY); 
        if (fd == -1) {
            perror("Open");
            exit(EXIT_FAILURE);
        }
        
        printf("Enter the numbers or q to exit: ");
        fflush(stdout);
        fgets(input_string, 80 , stdin);

        write(fd, input_string, strlen(input_string)+1); 
        close(fd);

        if (input_string[0] == 'q') exit(EXIT_SUCCESS);
        sleep(1);
    }
}

void read_fifo(int process, char * myfifo) {
    int n1, n2, n3;
    int fd;
    char str[80];

    while (1) {        
        fd = open(myfifo, process == 1 ? O_RDWR : O_RDONLY);
        if (fd == -1) {
            perror("Open");
            exit(EXIT_FAILURE);
        }
        
        read(fd, str, 80);        
        if (str[0] == 'q') {
            if (process == 1) {
                write(fd, str, strlen(str)+1);
            }
            exit(EXIT_SUCCESS);
        }

        sscanf(str, "%d,%d,%d", &n1, &n2, &n3);
        if (process == 1 && n1 != 1) {
            write(fd, str, strlen(str)+1);
        } else {
            printf("Process %d reads the numbers: %d %d\n", process, n2, n3);
            fflush(stdout);
        }        
        close(fd);
    }
}

int main() {
    pid_t pid1, pid2, pid3;    
    char * myfifo = "/tmp/myfifo";
    
    mkfifo(myfifo, 0666);

    pid1 = fork();
    if (pid1 < 0) {
        perror("Fork");
        return -1;
    }
    if (pid1 == 0) {
        write_fifo(myfifo);
        exit(0);
    }

    pid2 = fork();
    if (pid2 < 0) {
        perror("Fork");
        return -1;
    }
    if (pid2 == 0) {
        read_fifo(1, myfifo);
        exit(0);
    }

    pid3 = fork();
    if (pid3 < 0) {
        perror("Fork");
        return -1;
    }
    if (pid3 == 0) {
        sleep(2);
        read_fifo(2, myfifo);
        exit(0);
    }
    
    wait(NULL);
    wait(NULL);
    wait(NULL);

    printf("\nFather terminates after all child processes have terminated\n");

    return 0;
}