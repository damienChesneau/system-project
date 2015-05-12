
typedef struct data {
    char * path;/*FILE NAME*/
    int timestamp; /* DATE OF LASTEST UPDATE */
    char * data; /* FILE DATA */
} Data;

Data * decode(const char * data);
char * encode(Data * data, int length);