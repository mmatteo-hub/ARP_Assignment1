#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <time.h>
#include <signal.h>

time_t t;

void sig_handler(int signo)
{
    if (signo == SIGUSR1)
    {
        printf("Signal received\n");
        t = time(NULL);
    }
}

int main(int argc, char * argv[])
{
    t = time(NULL);
    signal(SIGUSR1,sig_handler);
    int fd3;
    int fdp;
    int fdp2;
    char * myfifo3 = "/tmp/fifo_ex";
    char * myfifop = "/tmp/fifo_dog";
    char * myfifop2 = "/tmp/fifo_dog2";
    mkfifo(myfifo3,0666);
    mkfifo(myfifop,0666);
    mkfifo(myfifop2,0666);

    char str3[80]; 
    char strp[80];
    char strp2[80];
    char format_string[80]="%d,%d";
    char format_string_p[80] = "%d";
    int pid1, pid2;

    fd3 = open(myfifo3,O_RDONLY); 
    read(fd3, str3, 80); 
    close(fd3);

    fdp = open(myfifop,O_WRONLY);
    sprintf(strp, format_string_p, (int)getpid());
    write(fdp,strp, strlen(strp)+1);
    close(fdp);

    fdp2 = open(myfifop2,O_WRONLY);
    sprintf(strp2, format_string_p, (int)getpid());
    write(fdp2,strp2, strlen(strp2)+1);
    close(fdp2);

    sscanf(str3, format_string, &pid1, &pid2);
        
    printf("Inside watch dog\n");
    fflush(stdout);
    printf("The two PIDs are: (PID_1 = %d)= and (PID_2 = %d)\n", pid1,  pid2); 
    fflush(stdout);
    
    while(1)
    {
        if(difftime(time(NULL),t) > 15)
        {
            kill(pid1,SIGKILL);
            printf("Process with (PID = %d) killed\n",pid1);
            fflush(stdout);
            kill(pid2,SIGKILL);
            printf("Process with (PID = %d) killed\n",pid2);
            fflush(stdout);
            sleep(2);
            printf("I'll terminate watch dog process, (PID = %d), in 5 secs\n",(int)getpid());
            fflush(stdout);
            sleep(5);
            exit(0);
        }
    }
}