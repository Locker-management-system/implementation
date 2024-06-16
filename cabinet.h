#ifndef CABINET_H
#define CABINET_H

#include <stdio.h>

typedef struct Cabinet{
    int index;
    char password[9];
    char file_name[19];
    char file_description[17];
    int incorrect_cnt;
    int status;    //잠금 상태 1, 잠금 해제 0, 비어있는상태 -1
}Cabinet;


#endif