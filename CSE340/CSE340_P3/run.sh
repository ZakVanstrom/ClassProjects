#!/bin/sh
g++ -c lexer.cc inputbuf.cc
g++ -o main.exe lexer.o inputbuf.o

for i in 1 2 3 4 5 6 7 8 9 10 11 12
do
	echo "Test: $i"
	cat "old_tests/test$i.txt" | ./main.exe > "old_tests/result$i.txt"

	#cat "results/result$i.txt"
	#cat "old_tests/result$i.txt"
	
	if cmp -s "results/result$i.txt" "old_tests/result$i.txt"; then
    	echo "PASSED!"
	else
    	echo "failed... :("
	fi

	rm "old_tests/result$i.txt"
	echo
done

