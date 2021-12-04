# unzip src.zip file into the path inserted
unzip -q -d $1/ src.zip

# create 2 folder to organise the files
mkdir $1/exc
mkdir $1/log

# compile .c files, from src folder, and put them into exc folder created
gcc $1/src/master/master.c -o $1/exc/master
gcc $1/src/commandconsole/commandconsole.c -o $1/exc/commandconsole
gcc $1/src/inspectionconsole/inspectionconsole.c -o $1/exc/inspectionconsole
gcc $1/src/motorX/motorX.c -o $1/exc/motorX
gcc $1/src/motorZ/motorZ.c -o $1/exc/motorZ
gcc $1/src/watchdog/watchdog.c -o $1/exc/watchdog

# printing on the shell
echo installation succeded