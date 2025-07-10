#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include "cJSON.h"

typedef enum {
    SUCCESS = 0,
    FAILURE = -1,
    INVALID_PARAM = -2,
    NOT_SUPPORTED = -3,
}result;


typedef struct {
    char camSerialNumber[16];
    char uniqId[16];
    char ip[16];
    char subnetmask[16];
    char gateway[16];
    char macaddress[18];
} stCameraConfig;






