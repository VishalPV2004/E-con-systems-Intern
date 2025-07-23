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
#include "FrameGrabber.h"

extern int frame_id[FRAME_RATE];
extern uint8_t frames[FRAME_RATE][BUFFER_WIDTH * BUFFER_HEIGHT * 2];
int frame_size = BUFFER_HEIGHT * BUFFER_WIDTH * 2;
int server_fd = -1;

int start_tcp_listener() {
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

            int requested_id = atoi(buffer);

            if (requested_id < 1) {
                const char *msg = "Underflow\n";
                send(client_fd, msg, strlen(msg), 0);
                logMessage("Underflow request.");
            } else if ((unsigned int)requested_id > 4294967295U) {
                const char *msg = "Overflow\n";
                send(client_fd, msg, strlen(msg), 0);
                logMessage("Overflow request.");
            } else {
                int found = 0;
                for (int i = 0; i < FRAME_RATE; i++) {
                    if (frame_id[i] == requested_id) {
                        int total_sent = 0;
                        while (total_sent < frame_size) {
                            int sent = send(client_fd, frames[i] + total_sent, frame_size - total_sent, 0);
                            if (sent < 0) {
                                logMessage("Send failed during frame ID %d", requested_id);
                                break;
                            }
                            total_sent += sent;
                        }
                        logMessage("Frame ID %d sent", requested_id);
                        printf("Frame ID %d sent\n", requested_id);
                        found = 1;
                        break;
                    }
                }

                if (!found) {
                    send(client_fd, "", 0, 0);
                    logMessage("Frame ID %d not found", requested_id);
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
