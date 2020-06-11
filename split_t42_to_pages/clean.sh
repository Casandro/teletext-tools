#/bin/bash

for x in *.c
do
	bn=`basename $x .c`
	rm  $bn
done
