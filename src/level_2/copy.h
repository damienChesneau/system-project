#ifndef _T_COPY_T_
#define _T_COPY_T_

#include "parser.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


void print_data(Data * me_data,int size);

Data* get_data_form_dir(const char * dir, int * nb_of_datas);
Data * filter_and_replace(Data * me_data, int *nb_of_me_data, Data * data, int length);
int update_folder(Data * data, int size);

#endif
