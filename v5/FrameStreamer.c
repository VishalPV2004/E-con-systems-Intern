#include "FrameStreamer.h"
#include "CameraSdk.h"
#include "aieventhandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "status.h"

bool continue_thread = true;

struct v4l2_app_data v4l2data;

// Initializes the V4L2 camera with the specified width and height.
int init_v4l2(int width, int height) {
    printf("Capture init return value\n");
    int ret = v4l2_init_capture(&v4l2data, width, height);  
    printf("Capture init return value = %d\n", ret);
    if (ret != 1) {
        return FAILURE;  // Initialization failed
    }
    return SUCCESS;  // Success
}

// Deinitializes the V4L2 camera context and releases resources.
int de_init() {
    v4l2_deinit_capture(&v4l2data);
    printf("Deinit completed\n");
    return SUCCESS;
}

// Thread for any background processing like cloud upload (placeholder).
int process_thread() {
    printf("Running thread continuously\n");

    char path[PATH_BUFFER] = {0};
    FILE *fp = popen("find / -name frame.jpeg 2>/dev/null", "r");
    if (fp == NULL) {
        perror("popen failed");
        return FAILURE;
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

    while (continue_thread) {
        printf("Thread running\n");
        postAIDataToCloud();  // Custom cloud upload logic
        sleep(1);
    }

    return SUCCESS;
}

// Captures a single frame and saves it to frameXX.jpeg (rolling buffer style).
int getStreaming(uint8_t *buffer) {
    static int frame_id = 1;

    struct v4l2_buffer v4l2_buf_o;
    uint8_t frameBuffer[BUFFER_WIDTH * BUFFER_HEIGHT * 2];
    int frame_size;

    int ret = startStreaming(frameBuffer, &v4l2data, &v4l2_buf_o, &frame_size);
    printf("Frame id : %d",v4l2_buf_o.sequence);
    if (ret != 0) {
        printf("Failed to capture frame\n");
        return FAILURE;
    }

    printf("Frame capture success! ");

    if (frame_size <= BUFFER_WIDTH * BUFFER_HEIGHT * 2) {
        memcpy(buffer, frameBuffer, frame_size);
    }

    char filename[FILENAME_BUFFER];
    snprintf(filename, sizeof(filename), "frame%02d.jpeg", frame_id);

    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("File not opened\n");
        return FAILURE;
    }

    printf("File created! Writing to %s...\n", filename);
    fwrite(frameBuffer, 1, frame_size, fp);
    fclose(fp);

    frame_id = (frame_id % FRAME_RATE) + 1;

    return SUCCESS;
}

