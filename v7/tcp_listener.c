#include "tcp_listener.h"
#include "logger.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include "status.h"
#include <stdlib.h>

// Starts the TCP listener to serve JPEG frames on client request
int start_tcp_listener() {
    int server_fd;
    struct sockaddr_in address, client_address;
    int opt = 1;
    socklen_t addrlen = sizeof(client_address);
    char buffer[ONE_KB];

    logMessage("Creating socket...");
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        logMessage("Socket creation failed");
        return FAILURE;
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        logMessage("Bind failed");
        close(server_fd);
        return FAILURE;
    }

    if (listen(server_fd, 3) < 0) {
        logMessage("Listen failed");
        close(server_fd);
        return FAILURE;
    }

    logMessage("Listening on port 8080...");

    while (1) {
        int client_fd = accept(server_fd, (struct sockaddr*)&client_address, &addrlen);
        if (client_fd < 0) {
            logMessage("Accept failed");
            continue;
        }

        logMessage("Client connected.");

        while (1) {
            memset(buffer, 0, sizeof(buffer));
            int valread = read(client_fd, buffer, sizeof(buffer) - 1);
            if (valread <= 0) {
                logMessage("Client disconnected or read failed.");
                break;
            }

            buffer[valread] = '\0';
            logMessage("Received frame ID from client: %s", buffer);

            int frame_id = atoi(buffer);

            if (frame_id < 1) {
                const char *msg = "Underflow\n";
                send(client_fd, msg, strlen(msg), 0);
                logMessage("Underflow request.");
            } else if (frame_id > 9999999) {
                const char *msg = "Overflow\n";
                send(client_fd, msg, strlen(msg), 0);
                logMessage("Overflow request.");
            } else {
                char filename[BYTES_256];
                snprintf(filename, sizeof(filename), "/root/frame%02d.jpeg", frame_id);

                FILE *fp = fopen(filename, "rb");
                if (fp == NULL) {
                    char msg[MSG_BUFFER];
                    snprintf(msg, sizeof(msg), "Frame not found with frame id : %02d\n", frame_id);
                    send(client_fd, msg, strlen(msg), 0);
                    logMessage("File not found: %s", filename);
                } else {
                    char file_buf[ONE_KB];
                    int bytes_read;

                    while ((bytes_read = fread(file_buf, 1, sizeof(file_buf), fp)) > 0) {
                        int total_sent = 0;
                        while (total_sent < bytes_read) {
                            int sent = send(client_fd, file_buf + total_sent, bytes_read - total_sent, 0);
                            if (sent < 0) {
                                logMessage("Send failed");
                                break;
                            }
                            total_sent += sent;
                        }
                    }

                    fclose(fp);
                    logMessage("Image frame%02d.jpeg sent successfully.", frame_id);
                }
            }
        }

        shutdown(client_fd, SHUT_RDWR);
        close(client_fd);
        logMessage("Connection closed, returned to listening...");
    }

    close(server_fd);
    return SUCCESS;
}

