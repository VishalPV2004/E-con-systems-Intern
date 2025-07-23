/*This file will */
#pragma once
#ifndef LOGGER_H
#define LOGGER_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#define MAX_LOG_SIZE 262144 // 256 KB
#define LOG_FILE_COUNT 2
#define LOG_DIRECTORY "/App/Govcomm/"

int32_t initializeLogger();
int32_t logMessage(const char *message, ...);
int32_t cleanupLogger();


#endif 
