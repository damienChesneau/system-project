#ifndef _T_PARSER_T_
#define _T_PARSER_T_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PATH_SIZE 256

typedef struct data {
    char path[PATH_SIZE];/*FILE NAME*/
    int timestamp; /* DATE OF LASTEST UPDATE */
    char * data; /* FILE DATA */
    int mode; /* PERMISSIONS OF A FILE */
} Data;


/*
	Returns an array of Data decoded from the string data and the length of this array in nb.
*/
Data * decode(const char * data, int * nb);

/*
	Returns an encoded string from a Data data.
*/
char * encode(Data * data, int length);

/*
	Free an encoded message.
*/
void free_encoded_message(char * msg);

/*
	Free an array of Data.
*/
void free_data(Data * data, int length);

#endif
