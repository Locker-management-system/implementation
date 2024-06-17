#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "server.h"

void handle_client(int connfd) {
    int index;
    char check, fileName[50];
    Cabinet record;

    // 클라이언트로부터 index 값을 수신
    if (recv(connfd, &index, sizeof(index), 0) == sizeof(index)) {
        printf("Received int value: %d\n", index);
    } else {
        perror("recv failed");
        close(connfd);
        return;
    }

    // index에 해당하는 사물함 정보 가져오기
    record = get_cabinet(index);
    Cabinet cab = {index, "1234", "file1.txt"};
    set_cabinet(cab);

    // 사물함 정보를 클라이언트로 전송
    if (send(connfd, &record, sizeof(record), 0) != sizeof(record)) {
        perror("Failed to send cabinet information");
    }

    // 클라이언트로부터 파일 전송 여부와 파일 이름을 수신
    recv(connfd, &check, sizeof(char), 0);
    recv(connfd, fileName, sizeof(fileName), 0);

    if (check == 'Y') {
        FILE *file = fopen(fileName, "wb");
        if (!file) {
            perror("Failed to open file");
            close(connfd);
            return;
        }

        char buf[BUFSIZ];
        int nByte;

        // 파일을 수신하여 저장
        while ((nByte = recv(connfd, buf, sizeof(buf), 0)) > 0) {
            fwrite(buf, sizeof(char), nByte, file);
        }

        fseek(file, 0, SEEK_END);
        size_t file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        printf("file size = %zu\n", file_size);

        fclose(file);
    } else {
        printf("No file received\n");
    }

    close(connfd);
}

int main(int argc, char* argv[]) {
    int listenfd, connfd;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientlen = sizeof(clientAddr);

    // 소켓 생성
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("socket failed");
        return 1;
    }

    // 서버 주소 설정
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(atoi(argv[1]));

    // 소켓 바인딩
    if (bind(listenfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind failed");
        close(listenfd);
        return 1;
    }

    // 소켓 리스닝
    if (listen(listenfd, 5) < 0) {
        perror("listen failed");
        close(listenfd);
        return 1;
    }

    printf("Server is listening on port %s\n", argv[1]);

    while (1) {
        // 클라이언트 연결 수락
        connfd = accept(listenfd, (struct sockaddr*)&clientAddr, &clientlen);
        if (connfd < 0) {
            perror("accept failed");
            continue;
        }

        if (fork() == 0) {
            close(listenfd);
            handle_client(connfd);
            exit(0);
        } else {
            close(connfd);
        }
    }

    close(listenfd);
    return 0;
}
