#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"

char* appendTo(char * dest, const char *s) {
    int sLength = strlen(s);
    int destLen = strlen(dest);
  
    int totalLength = sLength + destLen;
    
    char * strBuf = (char *) malloc(sizeof(char)*(totalLength+1));
    
    strcpy(strBuf, dest);
    strcpy(strBuf + destLen, s);
    free(dest);
    return strBuf;
}

char* encode(Data * data, int length) {
    char * d = malloc(sizeof(char)*2);
    d[0] = '{';
    d[1] = '\0';
    int i = 0;
    for (i = 0; i < length; i++) {
        d = appendTo(d, "[#");
        d = appendTo(d, data[i].path);
        char buf[255];
        sprintf(buf, "%d", data[i].timestamp);
        d = appendTo(d, "#");
        d = appendTo(d, buf);
        d = appendTo(d, "#");
        d = appendTo(d, data[i].data);
        d = appendTo(d, "]");
        if (i != length - 1) {
            d = appendTo(d, ",");
        }
    }
    d = appendTo(d, "}\0");
    return d;
}

Data * decode(const char * data,int * nb) {
    int length = (int) strlen(data);
    char * newData = (char*) &data[0];
    int i = 0, nb_of_data = 0, nb_of_sharps = 0;
    char path [200];
    path[199] = '\0';
    int pos1, pos2, pos3=0, occurs = 0;
    
    for (i = 0; i < length; i++) {
        if (newData[i] == '[') {
            nb_of_data++;
        }
    }
    *nb = nb_of_data;
    Data * datatoret = (Data *)malloc(sizeof (Data) * nb_of_data);
    for (i = 0; i < length; i++) {
        if (newData[i] == '#') {
            nb_of_sharps++;
            switch (nb_of_sharps) {
                case 1:
                    pos1 = i + 1;
                    break;
                case 2:
                    pos2 = i + 1;
                    int len = i - pos1;
                    strncpy(path, &newData[pos1], len);
                    path[len] = '\0';
                    
                    strncpy(datatoret[occurs].path, path, strlen(path) + 1);
                    break;
                case 3:
                    pos3 = i + 1;
                    int len2 = i - pos2;
                    strncpy(path, &newData[pos2], len2);
                    path[len2] = '\0';
                    int tra = atoi(path);
                    datatoret[occurs].timestamp = tra;
                    break;
            }
        } else if (newData[i] == ']') {
            int len3 = i - pos3;
            char * strBuf = malloc(len3 + 1);
            strncpy(strBuf, &newData[pos3], len3);
            strBuf[len3] = '\0';
            datatoret[occurs - 1].data = strBuf;
        }
        if (nb_of_sharps == 3) {
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
