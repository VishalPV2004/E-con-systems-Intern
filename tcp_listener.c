#include "tcp_listener.h"
#include "logger.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void start_tcp_listener() {
    int server_fd;
    struct sockaddr_in address, client_address;
    int opt = 1;
    socklen_t addrlen = sizeof(client_address);
    char buffer[1024];

    printf("Creating socket...");
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        return;
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        return;
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        return;
    }

    printf("Listening on port 8080...");

    while (1) {
        int client_fd = accept(server_fd, (struct sockaddr*)&client_address, &addrlen);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }

        printf("Client connected.");

        memset(buffer, 0, sizeof(buffer));
        int valread = read(client_fd, buffer, sizeof(buffer) - 1);
        if (valread > 0) {
            buffer[valread] = '\0';

            printf("Received request from client.");

            FILE *fp = fopen("/root/frame.jpeg", "rb");
            if (fp == NULL) {
                printf("File open failed.");
                close(client_fd);
                continue;
            }

            char file_buf[1024];
            int bytes_read;

            while ((bytes_read = fread(file_buf, 1, sizeof(file_buf), fp)) > 0) {
                int total_sent = 0;
                while (total_sent < bytes_read) {
                    int sent = send(client_fd, file_buf + total_sent, bytes_read - total_sent, 0);
                    if (sent < 0) {
                        perror("Send failed");
                        break;
                    }
                    total_sent += sent;
                }
            }

            fclose(fp);
            printf("Image sent successfully.");
            sleep(1);
        } else {
            printf("Client disconnected or read failed.");
        }

        shutdown(client_fd, SHUT_RDWR);
        close(client_fd);
        printf("Connection closed, returned to listening...");
        sleep(1);
    }

    close(server_fd);
}
