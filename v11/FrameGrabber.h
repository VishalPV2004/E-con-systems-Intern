#ifndef FRAME_GRABBER_H
#define FRAME_GRABBER_H

#include <stdint.h>
#include "CameraSdk.h"

#define BUFFER_WIDTH 1920
#define BUFFER_HEIGHT 1080
#define PATH_BUFFER 512
#define FILENAME_BUFFER 64
#define FRAME_RATE 120

extern char filenames[FRAME_RATE][FILENAME_BUFFER];
extern int frame_id[FRAME_RATE];
extern uint8_t frames[FRAME_RATE][BUFFER_WIDTH * BUFFER_HEIGHT * 2];

// Global v4l2 data structure used across source files
extern struct v4l2_app_data v4l2data;
extern pthread_t ai_event_tid;

// Function declarations
int de_init();
int init_v4l2(int width, int height);
int getStreaming(uint8_t *buffer);




#endif // FRAME_STREAMER_H

