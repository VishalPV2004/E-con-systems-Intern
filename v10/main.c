#include <stdio.h>
#include <pthread.h>
#include "FrameGrabber.h"
#include "tcp_listener.h"
#include "logger.h"
#include "status.h"
#include <stdbool.h>
#include <signal.h>


bool continue_cap_img = true;

void handle_sigint(int sig) {
    logMessage("SIGINT received, cleaning up...");
    continue_cap_img = false;

    if (server_fd != -1) {
        shutdown(server_fd, SHUT_RDWR);
        close(server_fd);
        server_fd = -1;
        logMessage("Server socket closed");
    }
    bool client_running = false;
    bool listening = false;
    de_init();
    cleanupLogger();
    exit(0);
}

// Thread function to continuously capture frames from the camera
void *camera_loop(void *arg) {
    logMessage("Camera thread started");
    if (init_v4l2(BUFFER_WIDTH, BUFFER_HEIGHT) != SUCCESS) {
        logMessage("Failed to initialize camera");
        return NULL;
    }

    uint8_t buffer[BUFFER_WIDTH * BUFFER_HEIGHT * 2];

    while (continue_cap_img) {
        getStreaming(buffer); // assume it logs internally
        sleep(0.5);
    }

    logMessage("Camera thread exiting");
    return NULL;
}

int main() {

    if (initializeLogger() != SUCCESS) {
        printf("Logger init failed, exiting\n");
        return FAILURE;
    }

    logMessage("Main started, launching threads");
    signal(SIGINT, handle_sigint);


    pthread_t cam_tid, tcp_tid;

     if (pthread_create(&tcp_tid, NULL, (void *(*)(void *))start_tcp_listener, NULL) != 0) 
     {
        logMessage("Failed to create TCP listener thread");
        continue_cap_img = false;
        pthread_join(cam_tid, NULL);
        cleanupLogger();
        return FAILURE;
    }

    if (pthread_create(&cam_tid, NULL, camera_loop, NULL) != 0) {
        logMessage("Failed to create camera thread");
        cleanupLogger();
        return FAILURE;
    }

    pthread_join(cam_tid, NULL);
    pthread_join(tcp_tid, NULL);

    logMessage("Threads exited, cleaning up");
    cleanupLogger();

    return SUCCESS;
}

