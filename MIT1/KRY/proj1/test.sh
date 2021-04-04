#!/usr/bin/bash

# If no argument is given, then the testing files are in current directory
#	otherwise they are in the directory passed as 1st argument
dir=`pwd`
if [ ! -z "$1" ]; then
	dir=`pwd`"/$1"
fi

for file in "$dir"/*.ctxt
do
	printf '%s\n' "$file"
	./kry < "$file"
	printf '\n'
done

files_cnt=`ls -lR "$dir"/*.ctxt | wc -l`
printf '\nTotal number of files tried: %s\n' "$files_cnt"
