#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>

int fdX, fdZ;
char format_string[80] = "%d";
char format_str_var[80] = "%c";
char pid_motX[80];
char pid_motZ[80];
int pidX_got;
int pidZ_got;

int main(int argc, char * argv[])
{
    char outup_string[80];

    // pipes from command console to motors
    char * fifo_comm_motX = "/tmp/comm_motX";
    char * fifo_comm_motZ = "/tmp/comm_motZ";

    // pipe communicating with the executor
    char * myfifo_command = "/tmp/fifo_command";

    // pipes to pass the value from command console to the motorX and motorZ respectevely
    char * fifo_valX = "/tmp/fifo_valX";
    char * fifo_valZ = "/tmp/fifo_valZ";
    mkfifo(fifo_comm_motX,0666);
    mkfifo(fifo_comm_motZ,0666);
    mkfifo(myfifo_command,0666);
    mkfifo(fifo_valX,0666);

    // takes the pid of motorX and stores it into a variable
    fdX = open(fifo_comm_motX,O_RDONLY);
    read(fdX, pid_motX, 80);
    sscanf(pid_motX, format_string, &pidX_got);
    close(fdX);
    unlink(fifo_comm_motX);

    // takes the pid of motorZ and stores it into a variable
    fdZ = open(fifo_comm_motZ,O_RDONLY);
    read(fdZ, pid_motZ, 80);
    sscanf(pid_motZ, format_string, &pidZ_got);
    close(fdZ);
    unlink(fifo_comm_motZ);
    
    char ch1[80];
    char var;
    
    while(1)
    {
        // opens pipe to write the command inserted by the user
        int fdX_write = open(fifo_valX, O_WRONLY);
        int fdZ_write = open(fifo_valZ, O_WRONLY);
        printf("PRESS: \n w to go UP\n z to go DOWN\n d to go RIGHT\n a to go LEFT\n\n q to STOP X\n e to STOP Z\n");
        fflush(stdout);
        scanf("%s",ch1);

        if(strlen(ch1) != 1) // input error because two or more characters written
        {
            printf("Wrong input. Input has to be 1 character only!\n");
            fflush(stdout);
        }

        else
        {
            char out_str[80];
            sprintf(out_str,format_string,ch1[0]); // prints the char according to a format string
            var = ch1[0];
            switch(var)
            {
                case 119: // case w
                case 87: // case W
                    printf("UP WAS PRESSED\n");
                    fflush(stdout);
                    write(fdZ_write, out_str, strlen(out_str)+1);
                    break;

                case 122 :// case z
                case 90: // case Z
                    printf("DOWN WAS PRESSED\n");
                    fflush(stdout);
                    write(fdZ_write, out_str, strlen(out_str)+1);
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
                    write(fdZ_write, out_str, strlen(out_str)+1);
                    break;

                default:
                    printf("Wrong input, key pressed: %c\n", var);
                    fflush(stdout);
                    break;
            }
        }
        // closes pipes
        close(fdZ_write);
        close(fdX_write);
    }
    unlink(fifo_valZ);
    unlink(fifo_valX);
}