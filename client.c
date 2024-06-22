// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "cabinet.h"

#define PORT 88845
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

int connect_to_server() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    return sock;
}

void request_all_cabinets() {
    int sock = connect_to_server();
    ClientRequest request;
    request.request_type = REQUEST_ALL_CABINETS;

    if (send(sock, &request, sizeof(ClientRequest), 0) <= 0) {
        perror("send failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    Cabinet cabinet;
    printf("Cabinets Status:\n");
    printf("Index | File Name        | Description\n");
    printf("---------------------------------------\n");
    for (int i = 0; i < MAX_CABINETS; i++) {
        if (recv(sock, &cabinet, sizeof(Cabinet), 0) > 0) {
            printf("%5d | %-16s | %s\n", cabinet.index, cabinet.file_name, cabinet.file_description);
        } else {
            perror("recv failed");
            close(sock);
            exit(EXIT_FAILURE);
        }
    }
    close(sock);
}

Cabinet request_cabinet(int index) {
    int sock = connect_to_server();
    ClientRequest request;
    request.request_type = REQUEST_CABINET;
    request.cabinet.index = index;

    if (send(sock, &request, sizeof(ClientRequest), 0) <= 0) {
        perror("send failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    Cabinet cabinet;
    if (recv(sock, &cabinet, sizeof(Cabinet), 0) <= 0) {
        perror("recv failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    close(sock);
    return cabinet;
}

void update_cabinet_info() {
    int sock = connect_to_server();
    ClientRequest request;
    request.request_type = SAVE_CABINET;

    printf("Enter the index of the cabinet to update (1-10): ");
    if (scanf("%d", &request.cabinet.index) != 1 || request.cabinet.index < 1 || request.cabinet.index > 10) {
        fprintf(stderr, "Invalid index. Please enter a value between 1 and 10.\n");
        close(sock);
        return;
    }

    printf("Enter the file name: ");
    scanf("%s", request.cabinet.file_name);
    printf("Enter the file description: ");
    scanf(" %[^\n]", request.cabinet.file_description);  // Read until newline

    FILE *file = fopen(request.cabinet.file_name, "rb");
    if (!file) {
        perror("Could not open file to read");
        close(sock);
        return;
    }

    fseek(file, 0, SEEK_END);
    request.file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    printf("Enter password for the cabinet: ");
    scanf("%s", request.password);

    if (send(sock, &request, sizeof(ClientRequest), 0) <= 0) {
        perror("send failed");
        fclose(file);
        close(sock);
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (send(sock, buffer, bytes_read, 0) < 0) {
            perror("send failed");
            fclose(file);
            close(sock);
            exit(EXIT_FAILURE);
        }
    }

    fclose(file);

    int response;
    if (recv(sock, &response, sizeof(response), 0) <= 0) {
        perror("recv failed");
        close(sock);
        return;
    }

    if (response == 1) {
        printf("Cabinet information updated and file sent: %s (size: %zu bytes)\n", request.cabinet.file_name, request.file_size);
        if (remove(request.cabinet.file_name) == 0) {
            printf("File %s deleted successfully.\n", request.cabinet.file_name);
        } else {
            perror("Failed to delete the file");
        }
    } else {
        printf("Access denied. The cabinet index is not empty.\n");
    }

    close(sock);
}

void download_file_from_cabinet() {
    int sock = connect_to_server();
    ClientRequest request;
    request.request_type = DOWNLOAD_FILE;

    printf("Enter the index of the cabinet to download (1-10): ");
    if (scanf("%d", &request.cabinet.index) != 1 || request.cabinet.index < 1 || request.cabinet.index > 10) {
        fprintf(stderr, "Invalid index. Please enter a value between 1 and 10.\n");
        close(sock);
        return;
    }

    printf("Enter password for the cabinet: ");
    scanf("%s", request.password);

    if (send(sock, &request, sizeof(ClientRequest), 0) <= 0) {
        perror("send failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    int response;
    if (recv(sock, &response, sizeof(response), 0) <= 0) {
        perror("recv failed");
        close(sock);
        return;
    }

    if (response == 1) {
        Cabinet cabinet = request_cabinet(request.cabinet.index);
        FILE *file = fopen(cabinet.file_name, "wb");
        if (!file) {
            perror("Could not open file to save");
            close(sock);
            return;
        }

        char buffer[1024];
        int bytes_received;
        while ((bytes_received = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
            fwrite(buffer, 1, bytes_received, file);
        }

        fclose(file);
        printf("File downloaded: %s\n", cabinet.file_name);
    } else {
        printf("Access denied. Incorrect password.\n");
    }

    close(sock);
}

int main() {
    int choice;
    while (1) {
        printf("\n1. View all cabinets\n");
        printf("2. Update a cabinet\n");
        printf("3. Download file from cabinet\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            fprintf(stderr, "Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); // Clear invalid input
            continue;
        }

        switch (choice) {
            case 1:
                request_all_cabinets();
                break;
            case 2:
                update_cabinet_info();
                break;
            case 3:
                download_file_from_cabinet();
                break;
            case 4:
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
    return 0;
}
