// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include "cabinet.h"

#define PORT 88845
#define MAX_CLIENTS 5
#define FILENAME "cabinet_data.bin"
#define MAX_CABINETS 10

typedef enum {
    SAVE_CABINET,
    REQUEST_CABINET,
    REQUEST_ALL_CABINETS,
    DOWNLOAD_FILE
} RequestType;

typedef struct {
    RequestType request_type;
    Cabinet cabinet;
    size_t file_size;
    char password[20];
} ClientRequest;

void initialize_file() {
    int fd = open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Could not initialize file");
        exit(EXIT_FAILURE);
    }
    Cabinet empty_cabinet;
    for (int i = 0; i < MAX_CABINETS; i++) {
        empty_cabinet.index = i + 1; // 1부터 10까지의 인덱스 설정
        memset(empty_cabinet.file_name, 0, sizeof(empty_cabinet.file_name));
        memset(empty_cabinet.file_description, 0, sizeof(empty_cabinet.file_description));
        memset(empty_cabinet.password, 0, sizeof(empty_cabinet.password));
        write(fd, &empty_cabinet, sizeof(Cabinet));
    }
    close(fd);
}

int is_cabinet_empty(int index) {
    int fd = open(FILENAME, O_RDONLY);
    if (fd < 0) {
        perror("Could not open file for reading");
        exit(EXIT_FAILURE);
    }
    Cabinet cabinet;
    lseek(fd, (index - 1) * sizeof(Cabinet), SEEK_SET);
    read(fd, &cabinet, sizeof(Cabinet));
    close(fd);
    return strlen(cabinet.file_name) == 0;
}

void save_cabinet(Cabinet *cabinet) {
    int fd = open(FILENAME, O_RDWR);
    if (fd < 0) {
        perror("Could not open file for writing");
        exit(EXIT_FAILURE);
    }
    lseek(fd, (cabinet->index - 1) * sizeof(Cabinet), SEEK_SET); // 인덱스 기반 위치 설정
    write(fd, cabinet, sizeof(Cabinet));
    close(fd);
}

Cabinet request_cabinet(int index) {
    int fd = open(FILENAME, O_RDONLY);
    Cabinet cabinet = {0, "", "", ""};
    if (fd < 0) {
        perror("Could not open file for reading");
        exit(EXIT_FAILURE);
    }
    lseek(fd, (index - 1) * sizeof(Cabinet), SEEK_SET); // 인덱스 기반 위치 설정
    read(fd, &cabinet, sizeof(Cabinet));
    close(fd);
    return cabinet;
}

void send_all_cabinets(int sock) {
    int fd = open(FILENAME, O_RDONLY);
    Cabinet cabinet;
    if (fd < 0) {
        perror("Could not open file for reading");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < MAX_CABINETS; i++) {
        lseek(fd, i * sizeof(Cabinet), SEEK_SET); // 인덱스 기반 위치 설정
        read(fd, &cabinet, sizeof(Cabinet));
        send(sock, &cabinet, sizeof(Cabinet), 0);
    }
    close(fd);
}

void save_file(int sock, const char *file_name, size_t file_size) {
    char buffer[1024];
    FILE *file = fopen(file_name, "wb");
    if (!file) {
        perror("Could not open file to save");
        return;
    }

    size_t bytes_received = 0;
    while (bytes_received < file_size) {
        int bytes = recv(sock, buffer, sizeof(buffer), 0);
        if (bytes <= 0) {
            break;
        }
        fwrite(buffer, 1, bytes, file);
        bytes_received += bytes;
    }

    fclose(file);
    if (bytes_received == file_size) {
        printf("File saved: %s (size: %zu bytes)\n", file_name, file_size);
    } else {
        printf("File transfer incomplete: %s (expected: %zu bytes, received: %zu bytes)\n", file_name, file_size, bytes_received);
    }
}

void send_file(int sock, const char *file_name) {
    FILE *file = fopen(file_name, "rb");
    if (!file) {
        perror("Could not open file to read");
        return;
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (send(sock, buffer, bytes_read, 0) < 0) {
            perror("send failed");
            fclose(file);
            return;
        }
    }

    fclose(file);
    printf("File sent: %s (size: %zu bytes)\n", file_name, file_size);
}

void *handle_client(void *socket_desc) {
    int sock = *(int*)socket_desc;
    ClientRequest client_request;
    int response;

    while (recv(sock, &client_request, sizeof(ClientRequest), 0) > 0) {
        switch (client_request.request_type) {
            case SAVE_CABINET:
                if (is_cabinet_empty(client_request.cabinet.index)) {
                    strncpy(client_request.cabinet.password, client_request.password, sizeof(client_request.cabinet.password) - 1);
                    save_cabinet(&client_request.cabinet);
                    printf("Cabinet saved:\n");
                    printf("Index: %d\n", client_request.cabinet.index);
                    printf("File Name: %s\n", client_request.cabinet.file_name);
                    printf("File Description: %s\n", client_request.cabinet.file_description);
                    save_file(sock, client_request.cabinet.file_name, client_request.file_size);
                    response = 1; // 성공
                } else {
                    printf("Cabinet index %d is not empty. Update denied.\n", client_request.cabinet.index);
                    response = 0; // 실패
                }
                send(sock, &response, sizeof(response), 0); // 클라이언트에 성공/실패 알림
                break;
            case REQUEST_CABINET:
                {
                    Cabinet cabinet = request_cabinet(client_request.cabinet.index);
                    send(sock, &cabinet, sizeof(Cabinet), 0);
                }
                break;
            case REQUEST_ALL_CABINETS:
                send_all_cabinets(sock);
                break;
            case DOWNLOAD_FILE:
                {
                    Cabinet cabinet = request_cabinet(client_request.cabinet.index);
                    if (strcmp(cabinet.password, client_request.password) == 0) {
                        response = 1; // 비밀번호 일치
                        send(sock, &response, sizeof(response), 0);
                        send_file(sock, cabinet.file_name);
                    } else {
                        response = 0; // 비밀번호 불일치
                        send(sock, &response, sizeof(response), 0);
                    }
                }
                break;
            default:
                printf("Unknown request type.\n");
                break;
        }
    }

    close(sock);
    free(socket_desc);
    return 0;
}

int main() {
    int server_fd, new_socket, *new_sock;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    FILE *file = fopen(FILENAME, "rb");
    if (!file) {
        initialize_file();
    } else {
        fclose(file);
    }

    while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))) {
        printf("Connection accepted\n");

        pthread_t sniffer_thread;
        new_sock = malloc(sizeof(int));
        *new_sock = new_socket;

        if (pthread_create(&sniffer_thread, NULL, handle_client, (void*)new_sock) < 0) {
            perror("could not create thread");
            free(new_sock);
            return 1;
        }

        printf("Handler assigned\n");
    }

    if (new_socket < 0) {
        perror("accept failed");
        close(server_fd);
        return 1;
    }

    close(server_fd);
    return 0;
}
