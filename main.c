#include <stdio.h>
#include <pthread.h>
#include "FrameStreamer.h"
#include "aieventhandler.h"
#include "tcp_listener.h"
#include "logger.h"

void *camera_loop(void *arg) {
    init_v4l2(1920, 1080);
    uint8_t buffer[1920 * 1080 * 2];

    while (1) {
        int ret = getStreaming(buffer);
        if (ret != 1) {
            printf("Streaming failed");
        }
        sleep(1); // 1 FPS
    }

    return NULL;
}

int main() {
    pthread_t cam_tid, tcp_tid;

    pthread_create(&cam_tid, NULL, camera_loop, NULL);
    pthread_create(&tcp_tid, NULL, (void *(*)(void *))start_tcp_listener, NULL);

    pthread_join(cam_tid, NULL);
    pthread_join(tcp_tid, NULL);

    return 0;
}


