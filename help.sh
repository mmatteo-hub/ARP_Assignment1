# adding path and file .txt to the help.sh file to show in the shell
cd -- "$(find . -iname src -type d)"

less commandconsole/commandconsole.txt
less inspectionconsole/inspectionconsole.txt
less master/master.txt
less motorX/motorX.txt
less motorZ/motorZ.txt
less watchdog/watchdog.txt