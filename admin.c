#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include"admin.h"
#include "cabinet_manager.h"
#include "api.h"

int get_admin_fd() {
    int admin_fd;
    admin_fd = open(ADMIN_DB, O_RDWR | O_CREAT, 0640);
    if (admin_fd == -1) {
        perror("admin_db connection error");
        return -1;
    }
    return admin_fd;
}

/**
 * admin 계정 db를 초기화하는 함수
 */
void init_admin_account() {
    int fd = get_admin_fd();
    if (fd == -1) return;

    AdminAccount admin;
    lseek(fd, 0, SEEK_SET);
    if (read(fd, &admin, sizeof(admin)) <= 0) {
        // Set default password if file is empty
        strncpy(admin.password, INIT_ADMIN_PASSWORD, sizeof(admin.password));
        lseek(fd, 0, SEEK_SET);
        if (write(fd, &admin, sizeof(admin)) == -1) {
            perror("Failed to write to admin_db");
        }
    }

    close(fd);
}

int set_password() {
    AdminAccount account;


    int fd = get_admin_fd();
    if (fd == -1) return -1;

    int i = 0;
    printf("Enter new password(8word): ");
    getchar();  //버퍼 지우는 용도
    while (1) {
        if (i > PASSWORD_SIZE - 1) break;
        account.password[i] = getkey();
        printf("*");
        i++;
    }
    account.password[8] = '\0';

    lseek(fd, 0, SEEK_SET);
    if (write(fd, &account, sizeof(AdminAccount)) == -1) {
        perror("Failed to write to cabinet_db");
        close(fd);
        return -1;
    }

    close(fd);
    return 1;
}

int show_admin_menu() {
    int cmd;
    init_admin_account();
    printf("show_all_cabinet (1) | unlock_cabinet (2) | clear_cabinet (3) | change_admin_password (4)\n");
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
        case 4:
            set_password();
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
