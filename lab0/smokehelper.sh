#!/bin/bash

result=$(./smoketest.sh 2> /dev/null | grep "OK" | wc -l)
if [[ $result -eq 6 ]]
then
	echo "smoke test completed successfully."
else
	echo "smoke test failed. errors in program"
fi
