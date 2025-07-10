#include <stdio.h>
#include <pthread.h>
#include "FrameStreamer.h"
#include "aieventhandler.h"
#include "tcp_listener.h"
#include "logger.h"

bool continue_cap_img = true;
// Thread function to continuously capture frames from the camera
void *camera_loop(void *arg) {
    init_v4l2(BUFFER_WIDTH, BUFFER_HEIGHT); // initialize camera capture
    uint8_t buffer[BUFFER_WIDTH * BUFFER_HEIGHT * 2];

    while (continue_cap_img) {
        getStreaming(buffer); // store streaming frame, assume always successful
        sleep(1); // capture at 1 FPS
    }

    return NULL;
}

int main() {
    pthread_t cam_tid, tcp_tid;



    // Create thread to handle camera capture loop
    pthread_create(&cam_tid, NULL, camera_loop, NULL);

    // Create thread to handle TCP socket listener
    pthread_create(&tcp_tid, NULL, (void *(*)(void *))start_tcp_listener, NULL);

    // Wait for both threads to complete
    pthread_join(cam_tid, NULL);
    pthread_join(tcp_tid, NULL);



    return SUCCESS;
}


