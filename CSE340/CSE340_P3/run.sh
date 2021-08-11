#!/bin/sh
g++ -c lexer.cc inputbuf.cc
g++ -o main.exe lexer.o inputbuf.o
