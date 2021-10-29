# Vaccine Monitor App - System Programming Project
![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)
![Release: v1.0.0](https://img.shields.io/github/v/release/nassosanagn/Vaccine-Monitor-App?include_prereleases)
<br/>
This is my implementation of a Vaccine Monitor app created for the System Programming K24 course at the University of Athens.

## Implementation & Use

This project is based of the concept of one client (**travelMonitorClient**) and multiple servers (**monitorServers**).

The parent process (**travelMonitorCliend**) receives requests from citizens, who want to travel to other countries. The app then checks if the citizen has received the appropriate vaccination needed to enter the country and if the answer is yes, then it proceeds to accept the citizen's request.

This project is implemented using UNIX **sockets** for the communication between the parent process with the child processes.

Also, it uses POSIX **threads** to read the data files from the input directory.

## Bash script: testFile.sh

This bash script creates a test input file, located in the "info" directory, called "inputFile.txt".

The input file will contain a number of citizen vaccination records about different viruses, in the following order:
`citizenId firstName lastName country age virusName isVaccinated dateVaccinated`

For example, <br/>
    `125 Jason Dupont USA 76 H1N1 YES 30-10-2020`<br/>
    `889 John Papadopoulos Greece 52 Η1Ν1 ΝΟ`

This "inputFile.txt" will be used later as an input to the second bash script, called "create_infiles.sh".

### Create the input file:
    bash ./script_1/testFile.sh info/virusesFile.txt info/countriesFile.txt numLines duplicatesOn

Where **numLines** equals with the number of lines (records) that will be in the "inputFile.txt" and **duplicatesOn** = 1 if duplicate citizenIDs are allowed, or 0 if citizenIDs are unique.

The **virusesFile.txt**, located in the info directory, contains a list of 20 different viruses that can be modified as you wish. Same way with the **countriesFile.txt**.

## Bash script: create_infiles.sh

This bash script reads the inputFile.txt and creates a directory that contains subdirectories, one for each country name that exists in the inputFile. In each subdirectory, there are  **numFilesPerDirectory** files with names **country-n.txt**, where n is a positive integer. 

### Create the input directory:
    bash ./script_2/create_infiles.sh info/inputFile.txt input_dir numFilesPerDirectory

<!-- Vaccine Monitor Section -->

## Vaccine Monitor App:

### Compile the project:
    make
### Delete the objective files:
    make clean
### Run the program:
    ./travelMonitorClient –m numMonitors -b socketBufferSize -c cyclicBufferSize -s sizeOfBloom -i input_dir -t numThreads


|                   |                                                              |
| ----------------- | ------------------------------------------------------------ |
| numMonitors       | The number of monitorServer processes that will be created   |
| socketBufferSize  | The size of the buffer (in bytes), used to read from the sockets |
| cyclicBufferSize  | The size of monitorServer's cyclic buffer (in entries)       |
| sizeOfBloom       | The size of the bloom filters (in bytes)                     |
| input_dir         | The directory created from the create_infiles.sh bash script |
| numThreads        | The number of threads in each monitorServer                  |

<!-- ![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white) -->
