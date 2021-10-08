#!/bin/bash

if [ ! -e $1 ]               # Check if file 2 (virusesFile) exists and if it's a regular file
    then if [ ! -f $1 ] 
        then "ERROR: this is not a regular file "
        exit 
    fi
fi

if [ ! -e $2 ]              # Check if file 2 (countrieFile) exists and if it's a regular file
    then if [ ! -f $2 ] 
        then "ERROR: this is not a regular file "
        exit 
    fi
fi

FILE=./info/inputFile.txt
if [[ -f "$FILE" ]]                           # Check if "inputFile.txt" already exists
then
    echo "ERROR: inputFile.txt already exists"
    exit
fi

# Get the 4 arguments
virusesFile=$1
countriesFile=$2
numLines=$3
duplicatesAllowed=$4

if [ $# -ne 4 ];                            # We need to provide 4 arguments
then
    echo "ERROR: WE NEED 4 ARGUMENTS"
    exit
fi

if [ $duplicatesAllowed -ne 1 ] && [ $duplicatesAllowed -ne 0 ];            # The 4th argument (duplicatesAllowed) must be 1 or 0 */
then
    echo "4th ARGUMENT MUST BE 1 OR 0"
    exit
fi

declare -a idsArray
declare -a viruses
declare -a countries

while IFS= read -r line;           # Read viruses file
do
    viruses+=("$line")
done < $virusesFile 

while IFS= read -r line;           # Read countries file
do
    countries+=("$line")
done < $countriesFile

virusesSize=${#viruses[@]}              # Get viruses array size
countriesSize=${#countries[@]}          # Get countries array size

for (( i=0; i<$numLines; i++ ))         # Create numLines number of records
do
    citizenId="$(( 1+$RANDOM % 9999 ))"          # create random Id

    p1="$((1 + $RANDOM % 100))"       # propability 1

    # if duplicate is allowed and prob1 > 50 => get an existing id from idsArray
    if [ $duplicatesAllowed -eq 1 ] && [ $p1 -gt 50 ] && [ $i -ne 0 ];                  # Except of the 1st iteration
    then
        randomIdIndex="$(($RANDOM % $i))"
        citizenId=${idsArray[randomIdIndex]}           # a random Id from idsArray

    else                                # Create a new id that doesn't exist in idsArray
        flag=0                       # flag to check if id is already in idsArray
        while [ $flag -eq 0 ];
        do
            flag=1
            for (( j=0; j<${#idsArray[@]}; j++ ))         # For every citizenId in idsArray
            do
                if [[ $citizenId -eq ${idsArray[j]} ]];         # If id is already in idsArray
                then
                    citizenId="$(( 1 +$RANDOM % 9999 ))"          # create a new random Id
                    flag=0
                    break
                fi
            done
        done
    fi
        
    idsArray+=("$citizenId")                # Add the new citizenId in idsArray

    randomVirusIndex="$(($RANDOM % $virusesSize))" 
    randomCountryIndex="$(($RANDOM % $countriesSize))" 

    virusName=${viruses[randomVirusIndex]}                      # Get a random virus from viruses array
    countryName=${countries[randomCountryIndex]}                # Get a random country from countries array

    age="$(( 1+$RANDOM % 120 ))"
    firstName=$(head -3 /dev/urandom | tr -cd '[:alpha:]' | cut -c -$((3 + $RANDOM % 12)))
    lastName=$(head -3 /dev/urandom | tr -cd '[:alpha:]' | cut -c -$((3 + $RANDOM % 12)))

    p2="$((1 + $RANDOM % 100))"       # propability 2 of person being vaccinated or not (50 - 50 chance)

    if [ $p2 -gt 50 ];                  # If prob > 50 => person is vaccinated => create date
    then
        isVaccinated="YES"
        day="$(( 1 + $RANDOM % 30 ))" 
        month="$(( 1 + $RANDOM % 12 ))"
        year="$(( 2005 + $RANDOM % 17 ))"       # from 2005 to 2021 
        echo "$citizenId $firstName $lastName $countryName $age $virusName $isVaccinated $day-$month-$year" >>./info/inputFile.txt   
    else 
        isVaccinated="NO"
        echo "$citizenId $firstName $lastName $countryName $age $virusName $isVaccinated" >>./info/inputFile.txt
    fi
done