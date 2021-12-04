################ ASSIGNMENT ################
The assignment is the simulation of a hoist, realized in C language.
The hoist has two degrees of freedom, called X axis (moving right and left) and Z axis (moving up and down).
The origin of the hoist is the up-left point and on each degree of freedom it has a maximum space of 10 m.

Graphically it can be described as follows:

(0,0)   
    #------------------------
    |
    |
    |
    |
    |
    |
    |
    |

The program contains multiple files:
- command console;
- inspection console;
- motor X;
- motor Z;
- watchdog
- executor
and their behaviour will be described in the sections belows.
The program is provided of a log file, created every time the program is executed, which keeps trace of the action done during the execution of the program.

The communication of the program is mainly based of pipes with also the presence of singnals.

The master is the main process which runs all the other processes and if necessary ends them by just shell command.

######## PROGRAM PREPARATION ########
1 - type on a shell '$ ./install.sh <path name>' and press enter. The <path name> has to be the name of a folder (the folder will be created inside the actual folder, so do not give a gloabl path since the next step will not be done succesfully);

1.1 - check that, once the install.sh has completed, a message will be shown on the shell;

2 - type '$ ./help.sh' and read the brief description below for each process to understand the general purpose and structure of the program;

Now everything is ready to start the process ..

3 - type  '$ ./run.sh' and, if everything has been done correctly, the program will start automatically.

To be sure the program is started correctly you should see:
- 2 consoles, named inspection and command console, waiting for some inputs;
- the main shell waiting for command too.