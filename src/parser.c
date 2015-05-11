#include "parser.h"

char* appendTo(const char * dest, const char *s) {
    int sLength = strlen(s);
    int destLen = strlen(dest);
    int totalLength = sLength + destLen;

    char * strBuf = malloc(totalLength + 1);
    strcpy(strBuf, dest);
    strcpy(strBuf + destLen, s);
    dest= strBuf;
    return strBuf;    
}

char* encode(Data * data, int length){
    char * d = "dfs";
    d = appendTo(d,"he\0");
    return d;
}

Data * decode(char * data){
    
}

