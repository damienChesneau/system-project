#define _BSD_SOURCE  1
#define _SVID_SOURCE 1
#include<dirent.h>
#include "parser.h"
typedef int (*Fd_open)(int,struct dirent*);


int fd_open_out(int fd_in,struct dirent* file);
int fd_open_in(int fd_out,struct dirent* file);

int copy_all(int fd,Fd_open func);
int copy(int fd_out,int fd_in);
int isDirectory(struct dirent* info,char* path);

void get_data_form_dir(const char * dir, int * nb_of_datas, Data * datas);
