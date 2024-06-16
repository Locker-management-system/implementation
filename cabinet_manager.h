#ifndef CABINET_MANAGER_H
#define CABINET_MANAGER_H
#define MAX_CABINET_SIZE 10
#define START_CABINET_INDEX 1
#define CABINET_DB "cabinetDB"
#define MAX_INCORRECT_PASSWORD 5
#define CABINET_STATUS_LOCK 1
#define CABINET_STATUS_USED 0
#define CABINET_STATUS_EMPTY -1

#include <stdio.h>
#include "cabinet.h"

int set_cabinet(Cabinet cabinet);
int clear_cabinet(int cabinet_index);
void init_cabinet();
int is_cabinet_empty(int index);
Cabinet get_cabinet(int index);
void show_cabinet_list();
void show_cabinet(int index);
void select_cabinet();
void register_cabinet();
int input_password(int index);
void lock_cabinet(int index);
void show_lock_cabinet(int index);
void show_empty_cabinet(int index);
void show_used_cabinet(int index);
void show_my_cabinet(int index);
int is_cabinet_lock(int index) ;
void register_cabinet(int index);

#endif