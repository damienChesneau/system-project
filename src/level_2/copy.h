#define _BSD_SOURCE  1
#define _SVID_SOURCE 1
#include<dirent.h>
#include "parser.h"

void print_data(Data * me_data,int size);

Data* get_data_form_dir(const char * dir, int * nb_of_datas);
Data * filter_and_replace(Data * me_data, int *nb_of_me_data, Data * data, int length);
int update_folder(Data * data, int size);
