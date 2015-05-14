all: clean dir level_1_test 

level_1_test: lcopy lparser 
	gcc src/level_1.c ./bin/lparser.o ./bin/lcopy.o -Wall -ansi -o ./bin/level_1  -lpthread
lparser: 
	gcc -c ./src/parser.c -o ./bin/lparser.o
lcopy: 
	gcc -c ./src/copy.c -o ./bin/lcopy.o
dir:
	mkdir bin
clean:
	rm -r -f ./bin/
