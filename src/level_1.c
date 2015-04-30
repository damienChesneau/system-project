#include<stdio.h>
#include<stdlib.h>
#include<getopt.h>

/*
Level 1: Each repository has an execution of this program (multiple instances) and we must synchronize them.

*/

int main(int argc, char* argv[]){

	char* ip_addr = NULL;
	char options = 0;
	char* optstring="d:";
	int val;

	while (EOF!=(val=getopt(argc,argv,optstring))) { 
		 
		switch (val) {
			
		  case 'd': 
		 		options = 1;		 
		 		ip_addr = optarg;
		 		break;
		 		
		}
	}
	
	
	
	return EXIT_SUCCESS;
}
