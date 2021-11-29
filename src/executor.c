#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>

#define BUFFSIZE 80

#define KNRM  "\x1B[0m"
#define KYEL  "\x1B[33m"

// defining pids for 5 processes
int pid1, pid2, pid3, pid4, pid5;

// function to start the execution of the processes
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

void kill_processes()
{
  kill(pid1,SIGKILL);
  kill(pid2,SIGKILL);
  kill(pid3,SIGKILL);
  kill(pid4,SIGKILL);
  kill(pid5,SIGKILL);
}

int main() 
{
  int fd_watchdog;
  int fd_inspection;

  pid_t child_pid;

  // pipe communicating with the command console
  char * myfifo_command = "/tmp/fifo_command";

  // pipes communicating with motors
  char * myfifo_motorX = "/tmp/fifo_motX";
  char * myfifo_motorZ = "/tmp/fifo_motZ";

  mkfifo(myfifo_command,0666);
  mkfifo(myfifo_motorX,0666);
  mkfifo(myfifo_motorZ,0666);

  char input_string[BUFFSIZE];
  char outup_string[80];
  char motor_pids_string[80];
  char out_str[80];
  char format_string[80] = "%d,%d";
  
  // execl with the params for the konsoles
  child_pid = fork();
  if (child_pid == -1) perror("fork failed");
  if (child_pid != 0) pid1 = child_pid;
  else
  {
    if ( execl ("/usr/bin/konsole", "/usr/bin/konsole", "--hold",  "-e", "./exe/commandconsole", (char*) NULL) == -1) perror("exec failed");
	exit(1);
  }
  printf("1st konsole (PID = %d)\n", pid1);
  fflush(stdout);
  
  child_pid = fork();
  if (child_pid == -1) perror("fork failed");
  if (child_pid != 0) pid2 = child_pid;
  else
  {
    if ( execl ("/usr/bin/konsole", "/usr/bin/konsole", "--hold",  "-e", "./exe/inspectionconsole", (char*) NULL) == -1) perror("exec failed");
  exit(1);
  }
  printf("2nd konsole (PID = %d)\n", pid2);
  fflush(stdout);

  child_pid = fork();
  if (child_pid == -1) perror("fork failed");
  if (child_pid != 0) pid3 = child_pid;
  else
  {
    if ( execl ("./exe/motorX", "./exe/motorX", (char*) NULL) == -1) perror("exec failed");
  exit(1);
  }
  printf("3rd konsole (PID = %d)\n", pid3);
  fflush(stdout);

  child_pid = fork();
  if (child_pid == -1) perror("fork failed");
  if (child_pid != 0) pid4 = child_pid;
  else
  {
    if ( execl ("./exe/motorZ", "./exe/motorZ", (char*) NULL) == -1) perror("exec failed");
  exit(1);
  }
  printf("4th konsole (PID = %d)\n", pid4);
  fflush(stdout);

  child_pid = fork();
  if (child_pid == -1) perror("fork failed");
  if (child_pid != 0) pid5 = child_pid;
  else
  {
    if ( execl ("./exe/watchdog", "./exe/watchdog", (char*) NULL) == -1) perror("exec failed");
  exit(1);
  }
  printf("5th konsole (PID = %d)\n", pid5);
  fflush(stdout);

  char inputStr[80];
  
  do
  {
    printf ("\n%sPress T to TERMINATE the program\n",KYEL);
    scanf("%s",inputStr);
  }
  while(strcmp(inputStr, "T") != 0);

  kill_processes();
  return 0;
}