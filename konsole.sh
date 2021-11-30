rm -r exe
rm -r log
mkdir exe
mkdir log
gcc src/executor.c -o exe/executor
gcc src/commandconsole.c -o exe/commandconsole
gcc src/inspectionconsole.c -o exe/inspectionconsole
gcc src/motorX.c -o exe/motorX
gcc src/motorZ.c -o exe/motorZ
gcc src/watchdog.c -o exe/watchdog
./exe/executor