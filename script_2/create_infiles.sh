#!/bin/bash

if [ ! -e $1 ]               # Check if file 1 (inputFile) exists and if it's a regular file
    then if [ ! -f $1 ] 
        then "ERROR: this is not a regular file"
        exit
    fi
fi

if [ $# -ne 3 ];                            # We need to provide 3 arguments
then
    echo "ERROR: we need exactly 3 arguments"
    exit
fi

# Get the 3 arguments
inputFile=$1
inputDir=$2
numFilesPerDirectory=$3

if [[ -d $inputDir ]]                             # Check if directory already exists 
then
    echo "ERROR: $inputDir already exists"
    exit
fi

mkdir ./$inputDir             # Create the directory 

declare -a countries
declare -a index

n=1

while IFS=' ' read -r id firstName lastName country age virusName isVaccinated date             # reading each line
do
    flag=1
    for (( j=0; j<${#countries[@]}; j++ ))         # For every country in countriesArray
    do
        if [[ "$country" = "${countries[j]}" ]];         # If country is already in countriesArray
        then
            flag=0
            echo "$id $firstName $lastName $country $age $virusName $isVaccinated $date " >> ./$inputDir/$country/"$country-${index[j]}".txt
            index[j]=$((index[j]+1))        # add 1 to index[j]

            if [[ ${index[j]} -eq $((numFilesPerDirectory+1)) ]]
            then
                index[j]=1
            fi
            break
        fi
    done

    if [[ $flag -eq 1 ]]                        # If country isn't in countriesArray => add it
    then
        countries+=("$country")                         # Add a new country 
        mkdir ./$inputDir/$country                      # Make a new directory

        count=1
        for (( j=0; j<$numFilesPerDirectory; j++ ))        
        do
            echo -n >> ./$inputDir/$country/"$country-$count".txt  
            count=$((count+1))
        done

        index+=(2)
        echo "$id $firstName $lastName $country $age $virusName $isVaccinated $date " >> ./$inputDir/$country/"$country-1".txt  
    fi
n=$((n+1))
done < $inputFile