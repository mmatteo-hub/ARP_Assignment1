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
char format_str_var[80] = "%c";

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
    char * fifo_comm_motX = "/tmp/comm_motX";
    char * fifo_comm_motZ = "/tmp/comm_motZ";
    mkfifo(fifo_comm_motX,0666);
    mkfifo(fifo_comm_motZ,0666);

    char ch1[80];
    char var;
    
    while(1)
    {
        printf("PRESS: \n w to go UP\n z to go DOWN\n d to do RIGHT\n a to go LEFT\n");
        fflush(stdout);
        scanf("%s",ch1);

        if(strlen(ch1) != 1)
        {
            printf("Wrong input. Input has to be 1 character only!\n");
            fflush(stdout);
        }

        else
        {
            var = ch1[0];
            switch(var)
            {
                case 119:
                    printf("UP WAS PRESSED\n");
                    fflush(stdout);
                    open_pipe(var, fifo_comm_motZ);
                    break;

                case 122:
                    printf("DOWN WAS PRESSED\n");
                    fflush(stdout);
                    open_pipe(var, fifo_comm_motZ);
                    break;
                    
                case 97:
                    printf("LEFT WAS PRESSED\n");
                    fflush(stdout);
                    open_pipe(var, fifo_comm_motX);
                    break;

                case 100:
                    printf("RIGHT WAS PRESSED\n");
                    fflush(stdout);
                    open_pipe(var, fifo_comm_motX);
                    break;

                default:
                    printf("Wrong input, key pressed: %c\n", var);
                    fflush(stdout);
                    break;
            }
        }
    }
}