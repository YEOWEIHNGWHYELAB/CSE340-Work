read -p "Do you want to compile? Y/N " -n 1 -r;
if [[ $REPLY =~ ^[Yy]$ ]]; then
	echo;
	echo;
	echo "Compilation Initialized!";

	if g++ --std=c++11 -g -Wall execute.cc inputbuf.cc lexer.cc parser.cc addresser.cc error.cc instruction_debug.cc; then
		echo "----------------------";
		echo "Successfully Compiled!";
	else
		echo "----------------------";
		echo "Compilation Failed!";
	fi;
else
	echo;
	echo;
fi;

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

let count=0
let all=0

mkdir -p ./output

for test_file in $(find ./provided_tests -type f -name "*.txt" | sort); do
    all=$((all+1))
    name=`basename ${test_file} .txt`
    expected_file=${test_file}.expected
    output_file=./output/${name}.output
    diff_file=./output/${name}.diff
    ./a.out < ${test_file} > ${output_file}
    diff -Bw ${expected_file} ${output_file} > ${diff_file}
    echo
    if [ -s ${diff_file} ]; then
        echo "CASE ${all} (${name}): Output does not match expected:"
        echo "--------------------------------------------------------"
        cat ${diff_file}
    else
        count=$((count+1))
        echo "CASE ${all} (${name}): OK"
    fi
    echo "========================================================"
    echo
    rm -f ${output_file}
    rm -f ${diff_file}
done

echo
echo "Passed $count tests out of $all"
echo

rmdir ./output

if [ ! -d "./my_tests" ]; then
    echo "Error: tests directory not found!"
    exit 1
fi

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
        echo "CASE ${all} (${name}): Output does not match expected:"
        echo "--------------------------------------------------------"
        cat ${diff_file}
    else
        count=$((count+1))
        echo "CASE ${all} (${name}): OK"
    fi
    echo "========================================================"
    echo
    rm -f ${output_file}
    rm -f ${diff_file}
done

echo
echo "Passed $count tests out of $all"
echo

rmdir ./output

