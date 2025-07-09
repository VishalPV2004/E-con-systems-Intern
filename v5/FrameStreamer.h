#ifndef FRAME_STREAMER_H
#define FRAME_STREAMER_H

#include <stdint.h>
#include "CameraSdk.h"

#define BUFFER_WIDTH 1920
#define BUFFER_HEIGHT 1080
#define PATH_BUFFER 512
#define FILENAME_BUFFER 64
#define FRAME_RATE 50

// Global v4l2 data structure used across source files
extern struct v4l2_app_data v4l2data;
extern pthread_t ai_event_tid;

// Function declarations
int de_init();
int init_v4l2(int width, int height);
int getStreaming(uint8_t *buffer);
int process_thread();
#endif // FRAME_STREAMER_H



