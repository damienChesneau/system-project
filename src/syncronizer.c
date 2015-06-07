#include "connect.h"
/*
Level 1: Each repository has an execution of this program (multiple instances) and we must synchronize them.
*/

int main(int argc, char* argv[]) {
    char ** ips = NULL;
    if(argc > 1){
     if((ips = (char **) malloc(sizeof (char *)*argc)) == NULL){
     	perror(argv[0]);
     	return EXIT_FAILURE;
     }
    }
    /*char * ips[argc+1];*/
    pthread_t thread1;
    
    if (pthread_create(&thread1, NULL, switchman, NULL) == -1) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }
    int i = 0;
    for (i = 1; i < argc; i++) {
        if((ips[i - 1] = (char *) malloc(sizeof (char)*strlen(argv[i]))) == NULL){
        	perror(argv[0]);
        	return EXIT_FAILURE;
        }
        strncpy(ips[i - 1], argv[i],strlen(argv[i]));
    }
    
    sleep(3);
    if(argc>1){
    	ips[argc] = NULL;
    	connectman(ips,argc-1);
    }
    free(ips);
    ips = NULL;
    while (1) {
        sleep(1000);
    }
    return EXIT_SUCCESS;
}
