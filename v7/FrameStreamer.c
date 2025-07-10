#include "FrameStreamer.h"
#include "CameraSdk.h"
#include "aieventhandler.h"
#include "status.h"
#include "logger.h"
#include "status.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool continue_thread = true;
frameSlot FRAMES[FRAME_RATE];
struct v4l2_app_data v4l2data;

int init_v4l2(int width, int height) {
    logMessage("Initializing V4L2 capture with width=%d, height=%d", width, height);
    int ret = v4l2_init_capture(&v4l2data, width, height);  
    logMessage("Capture init return value = %d\n", ret);
    printf("Capture init return value = %d", ret);
    return (ret == 0) ? SUCCESS : FAILURE;
}

int de_init() {
    v4l2_deinit_capture(&v4l2data);
    logMessage("V4L2 de-initialized successfully");
    return SUCCESS;
}

int process_thread() {
    logMessage("Started background processing thread");

    char path[PATH_BUFFER] = {0};
    FILE *fp = popen("find / -name frame.jpeg 2>/dev/null", "r");
    if (fp == NULL) {
        logMessage("popen failed to execute find command");
        return FAILURE;
    }

    if (fgets(path, sizeof(path), fp) != NULL) {
        size_t len = strlen(path);
        if (len > 0 && path[len - 1] == '\n') {
            path[len - 1] = '\0';
        }
        logMessage("Found image path: %s", path);
    } else {
        logMessage("No image named 'frame.jpeg' found");
    }

    pclose(fp);

    while (continue_thread) {
        logMessage("process_thread is alive");
        postAIDataToCloud();
        sleep(1);
    }

    return SUCCESS;
}

int getStreaming(uint8_t *buffer) {
    static int frame_count = 0;

    struct v4l2_buffer v4l2_buf_o;
    uint8_t frameBuffer[BUFFER_WIDTH * BUFFER_HEIGHT * 2];
    int frame_size, inference_frame_id;

    int ret = startStreaming(frameBuffer, &v4l2data, &v4l2_buf_o, &frame_size, &inference_frame_id);
    logMessage("Frame ID: %d", inference_frame_id);
    if (ret != 0) {
        logMessage("Failed to capture frame");
        return FAILURE;
    }

    logMessage("Frame captured successfully");

    if (frame_size <= BUFFER_WIDTH * BUFFER_HEIGHT * 2) {
        memcpy(buffer, frameBuffer, frame_size);
    }

    int index = frame_count % FRAME_RATE;
    frameSlot *slot = &FRAMES[index];

    if (slot->filename[0] != '\0') {
        remove(slot->filename);
        logMessage("Removed old file: %s", slot->filename);
    }

    snprintf(slot->filename, sizeof(slot->filename), "frame%05d.jpeg", inference_frame_id);
    FILE *fp = fopen(slot->filename, "wb");
    if (fp == NULL) {
        logMessage("Failed to open file: %s", slot->filename);
        return FAILURE;
    }

    fwrite(frameBuffer, 1, frame_size, fp);
    fclose(fp);

    logMessage("Frame written to file: %s", slot->filename);

    slot->frame_id = inference_frame_id;
    frame_count++;

    return SUCCESS;
}

