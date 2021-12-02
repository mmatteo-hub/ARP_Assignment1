#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>
#include <time.h> 

#define BUFFSIZE 80

#define KNRM  "\x1B[0m"
#define KYEL  "\x1B[33m"

#define USEC 1000

// defining pids for 5 processes
int pid1, pid2, pid3, pid4, pid5;

// defininf file pointer and a time variable to read the current date
FILE *f;
time_t clk;

// function to start the execution of the processes, having a parameters list
int spawn(const char * program, char ** arg_list) 
{
  pid_t child_pid = fork();
  if (child_pid != 0)
    return child_pid;
  else 
  {
    execvp (program, arg_list);
    perror("exec failed");
    return 1;
  }
}

// function to kill al processes and respective write on the log file all actions computed
void kill_processes()
{
  kill(pid1,SIGKILL);
  fseek(f,0,SEEK_END);
  clk = time(NULL);
  fprintf(f,"Process (PID = %d) killed at : %s", pid1, ctime(&clk));
  fflush(f);

  kill(pid2,SIGKILL);
  fseek(f,0,SEEK_END);
  clk = time(NULL);
  fprintf(f,"Process (PID = %d) killed at : %s", pid2, ctime(&clk));
  fflush(f);

  kill(pid3,SIGKILL);
  fseek(f,0,SEEK_END);
  clk = time(NULL);
  fprintf(f,"Process (PID = %d) killed at : %s", pid3, ctime(&clk));
  fflush(f);

  kill(pid4,SIGKILL);
  fseek(f,0,SEEK_END);
  clk = time(NULL);
  fprintf(f,"Process (PID = %d) killed at : %s", pid4, ctime(&clk));
  fflush(f);

  kill(pid5,SIGKILL);
  fseek(f,0,SEEK_END);
  clk = time(NULL);
  fprintf(f,"Process (PID = %d) killed at : %s", pid5, ctime(&clk));
  fflush(f);
}

// main
int main(int argc, char *argv[])
{
  // opening the log file in writing mode to create if it does not exist
  f = fopen("./../log/logfile.txt","w");

  // defining all the paramters list for the processes
  char *arg_list_1[] = {"/usr/bin/konsole", "--hold", "-e", "./../exc/commandconsole", (char*)NULL};
  char *arg_list_2[] = {"/usr/bin/konsole", "--hold", "-e", "./../exc/inspectionconsole", (char*)NULL};
  char *arg_list_3[] = { "./../exc/motorX", (char*)NULL};
  char *arg_list_4[] = { "./../exc/motorZ", (char*)NULL};
  char *arg_list_5[] = { "./../exc/watchdog", (char*)NULL};
 
  // spawning the processes in a specific order to have the correctness for the execution

  pid3 = spawn("./../exc/motorX", arg_list_3);
  fseek(f,0,SEEK_END);
  clk = time(NULL);
  fprintf(f,"konsole (PID = %d) created at : %s", pid3, ctime(&clk));
  fflush(f);

  usleep(USEC);

  pid4 = spawn("./../exc/motorZ", arg_list_4);
  fseek(f,0,SEEK_END);
  clk = time(NULL);
  fprintf(f,"konsole (PID = %d) created at : %s", pid4, ctime(&clk));
  fflush(f);

  usleep(USEC);

  pid5 = spawn("./../exc/watchdog", arg_list_5);
  fseek(f,0,SEEK_END);
  clk = time(NULL);
  fprintf(f,"konsole (PID = %d) created at : %s", pid5, ctime(&clk));
  fflush(f);

  usleep(USEC);

  pid1 = spawn("/usr/bin/konsole", arg_list_1);
  fseek(f,0,SEEK_END);
  clk = time(NULL);
  fprintf(f,"konsole (PID = %d) created at : %s", pid1, ctime(&clk));
  fflush(f);

  usleep(USEC);

  pid2 = spawn("/usr/bin/konsole", arg_list_2);
  fseek(f,0,SEEK_END);
  clk = time(NULL);
  fprintf(f,"konsole (PID = %d) created at : %s", pid2, ctime(&clk));
  fflush(f);

  // variable to read the shell commmand to terminate the execution
  char inputStr[80];
  
  // loop to wait for the input charatcter from the shell
  do
  {
    printf ("%sPress T to TERMINATE the program\n",KYEL);
    scanf("%s",inputStr);
  }
  while(strcmp(inputStr, "T") != 0);

  // calling the kill function
  kill_processes();

  // writing on the log file
  fseek(f,0,SEEK_END);
  clk = time(NULL);
  fprintf(f,"Master process (PID = %d) finished at : %s", (int)getpid(), ctime(&clk));
  fflush(f);
  fclose(f);
  
  return 0;
}