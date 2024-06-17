#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "client.h"
#define fileLine 50

int main(int argc, char* argv[]) {
    int clientfd, result, index;
    Cabinet record;
    char check, fileName[fileLine], buf[BUFSIZ];
    FILE* file = NULL;
    size_t fsize, nsize = 0;

    struct sockaddr_in serverAddr;

    // 소켓 생성
    clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd < 0) {
        perror("socket creation failed");
        return 1;
    }

    // 서버 주소 설정
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 서버 IP 주소
    serverAddr.sin_port = htons(atoi(argv[1])); // 서버 포트 번호

    // 서버에 연결 요청
    result = connect(clientfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result < 0) {
        perror("connect failed");
        close(clientfd);
        return 1;
    }

    // 사물함 리스트 출력
    show_cabinet_list();

    // 사용자로부터 index 입력 받기
    printf("Index Input: ");
    scanf("%d", &index);

    // index 값을 서버로 전송
    if (send(clientfd, &index, sizeof(index), 0) != sizeof(index)) {
        perror("send failed");
        close(clientfd);
        return 1;
    }

    // 서버로부터 사물함 정보 수신
    if (recv(clientfd, &record, sizeof(record), 0) != sizeof(record)) {
        perror("Failed to receive cabinet information");
        close(clientfd);
        return 1;
    }
    printf("record: %d\n", record.index);

    // 사물함 정보 출력
    show_my_cabinet(record.index);

    // 파일 전송 여부 확인
    printf("\nDo you want to insert a file? (Y/N)\n");
    scanf(" %c", &check);
    if (check == 'Y') {
        send(clientfd, &check, sizeof(char), 0);
        strcpy(fileName, record.file_name);
        send(clientfd, fileName, strlen(fileName) + 1, 0);

        file = fopen("file1.txt", "rb");
        if (!file) {
            perror("Failed to open file");
            close(clientfd);
            return 1;
        }

        fseek(file, 0, SEEK_END);
        fsize = ftell(file);
        fseek(file, 0, SEEK_SET);

        while (nsize < fsize) {
            int fpsize = fread(buf, 1, sizeof(buf), file);
            if (fpsize < 0) break;
            nsize += fpsize;
            send(clientfd, buf, fpsize, 0);
        }
        printf("\nsize: %zu\n", nsize);
        fclose(file);
    } else {
        close(clientfd);
    }

    return 0;
}
