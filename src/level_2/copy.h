#ifndef _T_COPY_T_
#define _T_COPY_T_

#include "parser.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/*
	Print every path and data of an array of Data. It's used for debuging.
*/
void print_data(Data * me_data,int size);

/*
	Returns an array of Data from a folder dir and the length of this array in nb_of_data. It's recursif.
*/
Data* get_data_form_dir(const char * dir, int * nb_of_datas);

/*
	Update me_data by getting all the latest or new data in data.
	Returns an updated array of Data and the length of this new array in nb_of_data.
*/
Data * filter_and_replace(Data * me_data, int *nb_of_me_data, Data * data, int length);

/*
	Update the current folder with the array of Data data.
	Returns 1 on succes and -1 on failure.
*/
int update_folder(Data * data, int size);

#endif
