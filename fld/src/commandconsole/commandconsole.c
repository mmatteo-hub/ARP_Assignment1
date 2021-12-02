#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>
#include <time.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"

// definintion for al variables used inside the program
int fdX, fdZ, fdX_write, fdZ_write;
char format_string[80] = "%d";
char format_str_var[80] = "%c";
char pid_motX[80];
char pid_motZ[80];
char pid_WD[80];
char pid[80];
int pidX_got, pidZ_got, pid_wd;

// defininf file pointer and a time variable to read the current date
FILE *f;
time_t clk;

// main
int main(int argc, char * argv[])
{
    // opening the log file in append mod to add to the existing file
    f = fopen("./../log/logfile.txt","a");
    
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

    // opening pipes to get the value of X and Z motors and pass them to the respective motor
    fdX_write = open(fifo_valX, O_WRONLY);
    fdZ_write = open(fifo_valZ, O_WRONLY);
    
    while(1)
    {
        // showing the menù
        printf("%sPRESS: \n W (or w) to go UP\n Z (or z) to go DOWN\n D (or d) to go RIGHT\n A (or a) to go LEFT\n\n Q (or q) to STOP X\n E (or e) to STOP Z\n",KNRM);
        fflush(stdout);
        scanf("%s",ch1);

        // checking the correctness for the input: 1 character only
        if(strlen(ch1) != 1)
        {
            // writing on the log file
            fseek(f,0,SEEK_END);
            clk = time(NULL);
            fprintf(f,"Command Console, too many inputs inserited at : %s",ctime(&clk));
            fflush(f);
        }

        else
        {
            char out_str[80];
            sprintf(out_str,format_string,ch1[0]);
            var = ch1[0];

            // switch to check the input given and to write the command to the respectively motor according to pipes
            // write, for each case, in the log file
            switch(var)
            {
                case 119: // case w
                case 87: // case W
                    fseek(f,0,SEEK_END);
                    clk = time(NULL);
                    fprintf(f,"Command Console, UP command inserted at : %s",ctime(&clk));
                    fflush(f);
                    write(fdZ_write, out_str, strlen(out_str)+1);
                    break;

                case 122 :// case z
                case 90: // case Z
                    fseek(f,0,SEEK_END);
                    clk = time(NULL);
                    fprintf(f,"Command Console, DOWN command inserted at : %s",ctime(&clk));
                    fflush(f);
                    write(fdZ_write, out_str, strlen(out_str)+1);
                    break;
                    
                case 97: // case a
                case 65: // case A
                    fseek(f,0,SEEK_END);
                    clk = time(NULL);
                    fprintf(f,"Command Console, LEFT command inserted at : %s",ctime(&clk));
                    fflush(f);
                    write(fdX_write, out_str, strlen(out_str)+1);
                    break;

                case 100: // case d
                case 68: // case D
                    fseek(f,0,SEEK_END);
                    clk = time(NULL);
                    fprintf(f,"Command Console, RIGHT command inserted at : %s",ctime(&clk));
                    fflush(f);
                    write(fdX_write, out_str, strlen(out_str)+1);
                    break;

                case 113: // case q
                case 81: // case Q
                    fseek(f,0,SEEK_END);
                    clk = time(NULL);
                    fprintf(f,"Command Console, STOP X command inserted at : %s",ctime(&clk));
                    fflush(f);
                    write(fdX_write, out_str, strlen(out_str)+1);
                    break;

                case 101: // case e
                case 69: // case E
                    fseek(f,0,SEEK_END);
                    clk = time(NULL);
                    fprintf(f,"Command Console, STOP Z command inserted at : %s",ctime(&clk));
                    fflush(f);
                    write(fdZ_write, out_str, strlen(out_str)+1);
                    break;

                default:
                    fseek(f,0,SEEK_END);
                    clk = time(NULL);
                    fprintf(f,"Command Console, invalid command inserted at : %s",ctime(&clk));
                    fflush(f);
                    break;
            }
            // send a signal to the watchdog to restart the counter
            kill(pid_wd,SIGALRM);
        }
        // clear the konsole to have a clear output
        system("clear");
    }
    // close pipes
    close(fdZ_write);
    close(fdX_write);

    // unlink pipes
    unlink(fifo_valZ);
    unlink(fifo_valX);
}