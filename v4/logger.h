#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

extern FILE *logger;       
extern int logNumber;      

int32_t initializeLogger();
int32_t logMessage(const char *message);
int32_t cleanupLogger();

#endif

