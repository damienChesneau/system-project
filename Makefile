all: clean dir level_1_test 

level_1_test: lparser
	gcc src/level_1.c lparser.o -Wall -ansi -o ./bin/level_1  -lpthread
lparser: 
	gcc -c ./src/parser.c -o lparser.o
dir:
	mkdir bin
clean:
	rm -r -f ./bin/
