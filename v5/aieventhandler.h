#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "cloudctrl.h"
#include "common.h"
#include "cJSON.h"
#include <time.h>
#define MAX_DETECT_BOX  256
#define NOMODELFILEFOUND "noFileFound"
#define UNKNOWN_FLASH_STATUS 0
#define MODEL_FLASH_INPROGRESS 1
#define MODEL_FLASH_COMPLETED 2
#define AI_MODEL_NAME_BUFFER 255
#define TIME_BUFFER 20
#define COUNT_BUFFER 12

#pragma pack(1)


typedef struct {
    int16_t x_min;
    int16_t x_max;
    int16_t y_min;
    int16_t y_max;
    int16_t npu_class;
    int16_t npu_score;
} obj_t;


#pragma pack(1)
typedef struct {
    int cnt;
    obj_t obj[MAX_DETECT_BOX];
} objs_t;

#pragma pack(1)
typedef struct detection{
        int frame_num;
        objs_t objects;

}DETECTION;

#pragma pack(1)
typedef struct packet{
        char command;
        int length;
        char message[4000];
}PACKET;

int init_socket(char *IP);
int closefd();
int vehicle_AIStats(int count, char *vehicle_type);
int postAIDataToCloud();
void *handleAIModel();
bool getCurrentModelFileFromBoard();
int epochToString(time_t epoch_time, char *time_str);
int checkModelFile();
