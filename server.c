#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <readline/readline.h>
#include "server.h"

int sockets() {
    unsigned int clientlen, index;
    int listenfd, connfd;
    struct sockaddr serverUnixaddr, clientUnixaddr;
    char password[PWDLength];

    clientlen = sizeof(clientUnixaddr);
    listenfd = socket(AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUnixaddr.sun_family = AF_UNIX;
    strcpy(serverUnixaddr.sun_path, "convert");
    unlink("convert");
    bind(listenfd, &serverUnixaddr, sizeof(serverUnixaddr));
    listen(listenfd, 5);

    while(1) {
        connfd = accept(listenfd, &clientUnixaddr, &clientlen);
        if (fork() == 0) {

            int fd = get_cabinet_fd();
            Cabinet record;

            show_cabinet_list();

            readIndex(connfd, index);
            readLine(connfd, password);

            lseek(fd, (index - START_CABINET_INDEX) * sizeof(record), SEEK_SET);
            if (read(fd, &record, sizeof(record)) != -1) {
                if(is_cabinet_empty(index) && password == cabinet.passward){
                    show_cabinet(record);
                } else if (!is_cabinet_empty(index)) {
                    write(fd, (char *) &record, sizeof(record));
                }
            }
        }
    }

}