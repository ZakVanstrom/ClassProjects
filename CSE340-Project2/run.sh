#!/bin/sh
g++ -c lexer.cc inputbuf.cc
g++ -o main.exe lexer.o inputbuf.o

for i in 1 2 3 4 5 6 7 8 9 10 11 12
do
	echo "Test: $i"
	cat "test$i.txt" | ./main.exe
	echo
done

