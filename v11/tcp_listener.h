#ifndef TCP_LISTENER_H
#define TCP_LISTENER_H

#define ONE_KB 1024
#define BYTES_256 256
#define MSG_BUFFER 64
#define BUFFER_WIDTH 1920
#define BUFFER_HEIGHT 1080
#define PATH_BUFFER 512
#define FILENAME_BUFFER 64
#define FRAME_RATE 120

extern int server_fd;             
int start_tcp_listener();

#endif

