#ifndef NETWORK_H
#define NETWORK_H

#include "nwy_sim_api.h"
#include "stdint.h"
#include "stdbool.h"

typedef enum
{
    APP_INIT = 0,
    APP_FLASH_READ,
    APP_WAIT_SIM,
    APP_WAIT_NETWORK,
    APP_PDP_ACTIVATE,
    APP_GET_IMEI,
    APP_SMS_INIT,
    APP_GET_VERSION,
    APP_HTTP_SETUP,
    APP_HTTP_WAIT_CONNECT,
    APP_HTTP_RUNNING,
    APP_HTTP_CLOSE,
    OTA_INIT,
    OTA_WAIT,
} APP_State_t;



void Network_Polling(void);
nwy_sim_status_e GET_SIM_Status(void);
uint8_t Set_Network_Reg(void);

extern APP_State_t VTS_State;
extern uint8_t http_reconnect_count;




#endif // NETWORK_H