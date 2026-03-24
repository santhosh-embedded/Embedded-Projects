#ifndef VTS_APP_H
#define VTS_APP_H

#include "stdlib.h"
#include "stdarg.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"


// #define     VEHICLE_ID      "TK103-002"
#define     FW_VERSION      "1.2.6"
#define     VBAT_GAIN       0.0736196319



typedef enum
{
    StartNavigation = 0,
    Bootnotification,
    Statusnotification,
    HeartBeat,
    Locationnotification,
    Max,
    Idle,
}FrameType_E;

typedef enum
{
    NoFix = 0,
    Valid,
    BadFix,
    // Invalid,
}FixStatus_E;

typedef enum
{
    GPS = 0,
    BD,
    GPS_BD,
    GPS_GLONASS,
    GPS_GALILEO,
    BD_GLONASS,
    BD_GALILEO,
    GLONASS_GALILEO,
    GLONASS,
    GALILEO,
    GPS_BD_GLONASS,
    GPS_BD_GALILEO,
    GPS_GLONASS_GALILEO,
    BD_GLONASS_GALILEO,
    GPS_BD_GLONASS_GALILEO,
}NavigMode_E;



typedef struct
{
    uint8_t Fix_Status[15];

}Status_Param_S;

typedef struct
{
    float latitude;
    float longitude;
    float speed; 
    float course;
    float altitude;
    float hdop;
    uint8_t timestamp[30];
    uint8_t fix_quality[15];
    uint32_t CID;
    int RSRP;
    uint8_t satellites;
    uint16_t MCC;
    uint16_t MNC;
}Location_Param_S;

typedef struct 
{
    volatile uint8_t Boot_Send          : 1;
    volatile uint8_t Heartbeat_Send     : 1;
    volatile uint8_t Location_Send      : 1;
    volatile uint8_t tx_Flag            : 1;
    volatile uint8_t OTA_req            : 1;
}VTS_Flags_S;


void VTS_App_Polling(void);

extern VTS_Flags_S     VTS_flags ;
extern FrameType_E     SendMsg;
extern FixStatus_E     Prev_Fixstate;
extern uint32_t        HeartBeat_interval;
extern uint8_t         Device_ID[]; 
extern Location_Param_S Loc_param;
extern float           Batt_Volt;
extern  void Get_Location(void);
extern  void Get_Battery_Volt(void);
#endif // VTS_APP_H