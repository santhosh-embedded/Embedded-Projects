#ifndef OTA_H
#define OTA_H

#include <stdint.h>
#include <stdbool.h>
#include "nwy_ftp_api.h"
#include "nwy_fota_api.h"


#define NWY_EXT_SIO_PER_LE 1024

#define FTP_HOST            "192.185.129.79"
#define FTP_PORT             21
#define FTP_USERNAME        "dhanush@outdidunified.com"
#define FTP_PASSWORD        "Dhanush@2024"
#define FTP_FILENAME        "nwy_open_app_fota.pkt"


// typedef struct
// {
//     uint8_t *data;
//     int len;
//     int offset;
//     int total_size;
// } ota_pack_t;


typedef struct
{
    uint8_t host[64];
    uint16_t port;          // Port number
    uint8_t username[64];
    uint8_t password[64];
    uint8_t filename[64];

} OTA_Server_Params_S;

// extern ota_pack_t g_ota_pack1;
extern char echoo_buff[NWY_EXT_SIO_PER_LE + 1];
extern nwy_ftp_handle_t g_ftp_fota_handl1;
extern nwy_ftp_get_param_t g_ftp_get_param1;
void ota_init(void);
// void ota_start_from_json(uint8_t *IotServerParams);
void ota_start_from_json();


extern OTA_Server_Params_S Ftp_config;


#endif
