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
        d = appendTo(d, data[0].path);
        char buf[255];
        int n = 0;
        sprintf(buf, "%d", data[0].timestamp);
        d = appendTo(d, "#");
        d = appendTo(d, buf);
        d = appendTo(d, "#");
        d = appendTo(d, data[0].data);
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
    Data datatoret[nb_of_data];
    for (i = 0; i < length; i++) {
        if (newData[i] == '#') {
            nb_of_sharps++;
            if (nb_of_sharps == 3) {
                nb_of_sharps = 0;
            }
            switch (nb_of_sharps){
                case 1 :
                    break;
            }
        }
    }
}

