#include <stdio.h>

#include "parser.h"

char* appendTo(const char * dest, const char *s) {
    int sLength = strlen(s);
    int destLen = strlen(dest);
    int totalLength = sLength + destLen;

    char * strBuf = malloc(totalLength + 1);
    strcpy(strBuf, dest);
    strcpy(strBuf + destLen, s);
    dest = strBuf;
    return strBuf;
}

char* encode(Data * data, int length) {
    char * d = "{";
    int i = 0;
    for (i = 0; i < length; i++) {
        d = appendTo(d, "[#");
        d = appendTo(d, data[i].path);
        char buf[255];
        int n = 0;
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

Data * decode(const char * data) {
    int length = strlen(data);
    char * newData = &data[0];
    int i = 0, nb_of_data = 0, nb_of_sharps;

    for (i = 0; i < length; i++) {
        if (newData[i] == '[') {
            nb_of_data++;
        }
    }
    int pos1,pos2,pos3,occurs=0;
    Data datatoret[nb_of_data];
    for (i = 0; i < length; i++) {
        if (newData[i] == '#') {
            nb_of_sharps++;
            printf("ds\n");
            char path [200];
            path[100] = '\0';
            switch (nb_of_sharps) {
                case 1:
                    pos1 = i + 1;
//                    int  postoUSe = (occurs == 0 )?0: pos3;
//                    printf("T=%d %d\n",postoUSe, i);
//                     int len1 = i - postoUSe;
//                    strncpy(path, &newData[postoUSe],len1);
//                    path[len1] = '\0';
//                    printf("T1= %s\n",path);
//                    strcpy(datatoret[occurs].path, path);
                    break;
                case 2:
                    pos2 = i + 1;
                    int len = i - pos1;
                    strncpy(path, &newData[pos1],len);
                    path[len] = '\0';
                    strcpy(datatoret[occurs].path, path); 
                    break;
                case 3: 
                    pos3 = i + 1;
                   printf("POS= '%d' '%d'\n",pos2,i);
                    int len2 = i - pos2;
                    printf("LEN= %d\n",len2);
                    strncpy(path, &newData[pos2],len2);
                    path[len2] = '\0';
//                    strcpy(datatoret[occurs]., path);
                    printf("T1= %s\n", path);
                    
                    occurs ++;
                    break;
                default: printf("default\n");
            }
            if (nb_of_sharps == 3) {
                nb_of_sharps = 0;
            }
        }
    }
}

