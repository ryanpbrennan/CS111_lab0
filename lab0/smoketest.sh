#!/bin/bash

# Ryan Brennan
# rpbrennan@g.ucla.edu
# 605-347-597

./lab0 --segfault > /dev/null 2>&1
if [[ $? -eq 139 ]]
then
	echo "OK"
else
	echo "NO"
fi


./lab0 --segfault --catch > /dev/null
if [[ $? -eq 4 ]]
then
	echo "OK"
else
	echo "NO"
fi


touch "t1.txt"
echo "this is a test" > "t1.txt"
./lab0 --input "t1.txt" --output "t2.txt" 2> /dev/null
if [[ $? -eq 0 ]]
then
	echo "OK"
else
	echo "NO"
fi


./lab0 --input "fakefile" --output "t2.txt" 2> /dev/null
if [[ $? -eq 2 ]]
then
	echo "OK"
else
	echo "NO"
fi


diff "t1.txt" "t2.txt" 2> /dev/null
if [[ $? -eq 0 ]]
then
	echo "OK"
else
	echo "NO"
fi


chmod -w "t2.txt"
./lab0 --input "t1.txt" --output "t2.txt" 2> /dev/null
if [[ $? -eq 3 ]]
then
	echo "OK"
else
	echo "NO"
fi

rm -f "t1.txt" "t2.txt"
