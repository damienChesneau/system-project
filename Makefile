all: level_1_test 

level_1_test: lparser
	gcc src/level_1.c lparser.o -Wall -ansi -o level_1
lparser: 
	gcc -c ./src/parser.c -o lparser.o
clean:
	rm -r -f ./bin/
