#ifndef HTTP_H
#define HTTP_H


#include "stdlib.h"
#include "stdarg.h"
#include "string.h"
#include "stdio.h"
#include "nwy_test_cli_adpt.h"
#include "nwy_http_api.h"


#define SERVER_HOST     "172.235.29.67"
#define SERVER_PORT      8787
#define ENDPOINT         "/Outdid/VTS/"

#define HTTP_RECONNECT_MAX_COUNT 5


#define HTTP_RX_LEN     4096
#define HTTP_TX_LEN     1024

typedef struct
{
    uint8_t Rx_buf[HTTP_RX_LEN];
    uint8_t tx_buf[HTTP_TX_LEN];
    uint16_t offset;
}HTTP_Handler_S;

typedef struct
{
    uint8_t host[50];
    uint16_t port;
    char endpoint[128];
}HTTP_Config_S;

uint8_t HTTP_Setup(void);
int Parse_IOT_Server_Params(char *ptr);
void flash_write(void);

extern HTTP_Handler_S HttpS ;
extern nwy_http_handle_t http_hdl1;
extern HTTP_Config_S Http_Config;

#endif // HTTP_H