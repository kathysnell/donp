#!/bin/bash
set -e

# A script to run the Descriptive Object Notated Protocol (DONP) simulation.
# Usage: ./donp.sh

# Text introduction output
echo ""
echo "Welcome to the Descriptive Object Notated Protocol (DONP) Simulation!"
echo ""
echo "Where a protocol simulation is achieved by a protocol described in a JSON file."
echo ""

# Let's have some fun!  
echo "For fun we will have a race, while demonstrating the DONP simulation in different programming languages."
echo ""
echo "The DONP simulation will loop through the configured message simulation 10 times for each language."
echo ""
echo "Simulated messages will be displayed in hexadecimal format."
echo ""
echo "Elapsed time for each language will be captured and displayed at the end of each run."
echo ""
echo "The languages to be tested are:"
echo " - Python"
echo " - Go"
echo ""
echo "At the end we will see which language was the fastest!"
echo ""

# structure indicating directories (programming language specific)
lang0=("Python" "python3" "donp.py")
lang1=("Go" "" "./donp")

languages=("lang0" "lang1")

# Loop through each language and execute the corresponding script
for lang in "${languages[@]}"; do
    lang_dir_var="${lang}[0]"
    lang_cmd_var="${lang}[1]"
    lang_file_var="${lang}[2]"
    lang_elapsed_var="${lang}[3]"
    
    lang_dir="${!lang_dir_var}"
    lang_cmd="${!lang_cmd_var}"
    lang_file="${!lang_file_var}"
    lang_elapsed="${!lang_elapsed_var}"

    SECONDS=0  # Reset the timer    
    printf "Running DONP simulation with ${lang_dir} in 10 seconds..."
    while [ $SECONDS -lt 10 ]; do
        sleep 1
        if [ $SECONDS -gt 6 ] && [ $SECONDS -lt 10 ]; then
            printf "\n$((10 - SECONDS)) ..."
        else 
            printf "."     
        fi
    done
    echo ""
    
    # Change to the language-specific directory
    cd "${lang_dir}" || { echo "Directory ${lang_dir} not found!"; exit 1; }
    
    
    # Execute while capturing the number of seconds from the last line of output
    $(${lang_cmd} "${lang_file}" > output.log 2>&1)
    # Output contents of output log
    echo "Output log for ${lang_dir}:"
    cat output.log
    echo ""
    last_line=$(tail -n 1 output.log)
    #echo "Last line of output log: ${last_line}"    
    seconds=$(echo "${last_line}" | sed -nE 's/.*Elapsed time=([0-9]+(\.[0-9]+)?) seconds.*/\1/p')
    echo "Time taken for ${lang_dir}: ${seconds} seconds"
    echo ""

    # Store the elapsed time for this language
    eval "${lang_elapsed_var}='${seconds}'"
    
    # Return to the original directory
    cd - > /dev/null || exit 1
done

# Determine the fastest language
fastest_lang=""
fastest_time=""
for lang in "${languages[@]}"; do
    lang_elapsed_var="${lang}[3]"
    lang_elapsed="${!lang_elapsed_var}"
    lang_elapsed_dir_var="${lang}[0]"
    lang_elapsed_dir="${!lang_elapsed_dir_var}"
    
    if [ -z "$fastest_time" ] || (( $(echo "$lang_elapsed < $fastest_time" | bc -l) )); then
        fastest_time="$lang_elapsed"
        fastest_lang="${lang_elapsed_dir}"
    fi
done

echo -e "The fastest language was \033[1m${fastest_lang}\033[0m with a time of ${fastest_time} seconds!"

exit 0