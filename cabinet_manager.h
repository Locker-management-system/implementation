#ifndef CABINET_MANAGER_H
#define CABINET_MANAGER_H
#define MAX_CABINET_SIZE 10
#define START_CABINET_INDEX 1
#define CABINET_DB "cabinetDB"

#include <stdio.h>
#include "cabinet.h"

int set_cabinet(Cabinet cabinet);
int clear_cabinet(int cabinet_index);
void check_cabinet_null();
int is_cabinet_empty(int index);
Cabinet get_cabinet(int index);



#endif