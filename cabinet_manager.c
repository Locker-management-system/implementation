#include<stdio.h>
#include<fcntl.h>
#include"cabinet_manager.h"

/**
 * 사물함 레코드 파일 디스크립터를 반환하는 함수
 * @return 사물함 레코드 fd, 실패하면 -1반환
 */
int get_cabinet_fd() {
    int cabinet_fd;
    cabinet_fd = open(CABINET_DB, O_RDWR | O_CREAT, 0640);
    if (cabinet_fd == -1) {
        perror("cabinet_db 연결 오류");
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
void check_cabinet_null() {
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
 * 파일 이름이 \0일 시 비어있는 케비넷으로 인식한다.
 * @param index 검사해볼 사물함의 인덱스
 * @return 비어있다면 1, 비어있지 않다면 0;
 */
int is_cabinet_empty(int index){
    int fd = get_cabinet_fd();
    if (fd == -1) return 0;

    Cabinet record;
    lseek(fd, (index - START_CABINET_INDEX) * sizeof(record), SEEK_SET);
    if (read(fd, &record, sizeof(record)) == -1) {
        perror("Failed to read from cabinet_db");
        close(fd);
        return 0;
    }

    close(fd);
    return record.file_name[0] == '\0';

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
 * @return
 */
int clear_cabinet(int index) {
    Cabinet record;
    record.index = index;
    strcpy(record.file_name, "");

    return set_cabinet(record);
}