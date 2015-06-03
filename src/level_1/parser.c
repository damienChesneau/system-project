#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"

char* appendTo(char * dest, const char *s) {
    int sLength = strlen(s);
    int destLen = strlen(dest);
  
    int totalLength = sLength + destLen;
    char * strBuf;
    if((strBuf = (char *) malloc(sizeof(char)*(totalLength)+1)) == NULL){
    	perror("appendTo");
    	return NULL;
    }
    
    strncpy(strBuf, dest, destLen);
    strcpy(strBuf+destLen, s);
    free(dest);
    dest = NULL;
    return strBuf;
}

char* encode(Data * data, int length) {
    char * d = NULL;
    if((d = malloc(sizeof(char)*2)) == NULL){
    	perror("encode");
    	return NULL;
    }
    d[0] = '{';
    d[1] = '\0';
    int i = 0;
    for (i = 0; i < length; i++) {
        /*printf("%s\n",data[i].path);*/
        d = appendTo(d, "--++[++----++#++--");
        d = appendTo(d, data[i].path);
        char buf[256];
        sprintf(buf, "%d", data[i].timestamp);
        d = appendTo(d, "--++#++--");
        d = appendTo(d, buf);
        d = appendTo(d, "--++#++--");
        sprintf(buf, "%d", data[i].mode);
        d = appendTo(d, buf);
        d = appendTo(d, "--++#++--");
        d = appendTo(d, data[i].data);
        d = appendTo(d, "--++]++--");
        if (i != length - 1) {
            d = appendTo(d, ",");
        }
        /*printf("%s\n\n",d);*/
    }
    d = appendTo(d, "}\0");
    return d;
}

Data * decode(const char * data,int * nb) {
    int length = (int) strlen(data);
    char * newData = (char*) &data[0];
    int i = 0, nb_of_data = 0, nb_of_sharps = 0;
    char path [PATH_SIZE];
    path[PATH_SIZE - 1] = '\0';
    int pos1 = 0, pos2 = 0, pos3 = 0,pos4 = 0, occurs = 0;
    
    for (i = 0; i < length; i++) {
        if (newData[i] == '[') {
            nb_of_data++;
        }
    }
    *nb = nb_of_data;
    Data * datatoret;
    if((datatoret = (Data *)malloc(sizeof (Data) * nb_of_data)) == NULL){
    	perror("decode");
    	return NULL;
    }
    
    for (i = 0; i < length; i++) {
        if (newData[i-4] == '-' && newData[i-3] == '-' && newData[i-2] == '+'&& newData[i-1] == '+' && newData[i] == '#' && newData[i+1] == '+'&& newData[i+2] == '+'&& newData[i+3] == '-'&& newData[i+4] == '-') {
            nb_of_sharps++;
            switch (nb_of_sharps) {
                case 1:
                    pos1 = i + 5;
                    break;
                case 2:
                    pos2 = i + 5;
                    int len = i - pos1 - 4;
                    strncpy(path, &newData[pos1], len);
                    path[len] = '\0';
                    
                    strncpy(datatoret[occurs].path, path, strlen(path)+1);
                    break;
                case 3:
                    pos3 = i + 5;
                    int len2 = i - pos2-4;
                    strncpy(path, &newData[pos2], len2);
                    path[len2] = '\0';
                    int tra = atoi(path);
                    datatoret[occurs].timestamp = tra;
                    break;
                case 4:
                    pos4 = i + 5;
                    int len3 = i - pos3-4;
                    strncpy(path, &newData[pos3], len3);
                    path[len3] = '\0';
                    int tra2 = atoi(path);
                    datatoret[occurs].mode = tra2;
                    break;
            }
        } else if (newData[i-4] == '-' && newData[i-3] == '-' && newData[i-2] == '+'&& newData[i-1] == '+' && newData[i] == ']' && newData[i+1] == '+'&& newData[i+2] == '+'&& newData[i+3] == '-'&& newData[i+4] == '-') {
            int len4 = i - pos4-4;
           	char * strBuf;
            if((strBuf = malloc((sizeof(char)*len4) + 1)) == NULL){
            	perror("decode");
            	return NULL;
            }
            
            strncpy(strBuf, &newData[pos4], len4);
            strBuf[len4] = '\0';
            datatoret[occurs - 1].data = strBuf;
        }
        if (nb_of_sharps == 4) {
            nb_of_sharps = 0;
            occurs++;
        }
    }
    
    return datatoret;
}

void free_encoded_message(char * msg){
	free(msg);
	msg = NULL;
}

void free_data(Data * data, int length){
	int i;
	for(i = 0; i<length; i++){
		free(data[i].data);
		data[i].data = NULL;
	}
	free(data);
	data = NULL;
}
