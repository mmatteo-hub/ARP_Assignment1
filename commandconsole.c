#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>

int fd;
char format_string[80] = "%d";
char format_str_var[80] = "%c";
char pid_motX[80];
char pid_motZ[80];
int pidX_got;
int pidZ_got;

int open_pipe(int ch2, char* pipe_addr)
{
    char outup_string[80];
    sprintf(outup_string, format_string, ch2);
    fd = open(pipe_addr, O_WRONLY);
    write(fd, outup_string, strlen(outup_string)+1);
    close(fd);
}

int main(int argc, char * argv[])
{
    // pipe from command console to motors
    char * fifo_comm_motX = "/tmp/comm_motX";
    char * fifo_comm_motZ = "/tmp/comm_motZ";
    char * myfifo_command = "/tmp/fifo_command";
    char * fifo_valX = "/tmp/fifo_valX";
    mkfifo(fifo_comm_motX,0666);
    mkfifo(fifo_comm_motZ,0666);
    mkfifo(myfifo_command,0666);
    mkfifo(fifo_valX,0666);

    fd = open(fifo_comm_motX,O_RDONLY);
    read(fd, pid_motX, 80);
    sscanf(pid_motX, format_string, &pidX_got);
    close(fd);
    
    char ch1[80];
    char var;
    
    while(1)
    {
        printf("PRESS: \n w to go UP\n z to go DOWN\n d to go RIGHT\n a to go LEFT\n\n R to RESET\n S to STOP\n");
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
                    open_pipe(var, fifo_valX);
                    break;

                case 100:
                    printf("RIGHT WAS PRESSED\n");
                    fflush(stdout);
                    open_pipe(var, fifo_valX);
                    break;

                case 115:
                    printf("STOP WAS PRESSED\n");
                    fflush(stdout);
                    open_pipe(var, fifo_valX);
                    break;

                case 114:
                    printf("RESET WAS PRESSED\n");
                    fflush(stdout);
                    open_pipe(var, fifo_valX);
                    break;

                default:
                    printf("Wrong input, key pressed: %c\n", var);
                    fflush(stdout);
                    break;
            }
        }
    }
}