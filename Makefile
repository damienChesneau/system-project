all: clean dir level_2_test 

level_2_test: lparser_2 lcopy_2 lconnect_2
	gcc src/level_2/level_2.c ./bin/lparser.o ./bin/lcopy.o ./bin/lconnect.o -Wall -ansi -o ./bin/synchronizer -lpthread
lparser_2: 
	gcc -c ./src/level_2/parser.c -o ./bin/lparser.o
lcopy_2: 
	gcc -c ./src/level_2/copy.c -o ./bin/lcopy.o
lconnect_2:
	gcc -c ./src/level_2/connect.c -o ./bin/lconnect.o	

level_1_test: lcopy_1 lparser_1 
	gcc src/level_1/level_1.c ./bin/lparser.o ./bin/lcopy.o -Wall -ansi -o ./bin/synchronizer  -lpthread
lparser_1: 
	gcc -c ./src/level_1/parser.c -o ./bin/lparser.o
lcopy_1: 
	gcc -c ./src/level_1/copy.c -o ./bin/lcopy.o

dir:
	mkdir bin
clean:
	rm -r -f ./bin/
