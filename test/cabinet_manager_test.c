#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cabinet_manager.h"

#define START_CABINET_INDEX 1
#define MAX_CABINET_SIZE 10

void print_cabinet(Cabinet cabinet) {
    printf("Cabinet Index: %d, File Name: %s\n", cabinet.index, cabinet.file_name);
}

int main() {
    // 사물함 데이터베이스 초기화 체크
    init_cabinet();

    // 사물함 설정 테스트
    Cabinet cabinet1 = {1, "file1.txt"};
    if (set_cabinet(cabinet1) == 1) {
        printf("Cabinet 1 set successfully.\n");
    } else {
        printf("Failed to set Cabinet 1.\n");
    }

    // 사물함 가져오기 테스트
    Cabinet retrieved_cabinet1 = get_cabinet(1);
    print_cabinet(retrieved_cabinet1);

    // 사물함 비어있는지 확인 테스트
    int isEmpty = is_cabinet_empty(1);
    printf("Is Cabinet 1 Empty? %d\n", isEmpty);

    // 사물함 비우기 테스트
    if (clear_cabinet(1) == 1) {
        printf("Cabinet 1 cleared successfully.\n");
    } else {
        printf("Failed to clear Cabinet 1.\n");
    }

    // 다시 가져오기 및 확인
    Cabinet cleared_cabinet = get_cabinet(1);
    print_cabinet(cleared_cabinet);
    isEmpty = is_cabinet_empty(1);
    printf("Is Cabinet 1 Empty after clearing? %d\n", isEmpty);

    show_cabinet_list();

    return 0;
}
