#ifndef DRIVER_H
#define DRIVER_H

#include "stdlib.h"
#include "stdarg.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "nwy_test_cli_adpt.h"



#define DBG_LTE_TAG        "\r\n[NET] "
#define DBG_ENABLE
#define MAX_STR_LEN         64

#define SIM_PRESENCE        01



typedef enum {
    FC_NONE = 0,  // None Flow Control
    FC_RTSCTS,    // Hardware Flow Control (rtscts)
    FC_XONXOFF    // Software Flow Control (xon/xoff)
}UART_FlowCtrl_E;

typedef enum
{
    PORT_DEBUG = 0,
    PORT_B2B,
} UART_List_E;

typedef enum
{
    TZ_UTC = 0,
    TZ_IST = 22, // IST (UTC +5:30) 
}TIMEZONE_E;

typedef struct
{
    int  ota_flag;
    char host[MAX_STR_LEN];
    char port[MAX_STR_LEN];          // Port number
    char username[MAX_STR_LEN];
    char password[MAX_STR_LEN];
    char filename[MAX_STR_LEN];
} OTA_Server_Params_tt;
typedef struct
{
    char server_ip[64];
    uint16_t port;
    char endpoint[128];
    bool valid;
} server_cfg_tt;

typedef struct
{
    OTA_Server_Params_tt ota_params;
    server_cfg_tt        server;
} flash_params_tt;


void Init_Peripheral_Drivers(void);

/* Debug UART */
void Init_Debug_UART(void);
void UART_Debug_Send(uint8_t *send_data, uint16_t send_len);

/* General Purpose Timer */
void Init_Usr_Timer(void);
void Usr_Timer_Callback(void);

/* RTC */
void RTC_Date_Time_Set(nwy_time_t *time, TIMEZONE_E timezone);
void RTC_Date_Time_Get(nwy_time_t *Curr_time, TIMEZONE_E tz);

/* ADC */
uint16_t Read_CP_ADC(nwy_adc_e adc_ch, nwy_adc_aux_scale_e adc_ref);

/* SIM  */
uint8_t Read_SIM_Presence(void);
void SiM_Init(void);
void Sim_Detect_Callback(nwy_sim_id_e sim_id, nwy_sim_urc_type_e urc_type, nwy_sim_info_ind_t *ind_struct);


void odt_vts_dbgPrintf(const char *fmt, ...);




/* External variables */
extern void GPIO_Set_Output(uint8_t gpio_num, uint8_t value);

extern char  nwy_test_file_name[];
extern flash_params_tt flash_paramss;
extern uint16_t sim_index;
extern bool     sms_receive_flag;
extern uint16_t HeartBeat_counter; 

#endif /* DRIVER_H */