#! /bin/env sh
bison -d -v -Wall parser.y
flex lexer.fl
gcc *.c -Ofast -lm -o trip
echo "Usage: trip <input file>"
rm parser.tab.* lex.yy.c parser.output