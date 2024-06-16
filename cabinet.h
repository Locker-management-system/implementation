#ifndef CABINET_H
#define CABINET_H

#include <stdio.h>

typedef struct Cabinet{
    int index;
    char file_name[20];
    char file_description[100];
}Cabinet;


#endif