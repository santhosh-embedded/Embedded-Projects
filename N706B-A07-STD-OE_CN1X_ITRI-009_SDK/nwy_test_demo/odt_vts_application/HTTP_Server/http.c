#include "http.h"
#include "Network.h"
#include "VTS_App.h"
#include "nwy_adapt_platform.h"
#include "nwy_file_api.h"
#include "driver.h"


nwy_http_handle_t http_hdl1 = NULL;
HTTP_Handler_S HttpS        = {0};
HTTP_Config_S Http_Config   = {0};

static void Http_Rx_Data_Process(uint8_t *rx_data)
{
    odt_vts_dbgPrintf("[HTTP] Recv Data : %s\r\n", rx_data);

    if(strstr(rx_data, "BootNotification"))
    {
        char* currentTimeStr = NULL;
        char* currentTimeKey = "\"timestamp\":\"";
        currentTimeStr = strstr(rx_data, currentTimeKey);
        if (currentTimeStr) 
        {
            currentTimeStr += strlen(currentTimeKey);
            char* endQuote = strchr(currentTimeStr, '\"');
            if (endQuote) 
            {
                *endQuote = '\0';
                nwy_time_t dt = {0};
                parse_datetime(currentTimeStr, &dt);
                RTC_Date_Time_Set(&dt, TZ_UTC);

                *endQuote = '\"';

            }
        }
        if(strstr(rx_data,"\"status\":\"Accepted\""))
        {
            char *interval_ptr = strstr(rx_data, "\"interval\":");
            if (interval_ptr) 
            {
                interval_ptr += strlen("\"interval\":");
                HeartBeat_interval = atoi(interval_ptr);
                HeartBeat_interval /= 1000;
                // SendMsg = Statusnotification;
                Prev_Fixstate = -1;
                odt_vts_dbgPrintf("Heartbeat Interval : %d\r\n", HeartBeat_interval);  
            }
        }
    }

    else if(strstr(rx_data, "StatusNotification"))
    {

    }

    else if(strstr(rx_data, "LocationUpdate"))
    {

    }

    if (strstr(HttpS.Rx_buf, "\"OTA\":\"1\""))
    {
        // int parsed_data = Parse_IOT_Server_Params(HttpS.Rx_buf);
        // if(parsed_data == 0)
        {
            // flash_write();
            // Flash_Read();
            odt_vts_dbgPrintf("[OTA] OTA REQUEST RECEIVED FROM SERVER\r\n");
            VTS_flags.OTA_req = true;
            return;
        }
        // else
        // {
            // odt_vts_dbgPrintf("[OTA] PARAMETER SETUP FAILED\r\n");
        // }
    }

 VTS_flags.tx_Flag = false;

}

static void Http_Recv_Handler(nwy_http_result_t *response)
{
        odt_vts_dbgPrintf("[HTTP] Recv Len : %d\r\n", response->data_len);

        memcpy(HttpS.Rx_buf+HttpS.offset, response->data, response->data_len);
    
        if(response->data_len == 1501)
        {
            HttpS.offset += response->data_len;
            return;
        } 

        if( response->data_len != 1051)
        {
            // odt_vts_dbgPrintf("[HTTP] Recv Data : %s\r\n", HttpS.Rx_buf); // Enable to check http header

            HttpS.offset = 0;
            if (strstr(HttpS.Rx_buf, "HTTP/1.1 200 OK")) 
            {
                Http_Rx_Data_Process(strchr(HttpS.Rx_buf, '{'));          
            }
            memset(HttpS.Rx_buf, 0, sizeof(HttpS.Rx_buf));
        }
        
}

static void HTTP_Event_CallBack(nwy_http_result_t *resp)
{
    odt_vts_dbgPrintf("HTTPS Event Callback Triggered : %d\r\n",resp->event);
    if(!resp) return;

    switch(resp->event)
    {
        case NWY_HTTP_OPEN_FAIL:
        // GPIO_Set_Output(70, 0);
             odt_vts_dbgPrintf("HTTPS  Connect Failed\r\n");
            //   VTS_State = APP_WAIT_SIM;
            http_reconnect_count--;
             VTS_State = APP_HTTP_SETUP;
             break;
             
        case NWY_HTTP_OPENED:
             odt_vts_dbgPrintf("HTTPS Connect Success\r\n");
             GPIO_Set_Output(70, 1);
             VTS_State = APP_HTTP_RUNNING;
             SendMsg = StartNavigation;

             break;

        case NWY_HTTP_DATA_SEND_FINISHED:
             HeartBeat_counter = 0;
             odt_vts_dbgPrintf("HTTPS Post Success\r\n");
             break;

        case NWY_HTTP_DATA_RECVED:
            Http_Recv_Handler(resp);
            break;

        case NWY_HTTP_CLOSED_PASV:
            GPIO_Set_Output(70, 0);
           odt_vts_dbgPrintf("HTTPS Disconnected\r\n");
           odt_vts_dbgPrintf("HTTPS Reconnecting...\r\n"); 
           VTS_State = APP_HTTP_SETUP; 
           http_reconnect_count = HTTP_RECONNECT_MAX_COUNT;
           break;
        //    VTS_State = APP_WAIT_SIM;

        default:
            break;
    }

}
uint8_t HTTP_Setup(void)
{
    nwy_http_param_t param = {0};
    nwy_ssl_conf_t sslcfg;
    param.host =  Http_Config.host;//SERVER_HOST;//flash_params.server.server_ip;
    param.port =  Http_Config.port;//SERVER_PORT;//flash_params.server.port;
    param.cid = 1;
    param.timeout_s = 60;
    param.cb = HTTP_Event_CallBack;
    
    odt_vts_dbgPrintf("HTTP Host : Port ---> %s : %d\r\n",Http_Config.host,Http_Config.port);

    http_hdl1 = nwy_http_setup(&param, NULL);
    if(http_hdl1)
    {
        odt_vts_dbgPrintf("HTTPS Setup: Success");
        // GPIO_Set_Output(70, 1);
        return 1;
    }
    else
    {
        odt_vts_dbgPrintf("HTTPS Setup: Fail");
        http_reconnect_count--;
        return 0;
        // GPIO_Set_Output(70, 0);
    }
}

void HTTP_Close(void)
{
    nwy_http_close(http_hdl1);
    // if(ota_request)
    // {
    //     Switch_State = OTA_INIT;
    // }
    // if(http_connected == 0)
    // {
    //     Switch_State =  APP_HTTP_SETUP;
    // }
    // else{
    //     Switch_State =  APP_HTTP_SETUP;
    // }
}  


/**********
 * Parse the IOT Server Parameters from the received HTTP response
 * Expected format in the HTTP response body:
 */
int Parse_IOT_Server_Params(char *ptr)
    {
        odt_vts_dbgPrintf("ENTER INTO THE PARSE STATE\r\n");
        char *ota_ptr = strstr(ptr, "\"OTA\"");
        if (ota_ptr == NULL)
        {
            odt_vts_dbgPrintf("OTA field not found\n");
            return -1;
        }

        if (sscanf(ota_ptr, "\"OTA\":\"%d\"", &flash_paramss.ota_params.ota_flag) != 1)
        {
            odt_vts_dbgPrintf("OTA extract failed\n");
            return -2;
        }
        char *fwurl_ptr = strstr(ptr, "\"FWURL\"");
        if (fwurl_ptr == NULL)
            {
                printf("FWURL field not found\n");
                return -1;
            }
        char url[256] = {0};
        if(sscanf(fwurl_ptr, "\"FWURL\":\"%[^\"]\"", url)!=1)
            {
                printf("FWURL extract failed\n");
                return -2;
            }
        char *start = strstr(url, "://");
        if (start) start += 3;
        else start = url;
        int parsed = sscanf(start,
                            "%[^:]:%[^:]:%[^:]:%[^:]:%s",
                            flash_paramss.ota_params.host, flash_paramss.ota_params.port, flash_paramss.ota_params.username, flash_paramss.ota_params.password, flash_paramss.ota_params.filename);
        if (parsed != 5)
            {
                printf("ERROR: Parsing failed. parsed=%d\n", parsed);
                return -1;
            }
        odt_vts_dbgPrintf("OTA_FLAG  : %d\n", flash_paramss.ota_params.ota_flag);    
        odt_vts_dbgPrintf("Host      : %s\n", flash_paramss.ota_params.host);
        odt_vts_dbgPrintf("Port      : %s\n", flash_paramss.ota_params.port);
        odt_vts_dbgPrintf("Username  : %s\n", flash_paramss.ota_params.username);
        odt_vts_dbgPrintf("Password  : %s\n", flash_paramss.ota_params.password);
        odt_vts_dbgPrintf("Filename  : %s\n", flash_paramss.ota_params.filename);
        return 0;
    }


void 
parse_datetime(const char* datetime, nwy_time_t* dt)
{
    char temp[5];

        // Parse year
        strncpy(temp, datetime, 4);
        temp[4] = '\0';
        dt->year = (uint16_t)atoi(temp);

        // Parse month
        strncpy(temp, datetime + 5, 2);
        temp[2] = '\0';
        dt->mon = (uint8_t)atoi(temp);

        // Parse day
        strncpy(temp, datetime + 8, 2);
        temp[2] = '\0';
        dt->day = (uint8_t)atoi(temp);

        // Parse hour
        strncpy(temp, datetime + 11, 2);
        temp[2] = '\0';
        dt->hour = (uint8_t)atoi(temp);

        // Parse minute
        strncpy(temp, datetime + 14, 2);
        temp[2] = '\0';
        dt->min = (uint8_t)atoi(temp);

        // Parse second
        strncpy(temp, datetime + 17, 2);
        temp[2] = '\0';
        dt->sec = (uint8_t)atoi(temp);
}