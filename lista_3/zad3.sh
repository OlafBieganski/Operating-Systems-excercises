#!/bin/bash
while :
do
	sleep 2
	date
	trap 'echo "Signal blocked" ' 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
done
