#ifndef USER_H
#define USER_H
#define INIT_ADMIN_PASSWORD "88888888"
#define ADMIN_DB "adminDB"

#include <stdio.h>

typedef struct AdminAccount
{
    char password[9];
}AdminAccount;

void unlock_cabinet_menu();
int show_admin_menu();


#endif