#define _BSD_SOURCE  1
#define _SVID_SOURCE 1
#include<dirent.h>
#include "parser.h"
int isDirectory(struct dirent* info,char* path);

Data* get_data_form_dir(const char * dir, int * nb_of_datas);
void filter_and_replace(Data * me_data, int *nb_of_me_data, Data * data, int *length);
int update_folder(char * dir,Data * data, int size);
