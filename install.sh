unzip -q -d $1/ src.zip

mkdir $1/exc
mkdir $1/log
gcc $1/src/executor/executor.c -o $1/exc/executor
gcc $1/src/commandconsole/commandconsole.c -o $1/exc/commandconsole
gcc $1/src/inspectionconsole/inspectionconsole.c -o $1/exc/inspectionconsole
gcc $1/src/motorX/motorX.c -o $1/exc/motorX
gcc $1/src/motorZ/motorZ.c -o $1/exc/motorZ
gcc $1/src/watchdog/watchdog.c -o $1/exc/watchdog

echo installation succeded