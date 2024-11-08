
#include <stdio.h> 
#include <fcntl.h>
#include <stdbool.h>
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>

#define ack_msg "OK"
  
int main() 
{ 
    int fd1, fd2; 
  
    char * myfifo = "/tmp/myfifo"; 
    mkfifo(myfifo, 0666); 

    char * ack = "/tmp/ackfifo";
    mkfifo(ack, 0666);
  
    char str1[80], str2[80]; 
    char format_string[80]="%d,%d";
    int n1, n2;
    double mean;
    bool quit = false;

    while (1) 
    { 
        fd1 = open(myfifo,O_RDONLY);
        read(fd1, str1, 80);
 
        if (str1[0] != 'q') {
            /* read numbers from input line */
            sscanf(str1, format_string, &n1, &n2);
            mean = (n1 + n2) / 2.0; 
            printf("mean value is: %f, sum is: %d\n", mean, n1 + n2); 
            close(fd1);
        } else {
            quit = true;
        }

        printf("sending ack\n"); 
        sleep(3);
        fd2 = open(ack, O_WRONLY);
        write(fd2, ack_msg, strlen(ack_msg)+1); 
        close(fd2);
        if (quit) {
            exit(EXIT_SUCCESS);
        }
    } 
    return 0; 
} 
