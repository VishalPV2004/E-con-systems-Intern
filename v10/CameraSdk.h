#ifndef CAMERA_SDK_H
#define CAMERA_SDK_H
 
#include <stdint.h>
#include "v4l2_capture.h"
#include <ctype.h>
#include <arpa/inet.h>

 
// Return value enums
typedef enum {
    CAMERA_SUCCESS = 0,
    CAMERA_ERROR = -1,
    CAMERA_INVALID_PARAM = -2,
    CAMERA_NOT_SUPPORTED = -3,
    MODEL_ERROR = -4,
    INVALID_FIRMWARE = -5,
} CameraReturnCode;
 
// Structure for camera configuration
typedef struct {
    char camSerialNumber[16];
    char uniqId[16];
    char ip[16];
    char subnetmask[16];
    char gateway[16];
    char macaddress[18];
} CameraConfig;

#pragma pack(1)
struct dhcp_mode{
        short int dhcp_set;
        char ip_addr[20];
        char netmask[20];
        char gateway[20];
};

#pragma pack(1)
typedef struct set_video_params_{
        char fmt[20];
        short int width;
        short int height;
        short int port;
        short int fps;
}SET_VIDEO_PARAMS;

#pragma pack(1)
typedef struct rtsp_credentials{
        char user[30];
        char pass[30];
}RTSP_CRED;


#define NPU_START_BYTE 224
#define NPU_END_BYTE 227


// Function declarations

CameraReturnCode getCameraConfig(CameraConfig *config);

//GET's 
CameraReturnCode getBackLightCompensation(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
CameraReturnCode getBrightness(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
CameraReturnCode getContrast(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
CameraReturnCode getColorSaturation(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
CameraReturnCode getSharpness(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
CameraReturnCode getExposure(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
CameraReturnCode getGain(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
CameraReturnCode getAutoExposure(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
CameraReturnCode getManualExposure(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
CameraReturnCode getWhiteBalance(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
CameraReturnCode getHDRMode(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
CameraReturnCode getBGain(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
CameraReturnCode getRGain(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
CameraReturnCode getMinExposureTime(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
CameraReturnCode getMaxExposureTime(int16_t *cur_val, int16_t *min_val, int16_t *max_val);

//SET's
CameraReturnCode setBackLightCompensation(int16_t control_val);
CameraReturnCode setBrightness(int16_t control_val);
CameraReturnCode setContrast(int16_t control_val);
CameraReturnCode setColorSaturation(int16_t control_val);
CameraReturnCode setSharpness(int16_t control_val);
CameraReturnCode setExposure(int16_t control_val);
CameraReturnCode setGain(int16_t control_val);
CameraReturnCode setAutoExposure(int16_t control_val);
CameraReturnCode setManualExposure(int16_t control_val);
CameraReturnCode setWhiteBalance(int16_t control_val);
CameraReturnCode setHDRMode(int16_t control_val);
CameraReturnCode setBGain(int16_t control_val);
CameraReturnCode setRGain(int16_t control_val);
CameraReturnCode setMinExposureTime(int16_t control_val);
CameraReturnCode setMaxExposureTime(int16_t control_val);
 
 
//Additional Controls
CameraReturnCode getAdditionalControlsState(uint8_t *ControlsOnOff);
CameraReturnCode setAdditionalControlsState(uint8_t ControlsOnOff);
CameraReturnCode getGOPSize(uint8_t *GOPSize);
CameraReturnCode setGOPSize(char* GOPSize);
CameraReturnCode getIFrame(uint8_t *IFrame);
CameraReturnCode setIFrame(char* IFrame);
CameraReturnCode getMinQP(uint8_t *MinQP);
CameraReturnCode setMinQP(char* MinQP);
CameraReturnCode getMaxQP(uint8_t *MaxQP);
CameraReturnCode setMaxQP(char* MaxQP);
CameraReturnCode getH264CodecProfile(uint8_t *CodecProfile);
CameraReturnCode setH264CodecProfile(char* CodecProfile);
CameraReturnCode getRCtype(uint8_t *RCtype);
CameraReturnCode setRCtype(char* RCtype);
CameraReturnCode getBitrate(int *Bitrate);
CameraReturnCode setBitrate(char* Bitrate);
CameraReturnCode getNTPServerList(char **NTPServerList);
CameraReturnCode selectNTPServer(char *NTPServer);
CameraReturnCode getVideoFormat(uint8_t *VideoFormat);
CameraReturnCode setVideoFormat(uint8_t VideoFormat);
CameraReturnCode getVideoResolutions(uint16_t **ResolutionsList);
CameraReturnCode setVideoResolutions(uint16_t *Resolution);
CameraReturnCode getVideoFPS(uint8_t *FPS);
CameraReturnCode setVideoFPS(uint8_t FPS);

//AI Model APIs
CameraReturnCode ValidateAIModel(uint8_t* FilePath);
CameraReturnCode FlashAIModel(uint8_t* FilePath);

//Firmware Flashing API
CameraReturnCode ValidateFirmware(uint8_t* FilePath);
CameraReturnCode FlashFirmware(uint8_t* FilePath);

//Performance API
CameraReturnCode getPerformanceInfo(uint16_t* cpu_used, uint16_t* memory_used, uint16_t* gpu_used);

//Streaming API
CameraReturnCode startStreaming(uint8_t* frameBuffer, struct v4l2_app_data *v4l2_data, struct v4l2_buffer *v4l2_buf_o, int *frame_size, int *frame_id);

//Streaming Controls APIs
CameraReturnCode setStreamingControls(char *format, int width, int height, int fps);

//Network APIs
CameraReturnCode dhcp_mode_select(struct dhcp_mode *select_dhcp_mode);
CameraReturnCode get_dhcp_mode(struct dhcp_mode *select_dhcp_mode);
CameraReturnCode ntp_server_set(char *ntp_server);
CameraReturnCode get_ntp_server(char *ntp_server);

CameraReturnCode init_clovis_setup();
CameraReturnCode setRTSPcredentials();


#endif /* CAMERA_SDK_H */
