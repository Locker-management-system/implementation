#ifndef CABINET_H
#define CABINET_H

#include <stdio.h>

typedef struct Cabinet{
    int index;
    char password[20];
    char file_name[19];
    char file_description[17];
}Cabinet;


#endif