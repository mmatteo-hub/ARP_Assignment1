#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>

char format_string[80] = "%d";
char format_str_var[80] = "%c";

int main(int argc, char * argv[])
{
    char outup_string[80];

    // pipe communicating with the executor
    char * myfifo_command = "/tmp/fifo_command";

    // pipes to pass the value from command console to the motorX and motorZ respectevely
    char * fifo_valX = "/tmp/fifo_valX";
    char * fifo_valZ = "/tmp/fifo_valZ";
    mkfifo(myfifo_command,0666);
    mkfifo(fifo_valX,0666);

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