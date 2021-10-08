# Vaccine Monitor App - System Programming Project
![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)
![Release: v1.0.0](https://img.shields.io/github/v/release/nassosanagn/Vaccine-Monitor-App?include_prereleases)
<br/>
This is my implementation of a Vaccine Monitor app created for the System Programming K24 course at the University of Athens.

## Bash script: testFile.sh

This bash script creates a test input file, that will be located in the "info" directory, called "inputFile.txt".

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
    bash ./create_infiles.sh inputFile input_dir numFilesPerDirectory

<!-- ![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white) -->
