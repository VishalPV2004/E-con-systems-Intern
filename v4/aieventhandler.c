#include "aieventhandler.h"
#include "logger.h"

int sockfd; // Socket file descriptor
struct sockaddr_in servaddr; // Server address structure
DETECTION co_ord_data;
PACKET receive_packet;
// Function to get the value for a given key string
// to be removed if command id is implemented in Cloud.
bool is_model_file_present;
bool is_this_thread_exit;
uint16_t is_model_updated_flag;
char ai_model_file_name[255];
bool ai_event_flag = true;
bool post_AI_Data_ToCloud_flag = true;


void epochToString(time_t epoch_time, char *time_str) {
    // Convert epoch time to string representation
    sprintf(time_str, "%ld", epoch_time);
}

void vehicle_AIStats(int count, char *vehicle_type){
    cJSON *root = cJSON_CreateObject();
    char * macAddress;
    /*if (getMacAddress(&macAddress) == SUCCESS) {
        // Add MAC address to the cJSON object
        cJSON_AddStringToObject(root, "serialnumber", macAddress);
        // Free dynamically allocated memory
        free(macAddress);
    }*/
    // Epoch time
    char time_str[20];
    time_t current_time;
    current_time = time(NULL);
    // Convert epoch time to string representation
    // Assuming 20 characters would be enough for the string representation
    epochToString(current_time, time_str);
    cJSON_AddStringToObject(root, "dateofcollection",time_str);

    // Convert the integer count to a string
    char countStr[12]; // Enough to hold all int values including negative values and '\0'
    snprintf(countStr, sizeof(countStr), "%d", count);

    // Add the string representation of count to the JSON object
    cJSON_AddStringToObject(root, "numberofvehicle", countStr);
    
    cJSON_AddStringToObject(root, "typeofvehicle", vehicle_type);

    char *json_str = cJSON_Print(root);
    char *json_buffer = malloc((strlen(json_str) + 1) * sizeof(char));
    strcpy(json_buffer, json_str);
    printf("JSON String: %s\n", json_buffer);
    //int status = rest_add_vehicle_count(&json_buffer);
    //printf("vehicle_AIStats status = %d\n", status);
     // Free memory
    cJSON_Delete(root);
    free(json_str);
}

void face_AIStats(int count){
    
    cJSON *root = cJSON_CreateObject();
    char *serialnumber;
    /*if (getMacAddress(&serialnumber) == SUCCESS) {
        // Add MAC address to the cJSON object
        cJSON_AddStringToObject(root, "serialnumber", serialnumber);
        // Free dynamically allocated memory
        free(serialnumber);
    }*/
    // Current Epoch time
    char time_str[20];
    time_t current_time;
    current_time = time(NULL);
    // Convert epoch time to string representation
    // Assuming 20 characters would be enough for the string representation
    epochToString(current_time, time_str);
    cJSON_AddStringToObject(root, "dateofcollection",time_str);
    printf("Count = %d\t", count);
    // Convert integer to string
    char face_count_str[12];  // Allocate sufficient space for the integer string
    snprintf(face_count_str, sizeof(face_count_str), "%d", count);
    cJSON_AddStringToObject(root, "numberoffacesdeteced", face_count_str);

    char *json_str = cJSON_Print(root);

    char *json_buffer = malloc((strlen(json_str) + 1) * sizeof(char));
  
    strcpy(json_buffer, json_str);
   
    printf("JSON String: %s\n", json_buffer);
 
    //int status = rest_add_face_count(&json_buffer);
   
    //printf("face_AIStats status = %d\n", status);
    
     // Free memory
    cJSON_Delete(root);
    free(json_str);
}

void init_socket(char *IP) {
    // Socket creation
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        logMessage("Socket creation failed...");
    } else {
        logMessage("Socket successfully created..");
    }

    // Initialize server address structure
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(IP);
    servaddr.sin_port = htons(8100);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        logMessage("Connection with the server failed...");
    } else {
        logMessage("Connected to the server..");
    }
}

void closefd(){
    if(sockfd!=-1){
        close(sockfd);
        logMessage("Socket closed Successfully");
    }
}

// Function to check for the presence of the model file
bool checkModelFile() {
    // Implement your logic to check for the model file
    if(getCurrentModelFileFromBoard() ==true){
        sleep(3);
        // init_socket(LOCALHOST);
        return true;
    }
    return false; // Returning true for demonstration
}
bool getCurrentModelFileFromBoard() {
    FILE *file;
    // char buffer[100]; // Assuming the value in the file is less than 100 characters
    bool rtstatus = false;
    // Open the file for reading
    file = fopen("/tmp/model/model_name", "r");
    if (file == NULL) {
        // printf("No model Running in the Application!\n");
        // strcpy(out_file_name,NOMODELFILEFOUND);
        rtstatus = false;
    }else{
        // fgets(buffer, sizeof(buffer), file);
        // strcpy(out_file_name,buffer);
        // printf("getCurrentModelFileFromBoard Model Name: %s\n", buffer);
        rtstatus = true;
    }
    return rtstatus;
}

// Function to simulate posting AI data to cloud
void postAIDataToCloud() {
    logMessage("Inside postAIDataToCloud");

        int model_file = 3;
        
        read(sockfd, &receive_packet, sizeof(PACKET));
        memcpy(&co_ord_data, receive_packet.message, sizeof(DETECTION));
        printf("Model file: %d\t ", model_file);

        // printf("Face count: %d", co_ord_data.objects.cnt);
        //if((co_ord_data.objects.cnt > 0) && (co_ord_data.objects.cnt < 255))
        //{
            printf("\n");
            logMessage("Object Found");
            printf("co_ord_data.objects.cnt = %d\t", co_ord_data.objects.cnt);
            //face_AIStats(co_ord_data.objects.cnt); 
        //}   
        sleep(1);
    post_AI_Data_ToCloud_flag = false;
}
// Function to update inference data based on received packets - Padmini

void *handleAIModel() 
{
        logMessage("Handling AI model!!!");
        bool ai_model_flag = true;
        while(ai_model_flag)
        {
                logMessage("Running!");
                postAIDataToCloud();
                // Sleep for some time before posting the next data
                // usleep(1000000); // 1 second
                sleep(1);
        }
        ai_model_flag = false;
    return NULL;
}



