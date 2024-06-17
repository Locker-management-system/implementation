#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include"admin.h"
#include "cabinet_manager.h"

int get_admin_fd() {
    int cabinet_fd;
    cabinet_fd = open(ADMIN_DB, O_RDWR | O_CREAT, 0640);
    if (cabinet_fd == -1) {
        perror("admin_db connetion error");
        return -1;
    }
    return cabinet_fd;
}

int show_admin_menu() {
    int cmd;
    printf("show_all_cabinet (1) | unlock_cabinet (2) | clear_cabinet (3)\n");
    printf("input command: ");
    scanf("%d", &cmd);

    switch (cmd) {
        case 1:
            show_all_cabinet_list();
            break;
        case 2:
            unlock_cabinet_menu();
            break;
        case 3:
            clear_all_cabinet();
            break;
    }

}

void unlock_cabinet_menu() {
    int index;
    printf("Enter the locker you want to unlock: ");
    scanf("%d", &index);

    Cabinet cabinet = get_cabinet(index);
    if(cabinet.status != CABINET_STATUS_LOCK) {
        printf("The cabinet is already unlocked");
        return;
    }
    unlock_cabinet(index);
}
