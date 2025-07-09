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

// Starts the TCP listener to serve JPEG frames on client request
int start_tcp_listener() {
    int server_fd;
    struct sockaddr_in address, client_address;
    int opt = 1;
    socklen_t addrlen = sizeof(client_address);
    char buffer[ONE_KB];

    printf("Creating socket...\n");
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        return FAILURE;
    }

    // Allow reuse of port and address
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    // Configure server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // Bind the socket to the specified IP and port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        return FAILURE;
    }

    // Start listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        return FAILURE;
    }

    printf("Listening on port 8080...\n");

    while (1) {
        // Accept a new client connection
        int client_fd = accept(server_fd, (struct sockaddr*)&client_address, &addrlen);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }

        printf("Client connected.\n");

        while (1) {
            memset(buffer, 0, sizeof(buffer));
            int valread = read(client_fd, buffer, sizeof(buffer) - 1);
            if (valread <= 0) {
                printf("Client disconnected or read failed.\n");
                break;
            }

            buffer[valread] = '\0';
            printf("Received frame ID from client: %s\n", buffer);

            int frame_id = atoi(buffer);

            // Handle underflow and overflow conditions
            if (frame_id < 1) {
                const char *msg = "Underflow\n";
                send(client_fd, msg, strlen(msg), 0);
                printf("Underflow request.\n");
            } else if (frame_id > 50) {
                const char *msg = "Overflow\n";
                send(client_fd, msg, strlen(msg), 0);
                printf("Overflow request.\n");
            } else {
                // Construct frame file path
                char filename[BYTES_256];
                snprintf(filename, sizeof(filename), "/root/frame%02d.jpeg", frame_id);

                FILE *fp = fopen(filename, "rb");
                if (fp == NULL) {
                    // File does not exist
                    char msg[MSG_BUFFER];
                    snprintf(msg, sizeof(msg), "Frame not found with frame id : %02d\n", frame_id);
                    send(client_fd, msg, strlen(msg), 0);
                    printf("File not found: %s\n", filename);
                } else {
                    // Read and send file contents in chunks
                    char file_buf[ONE_KB];
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
                    printf("Image frame%02d.jpeg sent successfully.\n", frame_id);
                }
            }
        }

        // Clean up client connection
        shutdown(client_fd, SHUT_RDWR);
        close(client_fd);
        printf("Connection closed, returned to listening...\n");
    }

    close(server_fd);
    return SUCCESS;
}

