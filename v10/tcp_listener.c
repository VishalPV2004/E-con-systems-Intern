/*
Copyright © 2025, e-con Systems India Private Limited. All rights reserved.
This file contains proprietary information; they are provided under a license agreement containing restrictions on use
and disclosure and are also protected by copyright, patent, and other intellectual and industrial property laws. Please
refer the licensing agreement with e-con Systems to understand the restrictions. Reverse engineering, disassembly, or
decompilation of the Programs is prohibited. Except as may be expressly permitted in your license agreement for these
Programs, no part of these Programs may be reproduced or transmitted in any form or by any means, electronic or
mechanical, for any purpose. The Programs are not intended for use in any inherently dangerous applications. It shall be
the licensee's responsibility to take all appropriate fail-safe, backup, redundancy and other measures to ensure the
safe use of such applications if the Programs are used for such purposes, and we disclaim liability for any damages
caused by such use of the Programs.

Description :
    TCP listener module that acts as a server to transmit JPEG image frames on demand.
    - Waits for client connection on port 8080.
    - Accepts a frame ID from the client.
    - Responds with the corresponding image file if it exists.
    - Sends the file size first (4 bytes), followed by the image data.
    - Handles invalid requests (underflow/overflow/frame not found).
    - Ensures clean shutdown of socket connections.
*/


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

int server_fd = -1;               

// Start TCP server to send JPEG frames
int start_tcp_listener() {
    struct sockaddr_in address, client_address;
    int opt = 1;
    socklen_t addrlen = sizeof(client_address);
    char buffer[ONE_KB];

    logMessage("Creating socket...");
    server_fd = socket(AF_INET, SOCK_STREAM, 0);  // Create socket
    if (server_fd < 0) {
        logMessage("Socket creation failed");
        return FAILURE;
    }

    // Set socket options
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // Bind socket to port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        logMessage("Bind failed");
        close(server_fd);
        return FAILURE;
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        logMessage("Listen failed");
        close(server_fd);
        return FAILURE;
    }

    logMessage("Listening on port 8080...");

    while (1) {
        // Accept client connection
        int client_fd = accept(server_fd, (struct sockaddr*)&client_address, &addrlen);
        if (client_fd < 0) {
            logMessage("Accept failed");
            continue;
        }

        logMessage("Client connected.");
        printf("Client connected.\n");


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

            // Validate frame ID
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
                    // File not found
                    send(client_fd, "", 0, 0);
                    logMessage("File not found: %s", filename);
                } else {
                    // Send image data
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
                    printf("Image frame%02d.jpeg sent successfully.", frame_id);
                }
            }
        }

        // Clean up connection
        shutdown(client_fd, SHUT_RDWR);
        close(client_fd);
        logMessage("Connection closed, returned to listening...");
    }

    printf("Deniniting port\n");
  
    close(server_fd);
    return SUCCESS;
}

