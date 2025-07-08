#include "cloudctrl.h"
#include "cJSON.h"

uint8_t command[255];
int handleGetRequest(){
   printf("Inside handleGetRequest\n");
   // rest_get_config();
   printf("After calling rest_get_config API");
}

int handleStatusUpdate(){
   printf("Inside handleStatusUpdate\n");
   // rest_status_update();
   printf("After calling rest_status_update API");
}


void custom_strcpy(uint8_t *command, const uint8_t *src) {
    // Iterate through the source string until the null terminator is encountered
    while (*src != '\0') {
        *command = *src;   
        src++;          
        command++;         
    }
    *command = '\0';
}


char* handleGetConfig(const char* serial_number, char** getconfig){
    printf("Inside handleGetConfig++\n");
    char *config_data;
    int status = rest_get_config(serial_number,&config_data);
    if (status == 0){
        if(config_data !=NULL) {
            printf("handleGetConfig Response: %s\n", config_data);
            *getconfig = strdup(config_data);
            free(config_data);
        }
    }
    printf("Inside handleGetConfig--\n");
}

char* handleStreaming(){
    printf("Inside handleStreaming++\n");
    char *response = rest_get_streaming();
    if(response !=NULL) {
        printf("Response: %s\n", response);
    }
    printf("Inside handleGetConfig--\n");
}

char* handleHeartBeat(char** json_data){
    printf("handleHeartBeat\n");
    // printf("json_data -> %p\n", (void *)*json_data);
    char *response;
    // char* response=rest_heart_beat(buffer);
    int status = rest_heart_beat(json_data,&response);

    // free(json_data);
    if (status == CURL_FAILURE){
        custom_strcpy(command,NO_DATA_FOUND);
        return command;
    }else {
        if (response != NULL) {
            printf("handleHeartBeat Response: %s\n", response);
            cJSON *root = cJSON_Parse(response);
            if (root != NULL) {
                cJSON *item = cJSON_GetArrayItem(root, 0);
                if (item != NULL) {
                    cJSON *changecategory = cJSON_GetObjectItemCaseSensitive(item, "changecategory");
                    if (cJSON_IsString(changecategory) && (changecategory->valuestring != NULL)) {
                        printf("Change category: %s\n", changecategory->valuestring);
                        custom_strcpy(command,changecategory->valuestring);
                        printf("command_value: %s\n", command);
                    }else{
                        printf("Error accessing changecategory.\n");
                    }
                }else{
                    printf("Error accessing array item.\n");
                    cJSON_Delete(root);
                    return 1;
                }
            }          
            cJSON_Delete(root);
            } else {
            printf("Error parsing JSON\n");
            }
        free(response); 
        return command;// Free memory allocated for response
    } 
}   
