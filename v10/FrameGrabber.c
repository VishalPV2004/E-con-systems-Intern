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

Description:
    Handles V4L2 camera capture and JPEG frame generation.
    - Initializes and de-initializes V4L2 resources.
    - Captures frames from camera using V4L2 streaming.
    - Stores latest FRAME_RATE number of JPEG images in a ring buffer.
    - Provides JPEG file path and frame ID for TCP transmission or post-processing.
*/



#include "FrameGrabber.h"
#include "CameraSdk.h"
#include "status.h"
#include "logger.h"
#include "status.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Global control for processing thread
bool continue_thread = true;

// Stores most recent FRAME_RATE frames
//frameSlot FRAMES[FRAME_RATE];

char filenames[FRAME_RATE][FILENAME_BUFFER] = {0};
int frame_id[FRAME_RATE] = {0};


// Global V4L2 configuration structure
struct v4l2_app_data v4l2data;

// Initialize V4L2 capture with specified resolution
int init_v4l2(int width, int height) {
    logMessage("Initializing V4L2 capture with width=%d, height=%d", width, height);
    int ret = v4l2_init_capture(&v4l2data, width, height);  
    logMessage("Capture init return value = %d\n", ret);
    printf("Capture init return value = %d", ret);
    return (ret == 0) ? SUCCESS : FAILURE;
}

// De-initialize V4L2 resources
int de_init() {
    v4l2_deinit_capture(&v4l2data);
    logMessage("V4L2 de-initialized successfully");
    return SUCCESS;
}

// Capture and save a single frame, update buffer with captured data
int getStreaming(uint8_t *buffer) {
    static int frame_count = 0;

    struct v4l2_buffer v4l2_buf_o;
    uint8_t frameBuffer[BUFFER_WIDTH * BUFFER_HEIGHT * 2];
    int frame_size, inference_frame_id;

    // Capture frame using V4L2
    int ret = startStreaming(frameBuffer, &v4l2data, &v4l2_buf_o, &frame_size, &inference_frame_id);
    logMessage("Frame ID: %d", inference_frame_id);
    if (ret != 0) {
        logMessage("Failed to capture frame");
        return FAILURE;
    }

    logMessage("Frame captured successfully");

    // Copy captured frame data to output buffer
    if (frame_size <= BUFFER_WIDTH * BUFFER_HEIGHT * 2) {
        memcpy(buffer, frameBuffer, frame_size);
    }

    // Use ring buffer approach to overwrite oldest frame slot
    int index = frame_count % FRAME_RATE;
    //frameSlot *slot = &FRAMES[index];
    char *old_name = filenames[index];
    int *old_id = &frame_id[index];

    // Remove previously saved frame file, if any
    if (old_name[0] != '\0') {
        remove(old_name);
        logMessage("Removed old file: %s", old_name);
    }

    // Create new filename and write JPEG image
    snprintf(old_name, FILENAME_BUFFER, "frame%05d.jpeg", inference_frame_id);
    FILE *fp = fopen(old_name, "wb");
    if (fp == NULL) {
        logMessage("Failed to open file: %s", old_name);
        return FAILURE;
    }

    fwrite(frameBuffer, 1, frame_size, fp);
    fclose(fp);

    logMessage("Frame written to file: %s", old_name);
    printf("Captured and saved: %s\n", old_name);

    // Update slot metadata
    *old_id = inference_frame_id;
    frame_count++;

    return SUCCESS;
}
