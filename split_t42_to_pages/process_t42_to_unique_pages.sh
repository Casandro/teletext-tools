#!/bin/bash


INFILE="$1"
OUTDIR="$2"

#Split File into Pages

./split_t42_to_pages "$OUTDIR" < $1


#Delete pages that are duplicates of eachother (ignoring the header line)

for x in `ls $OUTDIR/*.t42 | cut -d "-" -f 1-2 | sort | uniq`
do
	echo $x
	./delete_double $x*
done
