#ifndef CLOUD_CTRL_H
#define CLOUD_CTRL_H
//#include "common.h"
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include <time.h>
#include <ctype.h>
#include<stdbool.h>
#include <signal.h>
// Checksum
#define STR_VALUE(val) #val
#define STR(name) STR_VALUE(name)
#define PATH_LEN 256
#define MD5_LEN 32

#define BUFFER_SIZE 1024
#define SECONDS 30
#define MILLI_SECONDS 1000

#define CURL_FAILURE -1
#define REGISTER_SUCCESS 0
#define REGISTER_FAILED -1
#define REGISTER_ALREADY -2
#define LOCALHOST "127.0.0.1"
#define NO_DATA_FOUND "No data were found"
#define SUCCESS_STATUS "Success"
#define PROGRESS_STATUS "In-Progress"
#define CONFIGSETTINGS 1
#define FIRMWARE 2
#define INITIATEGETFRAMES 3
#define STOPGETFRAMES 4
#define AI_MODEL_NO_DATA "nodata"
#define FACE_BLUR_FILE "face_blur_package.zip"
#define PERSON_INTRUSION_FILE "person_intrusion_package.zip"
#define VEHICLE_DETECTION_FILE "vehicledet_package.zip"
#define VEHICLE_DISTANCE_FILE "vehicle_distancedet_package.zip"

#define FACE_BLUR_CHECKSUM "38b15cf9f8dbcf61c3ba3496c7282d40"
#define PERSON_INTRUSION_CHECKSUM "b56471e803d2a1205643a75b17026ed2"
#define VEHICLE_DETECTION_CHECKSUM "82744b98e8586148bd9b4a7148764d6a"
#define VEHICLE_DISTANCE_CHECKSUM "8e0d4e97be912c658d266ee28895e13b"
//result handlePostRequest(uint8_t commandId); // Thread 
// to be removed if command id is implemented in Cloud.->getValueForKeyString
uint8_t getValueForKeyString(const char* key);
int handleGetRequest();
int handleResposeRequest();
char* handleHeartBeat(char** json_data);
char* handleGetConfig(const char* serial_number, char** getconfig);
void cameraSettings(char** config_data);
void statusUpdate(const char *changeStatus);
int verifyCheckSum(char *file_name, char *md5_sum);
#endif 
