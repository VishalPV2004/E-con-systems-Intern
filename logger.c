#include "logger.h"

FILE *logger = NULL;
int logNumber = 1;

int32_t initializeLogger() {
    FILE *fp = fopen("log_number.txt", "r");
    if (fp != NULL) {
        fscanf(fp, "%d", &logNumber);
        fclose(fp);
    }

    char filename[20];
    sprintf(filename, "log_%d.txt", logNumber);
    logger = fopen(filename, "w");
    if (logger == NULL) {
        return -1;
    }

    fprintf(logger, "Logging started...\n");
    return EXIT_SUCCESS;
}

int32_t logMessage(const char *message) {
    if (logger == NULL) return -1;

    fprintf(logger, "[%d] %s\n", logNumber++, message);
    fflush(logger);
    return EXIT_SUCCESS;
}

int32_t cleanupLogger() {
    if (logger != NULL) {
        fclose(logger);
        logger = NULL;
    }

    FILE *fp = fopen("log_number.txt", "w");
    if (fp != NULL) {
        fprintf(fp, "%d", logNumber);
        fclose(fp);
    }

    return EXIT_SUCCESS;
}
