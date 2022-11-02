#!/bin/bash

# Compilation Check
read -p "Do you want to compile? Y/N " -n 1 -r;
if [[ $REPLY =~ ^[Yy]$ ]]; then
	echo;
	echo;
	echo "Compilation Initialized!";

	if g++ --std=c++11 -g -Wall project2.cc tasks.cc error.cc lexer.cc inputbuf.cc execute.cc; then
		echo "----------------------";
		echo "Successfully Compiled!";
	else
		echo "----------------------";
		echo "Compilation Failed!";
		
		read -p "Do you want to continue test with previous a.out? Y/N " -n 1 -r;
		if [[ ! $REPLY =~ ^[Yy]$ ]]; then
			echo; # move to a new line
			echo;
			echo "TEST TERMINATED!";
			exit 1;
		fi;

		echo;
		echo;
		echo "Resuming Test with old a.out...";
	fi;
else
	echo;
	echo;
fi;

# Check if task number is provided
usage()
{
    echo
    echo "Usage: $0 n"
    echo
    echo "Where n is the desired task number in range [1..3]"
    echo
    exit 1
}

if [ "$#" -lt "1" ]; then
    echo "Error: task number not specified!"
    usage
fi

if [[ ! "$1" =~ ^[0-9]+$ ]]; then
    echo "Error: argument is not a number"
    usage
fi

if [ "$1" -lt "1" -o "$1" -gt "3" ]; then
    echo "Error: argument must be a number in range [1..3]"
    usage
fi

# Test cases
if [ ! -d "./provided_tests" ]; then
    echo "Error: tests directory not found!"
    exit 1
fi

if [ ! -e "./a.out" ]; then
    echo "Error: a.out not found!"
    exit 1
fi

if [ ! -x "./a.out" ]; then
    echo "Error: a.out not executable!"
    exit 1
fi

let task_num=$1
let count=0
let all=0

mkdir -p ./output

for test_file in $(find ./provided_tests -type f -name "*.txt" | sort); do
    all=$((all+1))
    name=`basename ${test_file} .txt`
    expected_file=${test_file}.expected${task_num}
    output_file=./output/${name}.output
    diff_file=./output/${name}.diff
    ./a.out  ${task_num} < ${test_file} > ${output_file}


    folder_name="$(cut -d'/' -f3 <<<"${test_file}")"
	
    if [ ${folder_name} = "Syntax_Error" ]; then
	    	diff -Bw ${expected_file} ${output_file} > ${diff_file}
	
		out=$(<${output_file})
		exp=$(<${expected_file})

		if [[ ${exp} == "SYNTAX ERROR" ]]; then
			if [[ ${out} == "SYNTAX ERROR" ]]; then
				count=$((count+1))
				echo "${name}: OK"
			else
				echo "${name}: Output does not match expected:"
				echo "--------------------------------------------------------"
				cat ${diff_file}
			fi
		else
			if [[ ${out} == "SYNTAX ERROR" ]]; then
				echo "${name}: Output does not match expected:"
				echo "--------------------------------------------------------"
				cat ${diff_file}
			else
				count=$((count+1))
				echo "${name}: OK"
			fi
		fi
            
    else


	    diff -Bw ${expected_file} ${output_file} > ${diff_file}
		
	    echo
	    if [ -s ${diff_file} ]; then
		echo "${name}: Output does not match expected:"
		echo "--------------------------------------------------------"
		cat ${diff_file}
	    else
		count=$((count+1))
		echo "${name}: OK"
	    fi
    fi
    echo "========================================================"
    rm -f ${output_file}
    rm -f ${diff_file}
done

echo
echo "Passed $count tests out of $all"
echo

rmdir ./output
