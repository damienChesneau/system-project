all: clean dir syncronizer 

syncronizer: lparser lcopy lconnect
	gcc src/syncronizer.c ./bin/lparser.o ./bin/lcopy.o ./bin/lconnect.o -Wall -ansi -o ./bin/synchronizer -lpthread
lparser: 
	gcc -c ./src/parser.c -o ./bin/lparser.o
lcopy: 
	gcc -c ./src/copy.c -o ./bin/lcopy.o
lconnect:
	gcc -c ./src/connect.c -o ./bin/lconnect.o	
dir:
	mkdir bin
clean:
	rm -r -f ./bin/
