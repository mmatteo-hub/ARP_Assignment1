#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"

int fdX, fdZ, fdX_write, fdZ_write;
char format_string[80] = "%d";
char format_str_var[80] = "%c";
char pid_motX[80];
char pid_motZ[80];
char pid_WD[80];
int pidX_got, pidZ_got, pid_wd;

int main(int argc, char * argv[])
{
    char outup_string[80];
    // pipe from command console to motors
    char * fifo_valX = "/tmp/fifo_valX";
    char * fifo_valZ = "/tmp/fifo_valZ";
    char * comm_wd = "/tmp/commd_wd";
    mkfifo(fifo_valX,0666);
    mkfifo(fifo_valZ,0666);
    mkfifo(comm_wd,0666);

    // get watchdog pid
    int fd_wdComm = open(comm_wd,O_RDONLY);
    read(fd_wdComm,pid_WD,80);
    pid_wd = atoi(pid_WD);
    close(fd_wdComm);
    
    char ch1[80];
    char var;
    
    while(1)
    {
        fdX_write = open(fifo_valX, O_WRONLY);
        fdZ_write = open(fifo_valZ, O_WRONLY);

        printf("%sPRESS: \n w to go UP\n z to go DOWN\n d to go RIGHT\n a to go LEFT\n\n Q to STOP X\n E to STOP Z\n",KNRM);
        fflush(stdout);
        scanf("%s",ch1);

        if(strlen(ch1) != 1)
        {
            printf("%sWrong input. Input has to be 1 character only!\n",KRED);
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
                    printf("%sUP WAS PRESSED\n",KBLU);
                    fflush(stdout);
                    write(fdZ_write, out_str, strlen(out_str)+1);
                    break;

                case 122 :// case z
                case 90: // case Z
                    printf("%sDOWN WAS PRESSED\n",KBLU);
                    fflush(stdout);
                    write(fdZ_write, out_str, strlen(out_str)+1);
                    break;
                    
                case 97: // case a
                case 65: // case A
                    printf("%sLEFT WAS PRESSED\n",KBLU);
                    fflush(stdout);
                    write(fdX_write, out_str, strlen(out_str)+1);
                    break;

                case 100: // case d
                case 68: // case D
                    printf("%sRIGHT WAS PRESSED\n",KBLU);
                    fflush(stdout);
                    write(fdX_write, out_str, strlen(out_str)+1);
                    break;

                case 113: // case q
                case 81: // case Q
                    printf("%sSTOP X WAS PRESSED\n",KBLU);
                    fflush(stdout);
                    write(fdX_write, out_str, strlen(out_str)+1);
                    break;

                case 101: // case e
                case 69: // case E
                    printf("%sSTOP Z WAS PRESSED\n",KBLU);
                    fflush(stdout);
                    write(fdZ_write, out_str, strlen(out_str)+1);
                    break;

                default:
                    printf("%sWrong input, key pressed: %c\n",KYEL, var);
                    fflush(stdout);
                    break;
            }
            // send a signal to the watchdog to restart the counter
            kill(pid_wd,SIGALRM);
        }
        sleep(2);
        system("clear");
        close(fdZ_write);
        close(fdX_write);
    }
    unlink(fifo_valZ);
    unlink(fifo_valX);
}