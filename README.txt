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

To have infos about how to prepre and run the program type "$ ./help.sh" into to shell and follow the instructions.