#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>

int fdX, fdZ, fdX_write, fdZ_write;
char format_string[80] = "%d";
char format_str_var[80] = "%c";
char pid_motX[80];
char pid_motZ[80];
int pidX_got;
int pidZ_got;

/*int open_pipe(int ch2, char* pipe_addr)
{
    char outup_string[80];
    sprintf(outup_string, format_string, ch2);
    fd = open(pipe_addr, O_WRONLY);
    write(fd, outup_string, strlen(outup_string)+1);
    close(fd);
}*/

int main(int argc, char * argv[])
{
    char outup_string[80];
    // pipe from command console to motors
    char * fifo_valX = "/tmp/fifo_valX";
    char * fifo_valZ = "/tmp/fifo_valZ";
    mkfifo(fifo_valX,0666);
    mkfifo(fifo_valZ,0666);
    
    char ch1[80];
    char var;
    
    while(1)
    {
        fdX_write = open(fifo_valX, O_WRONLY);
        //fdZ_write = open(fifo_valZ, O_WRONLY);
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
            char out_str[80];
            sprintf(out_str,format_string,ch1[0]);
            var = ch1[0];
            switch(var)
            {
                case 119: // case w
                case 87: // case W
                    printf("UP WAS PRESSED\n");
                    fflush(stdout);
                    //write(fdZ_write, out_str, strlen(out_str)+1);
                    break;

                case 122 :// case z
                case 90: // case Z
                    printf("DOWN WAS PRESSED\n");
                    fflush(stdout);
                    //write(fdZ_write, out_str, strlen(out_str)+1);
                    break;
                    
                case 97: // case a
                case 65: // case A
                    printf("LEFT WAS PRESSED\n");
                    fflush(stdout);
                    write(fdX_write, out_str, strlen(out_str)+1);
                    break;

                case 100: // case d
                case 68: // case D
                    printf("RIGHT WAS PRESSED\n");
                    fflush(stdout);
                    write(fdX_write, out_str, strlen(out_str)+1);
                    break;

                case 113: // case q
                case 81: // case Q
                    printf("STOP X WAS PRESSED\n");
                    fflush(stdout);
                    write(fdX_write, out_str, strlen(out_str)+1);
                    break;

                case 101: // case e
                case 69: // case E
                    printf("STOP Z WAS PRESSED\n");
                    fflush(stdout);
                    //write(fdZ_write, out_str, strlen(out_str)+1);
                    break;

                default:
                    printf("Wrong input, key pressed: %c\n", var);
                    fflush(stdout);
                    break;
            }
        }
        //close(fdZ_write);
        close(fdX_write);
    }
    //unlink(fifo_valZ);
    unlink(fifo_valX);
}