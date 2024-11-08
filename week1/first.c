#include <stdio.h>
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
  
int main() 
{ 
    int fd, fd1; 
   
    char * myfifo = "/tmp/myfifo"; 
    mkfifo(myfifo, 0666);

    char * ack = "/tmp/ackfifo";
    mkfifo(ack, 0666);
  
    char input_string[80], send_string[80], ack_string[80];

    while (1) 
    { 
      
        fd = open(myfifo, O_WRONLY); 

        printf("Please, write two integer numbers, separated by commas (,), or q to quit\n");
        /* to be sure that the previous is executed immediately */
	    fflush(stdout);
        /* read a full input line */
        fgets(input_string, 80 , stdin); 
	    sleep(1);
        write(fd, input_string, strlen(input_string)+1); 
        close(fd); 
        
        fd1 = open(ack, O_RDONLY);
	    read(fd1, ack_string, 80);
        printf("ack sent by second process is: %s\n", ack_string); 
        close(fd1);

        /* if the first input char is q, exit  */
        if (input_string[0] == 'q') exit(EXIT_SUCCESS) ;
    } 
    return 0; 
} 
