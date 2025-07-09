#include "aieventhandler.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdbool.h>

/*
 -1 -> failure
  0 -> success 
*/

int sockfd;
struct sockaddr_in servaddr;
DETECTION co_ord_data;
PACKET receive_packet;

bool is_model_file_present;
bool is_this_thread_exit;
uint16_t is_model_updated_flag;
char ai_model_file_name[AI_MODEL_NAME_BUFFER];
bool ai_event_flag = true;
bool post_AI_Data_ToCloud_flag = true;

// Converts epoch time to a string format
int epochToString(time_t epoch_time, char *time_str) {
    if (!time_str) return FAILURE;
    sprintf(time_str, "%ld", epoch_time);
    return SUCCESS;
}

// Sends vehicle count stats with type to cloud
int vehicle_AIStats(int count, char *vehicle_type) {
    if (!vehicle_type) return -1;

    cJSON *root = cJSON_CreateObject();

    char time_str[TIME_BUFFER];
    epochToString(time(NULL), time_str);
    cJSON_AddStringToObject(root, "dateofcollection", time_str);

    char countStr[COUNT_BUFFER];
    snprintf(countStr, sizeof(countStr), "%d", count);
    cJSON_AddStringToObject(root, "numberofvehicle", countStr);
    cJSON_AddStringToObject(root, "typeofvehicle", vehicle_type);

    char *json_str = cJSON_Print(root);
    char *json_buffer = malloc(strlen(json_str) + 1);
    if (!json_buffer) return FAILURE;

    strcpy(json_buffer, json_str);
    printf("JSON String: %s\n", json_buffer);

    free(json_str);
    cJSON_Delete(root);
    free(json_buffer);

    return SUCCESS;
}

// Sends face detection count stats to cloud
int face_AIStats(int count) {
    cJSON *root = cJSON_CreateObject();

    char time_str[TIME_BUFFER];
    epochToString(time(NULL), time_str);
    cJSON_AddStringToObject(root, "dateofcollection", time_str);

    char face_count_str[COUNT_BUFFER];
    snprintf(face_count_str, sizeof(face_count_str), "%d", count);
    cJSON_AddStringToObject(root, "numberoffacesdeteced", face_count_str);

    char *json_str = cJSON_Print(root);
    char *json_buffer = malloc(strlen(json_str) + 1);
    if (!json_buffer) return FAILURE;

    strcpy(json_buffer, json_str);
    printf("JSON String: %s\n", json_buffer);

    free(json_str);
    cJSON_Delete(root);
    free(json_buffer);

    return SUCCESS;
}

// Initializes TCP socket and connects to specified IP
int init_socket(char *IP) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed...");
        return FAILURE;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(IP);
    servaddr.sin_port = htons(8100);

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        perror("Connection with the server failed...");
        return FAILURE;
    }

    return SUCCESS;
}

// Closes the TCP socket if open
int closefd() {
    if (sockfd != -1) {
        close(sockfd);
        sockfd = -1;
    }
    return SUCCESS;
}

// Checks whether model file is present (blocking check)
int checkModelFile() {
    if (getCurrentModelFileFromBoard()) {
        sleep(3);
        return 1;
    }
    return SUCCESS;
}

// Checks if model file exists on filesystem
bool getCurrentModelFileFromBoard() {
    FILE *file = fopen("/tmp/model/model_name", "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

// Receives AI detection packet and prints the result
int postAIDataToCloud() {
    int model_file = 3;

    if (read(sockfd, &receive_packet, sizeof(PACKET)) <= 0) {
        perror("Socket read error");
        return FAILURE;
    }

    memcpy(&co_ord_data, receive_packet.message, sizeof(DETECTION));

    printf("Model file: %d\t", model_file);
    printf("\nObject Found\n");
    printf("co_ord_data.objects.cnt = %d\t", co_ord_data.objects.cnt);

    sleep(1);
    post_AI_Data_ToCloud_flag = false;

    return SUCCESS;
}

// Handles AI processing loop (meant to run in a thread)
void *handleAIModel() {
    bool ai_model_flag = true;

    while (ai_model_flag) {
        postAIDataToCloud();
        sleep(1);
    }

    return NULL;
}

