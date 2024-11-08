
#include <stdio.h> 
#include <string.h> 
#include <stdbool.h>
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
  
int main() 
{ 
    int fd1, fd2; 
  
    char * myfifo = "/tmp/myfifo"; 
    mkfifo(myfifo, 0666); 
    char * myfifo2 = "/tmp/myfifo2"; 
    mkfifo(myfifo2, 0666); 
  
    char str1[80], str2[80]; 
    char format_string[80]="%d,%d";
    char format_string2[80]="%d,%d";
    int n1, n2;
    double mean;
    bool quit = false;

    while (1) 
    { 
    
        fd1 = open(myfifo,O_RDONLY); 
        fd2 = open(myfifo2,O_WRONLY); 
        read(fd1, str1, 80); 
 
        if (str1[0] != 'q') {
            sscanf(str1, format_string, &n1, &n2);
            mean = (n1 + n2) / 2.0;
            printf("mean value is: %f, sum is: %d\n", mean, n1 + n2); 
            close(fd1); 

            sprintf(str2, format_string2, n1, n2);
        } else {
            quit = true;
            strncpy(str2, str1, 80);
        }

        write(fd2,str2, strlen(str2)+1);
        close(fd2);

        if (quit) {
            exit(EXIT_SUCCESS);
        }
    }
    return 0; 
} 
