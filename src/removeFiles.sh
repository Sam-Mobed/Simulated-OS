#!/bin/bash

# Get a list of all the files in the current directory
files=$(ls ../testcases/assignment2)

# Loop through each file and remove it (if it's a file)
for file in $files
do
    if [ -f $file ]; then
        rm $file
    fi
done
