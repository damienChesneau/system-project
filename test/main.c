
#include <string.h>
#include <stdio.h>

#include <stdlib.h>
#include "../src/parser.h"

int main(int argc, char** argv) {
    Data * data = (Data *) malloc(sizeof(Data)*3);
    data[0].data="dfghjkl";
    strcpy(data[0].path ,"hellof1");
    data[0].timestamp = 2020;
    data[1].data="sfdddddddddddddddd";
    strcpy(data[1].path ,"hellf2");
    data[1].timestamp = 5454;
    data[2].data="sdddsdsddddddddddddddddddd";
    strcpy(data[2].path ,"hellof3");
    data[2].timestamp = 4545;
		
    char * z = encode(data,3);
    printf("RES= %s\n", z);

    Data * datatoret;
    datatoret = decode(z);
    printf("TEST=%s\n", datatoret[0].path);
    printf("TEST=%s\n", datatoret[1].path);
    printf("TEST=%s\n", datatoret[2].path);
    printf("TESTTIM=%d\n", datatoret[0].timestamp);
    printf("TESTTIM=%d\n", datatoret[1].timestamp);
    printf("TESTTIM=%d\n", datatoret[2].timestamp);
    printf("TESTDATA=%s\n", datatoret[0].data);
    printf("TESTDATA=%s\n", datatoret[1].data);
    printf("TESTDATA=%s\n", datatoret[2].data);
    return 0;
}

