#include "FrameStreamer.h"
#include "CameraSdk.h"
#include "aieventhandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define BUFFER_WIDTH 1920
#define BUFFER_HEIGHT 1080

struct v4l2_app_data v4l2data;

// Initializes the V4L2 camera with the specified width and height.
int init_v4l2(int width, int height){
    printf("Capture init return value\n");
    int ret = v4l2_init_capture(&v4l2data, width, height);  
    printf("Capture init return value = %d\n", ret);
    if (ret != 1){
        return 0;  // Initialization failed
    }
    return 1;  // Success
}

// Deinitializes the V4L2 camera context and releases resources.
void de_init(){ 
    v4l2_deinit_capture(&v4l2data);  // Cleanup the V4L2 context
    printf("Deinit completed\n");
}



void process_thread()
{
    printf("Running thread continiously\n");

    char path[512] = {0};
    FILE *fp = popen("find / -name frame.jpeg 2>/dev/null", "r");
    if (fp == NULL) {
        perror("popen failed");
        return;
    }

    if (fgets(path, sizeof(path), fp) != NULL) {
        size_t len = strlen(path);
        if (len > 0 && path[len - 1] == '\n') {
            path[len - 1] = '\0';
        }
        printf("Found image path: %s\n", path);
    } else {
        printf("Image not found\n");
    }

    pclose(fp);

    bool thread_running = true;
    while(thread_running)
    {
        printf("Thread running\n");
        postAIDataToCloud();
        sleep(1);
    }
    thread_running = false;
}



// Captures a single frame from the camera and writes it to a file named 'frame.jpeg'.
// The captured frame is also copied into the user-provided buffer.
int getStreaming(uint8_t *buffer){
    static int frame_id = 1;

    struct v4l2_buffer v4l2_buf_o;
    uint8_t frameBuffer[BUFFER_WIDTH * BUFFER_HEIGHT * 2];
    int frame_size;

    int ret = startStreaming(frameBuffer, &v4l2data, &v4l2_buf_o, &frame_size);
    if (ret != 0) {
        printf("Failed to capture frame");
        return 0;
    }

    printf("Frame capture success! ");

    if (frame_size <= BUFFER_WIDTH * BUFFER_HEIGHT * 2) {
        memcpy(buffer, frameBuffer, frame_size);
    }

    char filename[64];
    snprintf(filename, sizeof(filename), "frame%02d.jpeg", frame_id);

    FILE *fp = fopen(filename, "wb");
    if (fp == NULL)
        printf("File not opened");
    else {
        printf("File created! Writing to %s...\n", filename);
        fwrite(frameBuffer, 1, frame_size, fp);
        fclose(fp);
    }

    frame_id = (frame_id % 50) + 1;

    return 1;
}

