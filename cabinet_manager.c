#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include"cabinet_manager.h"
#include"api.h"

/**
 * 사물함 레코드 파일 디스크립터를 반환하는 함수
 * @return 사물함 레코드 fd, 실패하면 -1반환
 */
int get_cabinet_fd() {
    int cabinet_fd;
    cabinet_fd = open(CABINET_DB, O_RDWR | O_CREAT, 0640);
    if (cabinet_fd == -1) {
        perror("cabinet_db connetion error");
        return -1;
    }

    return cabinet_fd;
}

/**
 * 사물함의 정보를 설정
 * @param cabinet 원하는 사물함의 정보
 * @return 성공할 시 1, 실패 시 -1
 */
int set_cabinet(Cabinet cabinet) {
    int fd = get_cabinet_fd();
    if (fd == -1) return -1;

    lseek(fd, (cabinet.index - START_CABINET_INDEX) * sizeof(Cabinet), SEEK_SET);
    if (write(fd, &cabinet, sizeof(Cabinet)) == -1) {
        perror("Failed to write to cabinet_db");
        close(fd);
        return -1;
    }

    close(fd);
    return 1;
}

/**
 * 사물함의 레코드가 초기화되어있는지 확인한 후 초기화가 필요하면 초기화를 진행
 * @return
 */
void init_cabinet() {
    int fd = get_cabinet_fd();
    if (fd == -1) return;

    Cabinet record;
    lseek(fd, 0, SEEK_SET);
    if (read(fd, &record, sizeof(record)) <= 0) {
        for (int i = START_CABINET_INDEX; i <= MAX_CABINET_SIZE; i++) {
            clear_cabinet(i);
        }
    }
    close(fd);
}

/**
 * 특정 인덱스의 사물함이 비어있는지 확인하는 함수
 * 비밀번호 첫번 째 자리가 \0일 시 비어있는 사물함으로 인식한다.
 * @param index 검사해 볼 사물함의 인덱스
 * @return 비어있다면 1, 비어있지 않다면 0;
 */
int is_cabinet_empty(int index){
    int fd = get_cabinet_fd();
    if (fd == -1) return 0;

    Cabinet record = get_cabinet(index);

    close(fd);
    return record.status == CABINET_STATUS_EMPTY;

}

/**
 * 원하는 인덱스의 사물함을 가져오는 함수
 * @param index 사물함 인덱스
 * @return 하당 인덱스의 사물함
 */
Cabinet get_cabinet(int index) {
    Cabinet record;
    int fd = get_cabinet_fd();
    if (fd == -1) return record;

    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = (index-START_CABINET_INDEX)*sizeof(Cabinet);
    lock.l_len = sizeof(Cabinet);

    if(fcntl(fd, F_SETLK, &lock) == -1) {
        perror("fcntl error:");
    }

    lseek(fd, (index - START_CABINET_INDEX) * sizeof(record), SEEK_SET);
    if (read(fd, &record, sizeof(record)) == -1) {
        perror("Failed to read from cabinet_db");
    }

    close(fd);
    return record;
}

/**
 * 원하는 인덱스의 사물함을 비우는 함수
 * @param index 사물함 인덱스
 * @return 성공할 시 1, 실패할 시 0
 */
int clear_cabinet(int index) {
    Cabinet record;
    record.index = index;
    record.status = CABINET_STATUS_EMPTY;
    return set_cabinet(record);
}

/**
 * 캐비넷 리스트를 정렬해서 보여주는 함수
 */
void show_cabinet_list(){
    for (int i = START_CABINET_INDEX; i <= MAX_CABINET_SIZE; i++) {
        show_cabinet(i);
    }
}

/**
 * 사물함 하나를 보여주는 함수
 * @param cabinet 보여줄 사물함
 */
void show_cabinet(int index){

    Cabinet cabinet = get_cabinet(index);

    switch (cabinet.status) {
        case CABINET_STATUS_LOCK:
            show_lock_cabinet(index);
            break;
        case CABINET_STATUS_EMPTY:
            show_empty_cabinet(index);
            break;
        case CABINET_STATUS_USED:
            show_used_cabinet(index);
            break;
    }
}

void register_cabinet(int index){
    Cabinet cabinet;
    cabinet.index = index;
    cabinet.status = CABINET_STATUS_USED;
    system("clear");
    printf("input the file name you wnat to archive: ");
    scanf("%s", cabinet.file_name);
    printf("input the file discription: ");
    scanf("%s", cabinet.file_description);
    printf("Enter your password(8word): ");
    cabinet.incorrect_cnt = 0;
    int i = 0;
    getchar();  //버퍼 지우는 용도
    while (1) {
        if (i > PASSWORD_SIZE - 1) break;
        cabinet.password[i] = getkey();
        printf("*");
        i++;
    }
    cabinet.password[8] = '\0';
    set_cabinet(cabinet);
    printf("\nregistered success!");
    printf("%s",cabinet.password);

}

/**
 * 사용할 사물함을 고르는 함수
 */
void select_cabinet(){
    int index;

    printf("select your cabinet: ");
    scanf("%d", &index);

    if(is_cabinet_lock(index) == 1) {   //사물함이 잠겨있는 경우
        printf("Contact this system administrator\n");
        return;
    }

    if(is_cabinet_empty(index) == 1){   //비어있는 사물함의 경우 바로 사물함을 등록한다.
        register_cabinet(index);
        return;
    }

    if(input_password(index) == 1) {
        //비밀번호 해제 성공
        show_my_cabinet(index);
    }
}

/**
 * 사물함이 잠겨있는지 확인하는 함수
 * @param index 사물함의 인덱스
 * @return 잠겨있으면 1, 잠겨있지 않다면 0
 */
int is_cabinet_lock(int index) {
    Cabinet cabinet = get_cabinet(index);
    if(cabinet.status == CABINET_STATUS_LOCK) {
        return 1;
    }
    return 0;
}

/**
 * 사물함을 잠그는 함수
 * @param index 사물함의 인덱스
 */
void lock_cabinet(int index) {
    Cabinet cabinet = get_cabinet(index);
    cabinet.status = CABINET_STATUS_LOCK;
    set_cabinet(cabinet);
}

/**
 * 비밀번호를 입력 받는 함수.
 * 비밀번호 입력에 실패하면 해당 사물함을 잠금상태로 만든다.
 * @param index 비밀번호를 입력 받을 인덱스
 * @return 비밀번호입력에 성공하면 1, 실패하면 -1
 */
int input_password(int index) {
    int incorrect_cnt = 0;
    Cabinet cabinet = get_cabinet(index);
    char password[9];
    getchar();
    while (incorrect_cnt < MAX_INCORRECT_PASSWORD) {
        system("clear");
        printf("Cabinet Index %d left %d times (8word): ", index, MAX_INCORRECT_PASSWORD - incorrect_cnt);

        int i = 0;
        while (1) {
            if (i > PASSWORD_SIZE - 1) break;
            password[i] = getkey();
            printf("*");
            i++;
        }
        password[8] = '\0';

        if(strcmp(password, cabinet.password) == 0) {
            printf("\n");
            return 1;
        }
        incorrect_cnt++;
    }
    lock_cabinet(index);
    return -1;
}

void show_my_cabinet(int index){
    Cabinet cabinet = get_cabinet(index);
    printf("+------------------------------+\n");
    printf("| Cabinet Index: %-15d|\n", cabinet.index);
    printf("| File Name: %-19s|\n", cabinet.file_name);
    printf("| Description: %-17s|\n", cabinet.file_description);
    printf("+------------------------------+\n");
}

void show_lock_cabinet(int index) {
    printf("+------------------------------+\n");
    printf("| Cabinet Index: %-15d|\n", index);
    printf("| Status: %-22s|\n", "Lock");
    printf("|              %-17s|\n"," ");
    printf("+------------------------------+\n");
}
void show_empty_cabinet(int index) {
    printf("+------------------------------+\n");
    printf("| Cabinet Index: %-15d|\n", index);
    printf("| Status: %-22s|\n", "Empty");
    printf("|              %-17s|\n"," ");
    printf("+------------------------------+\n");
}
void show_used_cabinet(int index) {
    printf("+------------------------------+\n");
    printf("| Cabinet Index: %-15d|\n", index);
    printf("| Status: %-22s|\n", "Used");
    printf("|              %-17s|\n"," ");
    printf("+------------------------------+\n");
}