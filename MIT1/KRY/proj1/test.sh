#!/usr/bin/bash

dir=`pwd`
for file in "$dir"/*.ctxt
do
	printf '%s\n' "$file"
	./kry < "$file"
	printf '\n'
done

files_cnt=`ls -lR "$dir"/*.ctxt | wc -l`
printf '\nTotal number of files tried: %s\n' "$files_cnt"
