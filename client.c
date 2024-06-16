#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include "client.h"

int clients(){

    unsigned int clientfd, index;
    int result;
    char password[PWDLength];
    struct sockaddr serverUnixaddr;

    clientfd = socket(AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUnixaddr.sun_family = AF_UNIX;
    strcpy(serverUnixaddr, "convert");

    do {
        result = connect(clientfd, &serverUnixaddr, sizeof(serverUnixaddr));
        if (result == -1) sleep(1);
    } while (result == -1);

    printf("인덱스 입력:\n");
    fgets(index, 2, stdin);
    write(clientfd, index, sizeof(index));

    printf("패스워드 입력:\n");
    fgets(password, PWDLength, stdin);
    write(clientfd, password, strlen(password) + 1);




}