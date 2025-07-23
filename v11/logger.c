#include "logger.h"

FILE *logger = NULL;
int logNumber = 1;
size_t currentLogSize = 0;

// Function to check if a file exists
int fileExists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

// Function to get the size of a file
size_t getFileSize(const char *filename) {
    struct stat buffer;
    if (stat(filename, &buffer) == 0) {
        return buffer.st_size;
    }
    return 0;
}

// Function to get the modification time of a file
time_t getFileModificationTime(const char *filename) {
    struct stat buffer;
    if (stat(filename, &buffer) == 0) {
        return buffer.st_mtime;
    }
    return 0;
}

// Function to determine the latest log file based on modification time
void determineStartingLogFile() {
    char filename1[100], filename2[100];
    time_t time1 = 0, time2 = 0;

    sprintf(filename1, LOG_DIRECTORY "log1.txt");
    sprintf(filename2, LOG_DIRECTORY "log2.txt");

    if (fileExists(filename1)) {
        time1 = getFileModificationTime(filename1);
    }
    if (fileExists(filename2)) {
        time2 = getFileModificationTime(filename2);
    }

    if (time1 > time2) {
        logNumber = 1;
        currentLogSize = getFileSize(filename1);
    } else {
        logNumber = 2;
        currentLogSize = getFileSize(filename2);
    }
}

// Function to delete a file
void deleteFile(const char *filename) {
    if (remove(filename) != 0) {
        printf("Error deleting file: %s\n", filename);
    }
}

// Function to open the log file
int openLogFile(int logNum) {
    if (logger != NULL) {
        fclose(logger);
        logger = NULL;
    }

    char filename[100];
    sprintf(filename, LOG_DIRECTORY "log%d.txt", logNum);

    // Check if the file exists and its size
    if (fileExists(filename)) {
        currentLogSize = getFileSize(filename);

        // If the file size is below MAX_LOG_SIZE, append; otherwise, delete and start new
        if (currentLogSize < MAX_LOG_SIZE) {
            logger = fopen(filename, "a");
        } else {
            // Delete the old log file
            deleteFile(filename);

            // Open a new log file
            logger = fopen(filename, "w");
            currentLogSize = 0;
        }
    } else {
        // If the file doesn't exist, create it in write mode
        logger = fopen(filename, "w");
        currentLogSize = 0;
    }

    if (logger == NULL) {
        printf("Error creating logger file: %s\n", filename);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// Function to initialize the logger
int initializeLogger() {
    determineStartingLogFile();
    return openLogFile(logNumber);
}

// Function to log messages with timestamp
int logMessage(const char *message, ...) {
    if (logger == NULL) {
        printf("Logger not initialized.\n");
        return EXIT_FAILURE;
    }
    //printf("Logger initialized\n");

    // Get the current time
    time_t rawTime;
    struct tm *timeInfo;
    char timeBuffer[80];

    time(&rawTime);
    timeInfo = localtime(&rawTime);

    // Format the time string
    strftime(timeBuffer, sizeof(timeBuffer), "[%Y-%m-%d %H:%M:%S]", timeInfo);

    // Print the timestamp to the log file
    fprintf(logger, "%s ", timeBuffer);

    va_list args;
    va_start(args, message);

    // Calculate the size of the message
    int messageSize = vfprintf(logger, message, args);
    va_end(args);

    // Add a newline and account for it in the size
    fprintf(logger, "\n");
    fflush(logger); // Force write to file
    currentLogSize += strlen(timeBuffer) + 1 + messageSize + 1;

    // Check if the current log file exceeds the maximum size
    if (currentLogSize >= MAX_LOG_SIZE) {
        // Determine the next log number and delete the old file
        logNumber = (logNumber % LOG_FILE_COUNT) + 1;
        openLogFile(logNumber);
    }

    return EXIT_SUCCESS;
}

// Function to clean up resources
int cleanupLogger() {
    if (logger != NULL) {
        fclose(logger);
        logger = NULL;
    }
    return EXIT_SUCCESS;
}
