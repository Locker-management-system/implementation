#ifndef CABINET_MANAGER_H
#define CABINET_MANAGER_H
#define MAX_CABINET_SIZE 10

#include <stdio.h>
#include "cabinet.h"

int set_cabinet(Cabinet cabinet);
int set_file(int cabinet_index, char* file_path);
int delete_file(int cabinet_index);
int clear_cabinet(int cabinet_index);


#endif