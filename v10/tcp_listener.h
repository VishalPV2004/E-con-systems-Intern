#ifndef TCP_LISTENER_H
#define TCP_LISTENER_H

#define ONE_KB 1024
#define BYTES_256 256
#define MSG_BUFFER 64

extern int server_fd;             
int start_tcp_listener();

#endif

