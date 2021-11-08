#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>

int fd;
char str[80];
char format_string[80] = "%d";

int open_pipe(int ch2, char* pipe_addr)
{
    sprintf(str, format_string, ch2);
    fd = open(pipe_addr, O_WRONLY);
    write(fd, str, strlen(str)+1);
    close(fd);
}


int main(int argc, char * argv[])
{
    // pipe from command console to motors
    char * fifo_comm_mot = "/tmp/comm_mot";
    mkfifo(fifo_comm_mot,0666);

    char ch1, ch2;
    char var;
    printf("PRESS: \n w to go UP\n z to go DOWN\n d to do RIGHT\n a to go LEFT\n");
    fflush(stdout);
    //scanf("%c",&var);
    var = getchar();
    
    if(var == 119)
    {
        printf("UP WAS PRESSED\n");
        fflush(stdout);
        open_pipe(var, fifo_comm_mot);
    }
    else if(var == 122)
    {
        printf("DOWN WAS PRESSED\n");
        fflush(stdout);
        open_pipe(var, fifo_comm_mot);
    }
    else if(var == 97)
    {
        printf("LEFT WAS PRESSED\n");
        fflush(stdout);
        open_pipe(var, fifo_comm_mot);
    }
    else if(var == 100)
    {
        printf("RIGHT WAS PRESSED\n");
        fflush(stdout);
        open_pipe(var, fifo_comm_mot);
    }
    else
    {
        printf("key pressed: %c\n", var);
        fflush(stdout);
    }
    sleep(20);
    //system("pause");
}