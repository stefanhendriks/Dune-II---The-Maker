#! /bin/sh
#
#  Convert text files from CR/LF to LF format.


echo "Converting files to Unix format..."

find . -type f "(" -name "*.c" -o -name "*.h" -o -name "makefile" -o -name "*.txt" ")" \
   -exec sh -c "echo {};
		mv {} _tmpfile;
		tr -d \\\r < _tmpfile > {};
		touch -r _tmpfile {}; 
		rm _tmpfile" \;
