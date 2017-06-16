#!/bin/bash

# driver.sh - The simplest autograder we could think of. It checks
#   that students can write a C program that compiles, and then
#   executes with an exit status of zero.
#   Usage: ./driver.sh

# Compile the code
test_case=("aarg" "ab" "arr" "cmt" "expr" "func" "funny" "id" "if" "ifelse" "logic" "nnim" "qsort" "recur" "revstr" "scope" "sort" "while" "fac")

(tar xvf handin.tar -C handin)
(cd tigger; make)
(cd handin; make clean; make)

#echo "Compiling hello.c"
#(make clean; make)
status=$?
if [ ${status} -ne 0 ]; then
    echo "Failure: Unable to compile hello.c (return status = ${status})"
    exit
fi
correct=()
for x in ${test_case[@]}
do
    ./handin/parser ./src/$x.c ./src/$x.e
    ./handin/eeyore_parser ./src/$x.e ./src/$x.t
    case_count=`ls ./test/$x/ -l | grep in |grep "^-"|wc -l`
    case_correct=0
    for y in `ls test/$x/ | grep in`
    do
	y=${y/\.in/}
	#echo $x, $y
        ./tigger/tigger ./src/$x.t < ./test/$x/$y.in > ./test/$x/$y.myout
	(diff -Z ./test/$x/$y.out ./test/$x/$y.myout)
	status=$?
	if [ ${status} -ne 0 ]; then
	    echo $x, $y, "fail."
	else
           let case_correct=case_correct+1
	fi
	rm ./test/$x/$y.myout
    done
    tmp=`echo "scale=2;$case_correct/$case_count*10.0" | bc`
    correct+=($tmp)
    echo $x " test done. Score: " $tmp
done
length=${#test_case[*]}
echo -n "{ \"scores\": {"
for ((i=0; i < ${length}-1; i++));
do
    echo -n "\"${test_case[i]}\": ${correct[i]} ,"
done
echo -n "\"${test_case[${length}-1]}\": ${correct[${lengt}h-1]} }}"
# Run the code
#echo "Running ./hello"
#./hello
#status=$?
#if [ ${status} -eq 0 ]; then
#    echo "Success: ./hello runs with an exit status of 0"
#    echo "{\"scores\": {\"Correctness\": 100}}"
#else
#    echo "Failure: ./hello fails or returns nonzero exit status of ${status}"
#    echo "{\"scores\": {\"Correctness\": 0}}"
#fi

#exit
#
