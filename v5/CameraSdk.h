#ifndef CAMERA_SDK_H
#define CAMERA_SDK_H
#include "v4l2_capture.h" 
#include <stdint.h>
 
// Return value enums
typedef enum {
    CAMERA_SUCCESS = 0,
    CAMERA_ERROR = -1,
    CAMERA_INVALID_PARAM = -2,
    CAMERA_NOT_SUPPORTED = -3,
    MODEL_ERROR = -4,
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
 
 
// Function declarations

// CameraReturnCode getCameraConfig(CameraConfig *config);

//GET's 
// CameraReturnCode getBackLightCompensation(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
// CameraReturnCode getBrightness(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
// CameraReturnCode getContrast(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
// CameraReturnCode getColorSaturation(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
// CameraReturnCode getSharpness(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
// CameraReturnCode getExposure(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
// CameraReturnCode getGain(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
// CameraReturnCode getAutoExposure(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
// CameraReturnCode getManualExposure(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
// CameraReturnCode getWhiteBalance(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
// CameraReturnCode getHDRMode(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
// CameraReturnCode getBGain(int16_t *cur_val, int16_t *min_val, int16_t *max_val);
// CameraReturnCode getRGain(int16_t *cur_val, int16_t *min_val, int16_t *max_val);

//SET's

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
 
 
//Additional Controls
// CameraReturnCode getAdditionalControlsState(uint8_t *ControlsOnOff);
// CameraReturnCode setAdditionalControlsState(uint8_t ControlsOnOff);
// CameraReturnCode getGOPSize(uint8_t *GOPSize);
// CameraReturnCode setGOPSize(uint8_t GOPSize);
// CameraReturnCode getIFrame(uint8_t *IFrame);
// CameraReturnCode setIFrame(uint8_t IFrame);
// CameraReturnCode getMinQP(uint8_t *MinQP);
// CameraReturnCode setMinQP(uint8_t MinQP);
// CameraReturnCode getMaxQP(uint8_t *MaxQP);
// CameraReturnCode setMaxQP(uint8_t MaxQP);
// CameraReturnCode getH264CodecProfile(uint8_t *CodecProfile);
// CameraReturnCode setH264CodecProfile(uint8_t CodecProfile);
// CameraReturnCode getNTPServerList(char **NTPServerList);
// CameraReturnCode selectNTPServer(char *NTPServer);
// CameraReturnCode getVideoFormat(uint8_t *VideoFormat);
// CameraReturnCode setVideoFormat(uint8_t VideoFormat);
// CameraReturnCode getVideoResolutions(uint16_t **ResolutionsList);
// CameraReturnCode setVideoResolutions(uint16_t *Resolution);
// CameraReturnCode getVideoFPS(uint8_t *FPS);
// CameraReturnCode setVideoFPS(uint8_t FPS);

//AI Model APIs
CameraReturnCode ValidateAIModel(uint8_t* FilePath);
CameraReturnCode FlashAIModel(uint8_t* FilePath);

//Performance API
CameraReturnCode getPerformanceInfo(uint16_t* cpu_used, uint16_t* memory_used, uint16_t* gpu_used);
 
CameraReturnCode startStreaming(uint8_t* frameBuffer, struct v4l2_app_data *v4l2_data, struct v4l2_buffer *v4l2_buf_o, int* frame_size);
#endif /* CAMERA_SDK_H */
