#include <stdio.h>
#include <pthread.h>
#include "FrameStreamer.h"
#include "aieventhandler.h"
#include "tcp_listener.h"
#include "logger.h"
#include "status.h"

bool continue_cap_img = true;

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
        sleep(1); // 1 FPS
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

    pthread_t cam_tid, tcp_tid;

    if (pthread_create(&cam_tid, NULL, camera_loop, NULL) != 0) {
        logMessage("Failed to create camera thread");
        cleanupLogger();
        return FAILURE;
    }

    if (pthread_create(&tcp_tid, NULL, (void *(*)(void *))start_tcp_listener, NULL) != 0) {
        logMessage("Failed to create TCP listener thread");
        continue_cap_img = false;
        pthread_join(cam_tid, NULL);
        cleanupLogger();
        return FAILURE;
    }

    pthread_join(cam_tid, NULL);
    pthread_join(tcp_tid, NULL);

    logMessage("Threads exited, cleaning up");
    cleanupLogger();

    return SUCCESS;
}

