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

/*
	Says if the string filename is a folder.
*/
int is_directory(const char * filename);

/*
	Returns permisions of a file of name filename.
*/
int get_mode(char * filename);

/*
	Returns the timestamp of a file of name filename.
*/
int get_timestamp_of_file(char * filename);

/*
	Returns the number of files in a foder dir. It's recursif.
*/
int count_dir_files(const char * dir) ;

/*
	Returns in the array of string files the path of every files in the folder dir. It's recursif.
*/
void get_all_files_from_dir(const char * dir, int * i, char ** files);

/*
	Returns in buffer a string of data of the file of name filename.
*/
void get_data_from_file(const char * filename, char ** buffer);

#endif
