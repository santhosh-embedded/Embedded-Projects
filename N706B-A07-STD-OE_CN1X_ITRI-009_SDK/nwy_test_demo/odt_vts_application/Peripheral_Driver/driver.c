#include "driver.h"
#include "http.h"
#include "OTA/ota.h"
#include "Network.h"
#include "VTS_App.h"
#include "nwy_gpio_api.h"


/* Special values for timer callback execution mode */
#define USR_TIMER_IN_ISR ((nwy_osi_thread_t *)NULL)


static nwy_osi_timer_t      Usr_Timer_Handle = NULL;
int dbg_uart_hdl;
char *UART_list[] = {
    "URT1",           // Used by B2B port
    "URT2",           // Used by debug port
    "URT3",           // Not Defined  /* Not Working */
};


void Init_Peripheral_Drivers(void)
{
    Init_Debug_UART();
    Init_Usr_Timer();
    // SiM_Init();
}


void Init_Debug_UART(void)
{
    // Initialize debug UART here
    dbg_uart_hdl = nwy_uart_open(UART_list[PORT_DEBUG], 115200, FC_NONE);
    if (dbg_uart_hdl < 0)
        odt_vts_dbgPrintf("Debug uart init error!\r\n"); 
    odt_vts_dbgPrintf("Debug uart init success!\r\n");   
}


void UART_Debug_Send(uint8_t *send_data, uint16_t send_len)
{
    // Send data via debug UART here
    nwy_uart_write(dbg_uart_hdl, send_data, send_len);
}

void RTC_Date_Time_Set(nwy_time_t *time, TIMEZONE_E timezone)
{
    // Set RTC date and time here
    nwy_date_set(time, timezone);
}

void RTC_Date_Time_Get(nwy_time_t *Curr_time, TIMEZONE_E tz)
{
    // Get RTC date and time here
    nwy_date_get(Curr_time, &tz);
}

uint16_t Read_CP_ADC(nwy_adc_e adc_ch, nwy_adc_aux_scale_e adc_ref)
{
    return  nwy_adc_get(adc_ch, adc_ref);
}
void SiM_Init(void)
{
    nwy_sim_detect_set(NWY_SIM_ID_SLOT_1, NWY_SIM_DETECT_TYPE_ENABLE, NWY_SIM_DETECT_MODE_LOW );

     nwy_sim_urc_reg(NWY_SIM_ID_SLOT_1, Sim_Detect_Callback);
}

uint8_t Read_SIM_Presence(void)
{
    return nwy_gpio_value_get(SIM_PRESENCE);
}

void Init_Usr_Timer(void)
{
    /* Initialize timer configuration */
    nwy_timer_para_t timer_config = {0};

    timer_config.thread_hdl = USR_TIMER_IN_ISR;
    timer_config.cb = Usr_Timer_Callback;
    timer_config.cb_para = NULL;
    timer_config.expired_time = 1000; /* 1000 ms */ /* 5 ms is minimum frequency */
    timer_config.type = NWY_TIMER_PERIODIC;

    if (Usr_Timer_Handle == NULL)
    {
        nwy_sdk_timer_create(&Usr_Timer_Handle, &timer_config);
        if (Usr_Timer_Handle == NULL) {
            odt_vts_dbgPrintf("Create timer fail");
            return;
        }
    }

    /* Start the timer */
    if (nwy_sdk_timer_start(Usr_Timer_Handle, &timer_config) != NWY_SUCCESS) {
        odt_vts_dbgPrintf("start timer fail");
    } else {
        odt_vts_dbgPrintf("start timer success\r\n");
    }
}
void GPIO_Set_Output(uint8_t gpio_num, uint8_t value)
{
    nwy_gpio_direction_set(gpio_num, PIN_DIRECTION_OUT);

    if(value)
    {
        nwy_gpio_value_set(gpio_num, PIN_LEVEL_HIGH);
    }
    else
    {
        nwy_gpio_value_set(gpio_num, PIN_LEVEL_LOW);
    }
}

void Get_Default_Config(void)
{
    /* Default HTTP Params*/
    strcpy(Http_Config.host, SERVER_HOST);
    Http_Config.port = SERVER_PORT;
    strcpy(Http_Config.endpoint, ENDPOINT);

    /* Default FTP Params */
    strcpy(Ftp_config.host, FTP_HOST);
    Ftp_config.port = FTP_PORT;
    strcpy(Ftp_config.username, FTP_USERNAME);
    strcpy(Ftp_config.password, FTP_PASSWORD);
    strcpy(Ftp_config.filename, FTP_FILENAME);

}

void odt_vts_dbgPrintf(const char *fmt, ...)
{
#ifdef DBG_ENABLE

    char dbg_buf[HTTP_RX_LEN+512];   // adjust size as needed
    uint16_t dbg_len;
    int offset = 0;

    // Add prefix
    offset = snprintf(dbg_buf, sizeof(dbg_buf), "%s", DBG_LTE_TAG);

    // Append formatted message
    va_list args;
    va_start(args, fmt);
    dbg_len = vsnprintf(dbg_buf + offset, sizeof(dbg_buf) - offset, fmt, args);
    va_end(args);

    // Print final string
    nwy_test_cli_echo("%s", dbg_buf);
    dbg_len += strlen(DBG_LTE_TAG);
    if (dbg_uart_hdl < 0)
    {
        nwy_test_cli_echo("\r\nTest uart failure!\r\n"); 
        return; // UART not opened yet
    }
    else
    {
        UART_Debug_Send(dbg_buf,dbg_len);
    }

#endif    
}


