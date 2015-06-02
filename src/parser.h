
typedef struct data {
    char path[255];/*FILE NAME*/
    int timestamp; /* DATE OF LASTEST UPDATE */
    char * data; /* FILE DATA */
} Data;

Data * decode(const char * data, int * nb);
char * encode(Data * data, int length);

void free_encoded_message(char * msg);
void free_data(Data * data, int length);
