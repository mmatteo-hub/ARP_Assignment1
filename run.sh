# look for the presence of the exc folder, containig all executables files and move into it
cd -- "$(find . -iname exc -type d)"

# run the master process
./master