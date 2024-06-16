#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cabinet_manager.h"

#define CABINET_DB "cabinet_db.bin"
#define START_CABINET_INDEX 1
#define MAX_CABINET_SIZE 10

void print_cabinet(Cabinet cabinet) {
    printf("Cabinet Index: %d, File Name: %s\n", cabinet.index, cabinet.file_name);
}

int main() {
    // �繰�� �����ͺ��̽� �ʱ�ȭ üũ
    check_cabinet_null();

    // �繰�� ���� �׽�Ʈ
    Cabinet cabinet1 = {1, "file1.txt"};
    if (set_cabinet(cabinet1) == 1) {
        printf("Cabinet 1 set successfully.\n");
    } else {
        printf("Failed to set Cabinet 1.\n");
    }

    // �繰�� �������� �׽�Ʈ
    Cabinet retrieved_cabinet1 = get_cabinet(1);
    print_cabinet(retrieved_cabinet1);

    // �繰�� ����ִ��� Ȯ�� �׽�Ʈ
    int isEmpty = is_cabinet_empty(1);
    printf("Is Cabinet 1 Empty? %d\n", isEmpty);

    // �繰�� ���� �׽�Ʈ
    if (clear_cabinet(1) == 1) {
        printf("Cabinet 1 cleared successfully.\n");
    } else {
        printf("Failed to clear Cabinet 1.\n");
    }

    // �ٽ� �������� �� Ȯ��
    Cabinet cleared_cabinet = get_cabinet(1);
    print_cabinet(cleared_cabinet);
    isEmpty = is_cabinet_empty(1);
    printf("Is Cabinet 1 Empty after clearing? %d\n", isEmpty);

    return 0;
}
