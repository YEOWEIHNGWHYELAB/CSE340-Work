#!/bin/bash

read -p "Do you want to compile? Y/N " -n 1 -r;
if [[ $REPLY =~ ^[Yy]$ ]]; then
	echo;
	echo;
	echo "Compilation Initialized!";

	if g++ -std=c++11 -Wall parser.cc syntaxerror.cc lexer.cc mylexer.cc inputbuf.cc; then
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

TEST_FOLDER="./provided_tests"

SPECIAL_TEST_FOLDERS=(
	"Syntax_Error_General" "SNYTAX ERORR"
	"Syntax_Error_Expr" "SYNTAX ERROR IN"
)

if [ ! -d "${TEST_FOLDER}" ]; then
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

echo "========================================================"
echo "Start of Provided Test"

let count=0
let all=0

mkdir -p ./output

for test_file in $(find "${TEST_FOLDER}" -type f -name "*.txt" | sort); do
    all=$((all+1))
    name=`basename ${test_file} .txt`
    expected_file=${test_file}.expected
    output_file=./output/${name}.output
    diff_file=./output/${name}.diff
    ./a.out < ${test_file} > ${output_file}


    folder_name="$(cut -d'/' -f3 <<<"${test_file}")"

	for special_index in seq 0 2 ${#SPECIAL_TEST_FOLDERS[@]}; do
		special_name=${SPECIAL_TEST_FOLDERS[${special_index}]}
		special_test=${SPECIAL_TEST_FOLDERS[$((special_index+1))]}

		if [ "${folder_name}" = "${special_name}" ]; then
			diff -Bw ${expected_file} ${output_file} > ${diff_file}

			out=$(<${output_file})
			exp=$(<${expected_file})

			if [[ ${exp} == *${special_test}* ]]; then
				if [[ -s ${diff_file} ]]; then
					echo "${name}: Output does not match expected:"
					echo "--------------------------------------------------------"
					cat ${diff_file}
				else
					count=$((count+1))
					echo "${name}: OK"
				fi
			else
				if [[ ${out} == *${special_test}* ]]; then
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

			continue 2
		fi
	done

	# General folder handling
	
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
    echo "========================================================"
    rm -f ${output_file}
    rm -f ${diff_file}
done

echo
echo "Passed $count tests out of $all"
echo

rmdir ./output

read -p "Do you want use your own test cases? Y/N " -n 1 -r;
if [[ $REPLY =~ ^[Yy]$ ]]; then
	echo "========================================================"
	echo "Start of My Test"

	let count=0
	let all=0

	mkdir -p ./output

	for test_file in $(find ./my_tests -type f -name "*.txt" | sort); do
		all=$((all+1))
		name=`basename ${test_file} .txt`
		expected_file=${test_file}.expected
		output_file=./output/${name}.output
		diff_file=./output/${name}.diff
		./a.out < ${test_file} > ${output_file}
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
		echo "========================================================"
		rm -f ${output_file}
		rm -f ${diff_file}
	done

	echo
	echo "Passed $count of my tests out of $all"
	echo

	rmdir ./output

else
	echo
	echo

fi
