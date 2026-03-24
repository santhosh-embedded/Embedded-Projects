#include "sms.h"
#include "driver.h"
#include "VTS_App.h"
#include "Network.h"
#include "http.h"
#include "nwy_network_api.h"
char received_version[20] = {0};
char *Sms_Cmd_list[] = 
{
    "STARTOTA:<ver>",          //1
    "GET LOCATION",            //2
    "GET SIM STATUS",          //3
    "GET NETWORK",             //4
    "GET VERSION",             //5
    "GET BATTERY VOLTAGE",     //6
    "GET SERVER STATUS",       //7
    "GET SERVER INFO",         //8
    "GET DEVICE ID",           //9 
};

enum
{
    START_OTA = 0,
    GET_LOCATION,
    GET_SIM_STATUS,
    GET_NETWORK,
    GET_VERSION,
    GET_BATTERY_VOLTAGE,
    GET_SERVER_STATUS,
    GET_SERVER_INFO,
    GET_DEVICE_ID
};

#define SMS_CMD_LIST_SIZE (sizeof(Sms_Cmd_list) / sizeof(Sms_Cmd_list[0]))

static int compare_version(const char *v1, const char *v2)
{
    int major1, minor1, patch1;
    int major2, minor2, patch2;

    sscanf(v1, "%d.%d.%d", &major1, &minor1, &patch1);
    sscanf(v2, "%d.%d.%d", &major2, &minor2, &patch2);

    if (major1 != major2)
        return major1 - major2;

    if (minor1 != minor2)
        return minor1 - minor2;

    return patch1 - patch2;
}


void Init_SMS(void)
{
    if(nwy_sms_option_init(NWY_SIM_ID_SLOT_1) == NWY_SUCCESS)
    {
        odt_vts_dbgPrintf("\r\nnwy init sms success!\r\n"); 
    }
    else
    {
         odt_vts_dbgPrintf("\r\nnwy init sms fail!\r\n");
    } 
    /****************CALLBACK FOR THE SMS ************************* */   
    nwy_sms_recv_cb_reg(NWY_SIM_ID_SLOT_1,(nwy_sms_evt_handler)nwy_sms_test_recv_cb);
}

void Read_SMS(void)
{

    nwy_sms_recv_info_type_t g_sms_data = {0};
    int ret = nwy_sms_msg_read(0, sim_index, &g_sms_data) ;
    if(ret == NWY_SUCCESS)
    {
        nwy_test_cli_echo("\r\nSMS Content:\r\n%s\r\n",
                        g_sms_data.msg_content);
        nwy_test_cli_echo("SOURCE MOBILE NUMBER:%s",g_sms_data.source_phone_num);
        // SMS_Recv_Handler(g_sms_data.msg_content);
        nwy_test_cli_echo("before the sms receiver handler\r\n");
        SMS_Recv_Handler(g_sms_data.msg_content,
                 g_sms_data.source_phone_num);
        nwy_sms_msg_del(0,sim_index);
        /* Optional cleanup */
        // nwy_sms_delete(storage, sms_data->sms_info.nIndex); 
    }
    else
    {
        nwy_test_cli_echo("\r\nSMS read failed at index ");
    }
}

void SMS_Recv_Handler(char *sms_resp, char *sender_num)
{
    odt_vts_dbgPrintf("ENTER INTO THE RECEIVE HANDLER STATE\r\n");
    if (strstr(sms_resp, "STARTOTA"))
    {
        // char received_version[20] = {0};

        sscanf(sms_resp, "STARTOTA:%19s", received_version);

        odt_vts_dbgPrintf("[OTA] Received Version: %s\r\n", received_version);
        odt_vts_dbgPrintf("[OTA] Current Version : %s\r\n", FW_VERSION);

        if (compare_version(received_version, FW_VERSION) > 0)
        {
            odt_vts_dbgPrintf("[OTA] Newer version detected. Starting OTA...\r\n");
            VTS_flags.OTA_req = true;
        }
        else
        {
            odt_vts_dbgPrintf("[OTA] Received version is not newer. Ignoring OTA.\r\n");
        }
            // odt_vts_dbgPrintf("[OTA] OTA REQUEST RECEIVED FROM SMS\r\n");
            // VTS_flags.OTA_req = true;
            // return;
    }
    else if(strstr(sms_resp,Sms_Cmd_list[GET_LOCATION]))
    {
        Get_Location();
        nwy_sms_info_type_t sms_location_info_send={0};
         snprintf(sms_location_info_send.msg_context,
             NWY_SMS_MAX_MO_MSG_LENGTH,
             "Lat: %.6f\nLon: %.6f\nSpeed: %.2f km/h\nCOG: %.6f\nsatellites: %d",
             Loc_param.latitude,
             Loc_param.longitude,
             Loc_param.speed,
             Loc_param.course,
             Loc_param.satellites);
        sms_location_info_send.msg_context_len = strlen(sms_location_info_send.msg_context);
        sms_location_info_send.msg_format = NWY_SMS_MSG_FORMAT_GSM7;
       strcpy(sms_location_info_send.phone_num, sender_num);
       

       odt_vts_dbgPrintf("before the sms send\r\n");
        if(nwy_sms_msg_send(NWY_SIM_ID_SLOT_1,&sms_location_info_send) == NWY_SUCCESS)
        {
            odt_vts_dbgPrintf("sms sent success\r\n");
        }
        else
        {
            odt_vts_dbgPrintf("sms sent failed\r\n");
        }
        odt_vts_dbgPrintf("after the sms send\r\n");
    }
    else if(strstr(sms_resp,Sms_Cmd_list[GET_SIM_STATUS]))
    {
        nwy_sms_info_type_t sms_sim_info_send;
       nwy_sim_status_e status =  GET_SIM_Status();
       memset(sms_sim_info_send.msg_context,0,sizeof(sms_sim_info_send.msg_context));
       if(status == 0)
       {
         sprintf(sms_sim_info_send.msg_context,"SIM STATUS: SIM NOT INSERTED");
       }
       else if(status == 1)
       {
        sprintf(sms_sim_info_send.msg_context,"SIM STATUS: SIM INSERTED");
       }
       else
       {
        sprintf(sms_sim_info_send.msg_context,"SIM STATUS: SIM ERROR");
       }

       sms_sim_info_send.msg_context_len = strlen(sms_sim_info_send.msg_context);
        sms_sim_info_send.msg_format = NWY_SMS_MSG_FORMAT_GSM7;
       strcpy(sms_sim_info_send.phone_num, sender_num);
        nwy_sms_msg_send(0,&sms_sim_info_send);
    }
    else if(strstr(sms_resp,Sms_Cmd_list[GET_NETWORK]))
    {
        nwy_sms_info_type_t sms_network_info_send;
        uint8_t reg_sms;
       reg_sms =  Set_Network_Reg();
       memset(sms_network_info_send.msg_context,0,sizeof(sms_network_info_send.msg_context));
       if(reg_sms ==1)
       {
        //Network Registration Status: Success
            sprintf(sms_network_info_send.msg_context,"Network Registration Status: Success");
       }
       else
       {
        sprintf(sms_network_info_send.msg_context,"Network Registration Status: Failed");
       }
        sms_network_info_send.msg_context_len = strlen(sms_network_info_send.msg_context);
        sms_network_info_send.msg_format = NWY_SMS_MSG_FORMAT_GSM7;
       strcpy(sms_network_info_send.phone_num, sender_num);
        nwy_sms_msg_send(0,&sms_network_info_send);
    }
    else if(strstr(sms_resp,Sms_Cmd_list[GET_VERSION]))
    {
         nwy_sms_info_type_t sms_firm_ver_info_send;
          memset(sms_firm_ver_info_send.msg_context,0,sizeof(sms_firm_ver_info_send.msg_context));
          sprintf(sms_firm_ver_info_send.msg_context,"FIRMWARE VERSION: %s",FW_VERSION);
          sms_firm_ver_info_send.msg_context_len = strlen(sms_firm_ver_info_send.msg_context);
        sms_firm_ver_info_send.msg_format = NWY_SMS_MSG_FORMAT_GSM7;
       strcpy(sms_firm_ver_info_send.phone_num, sender_num);
        nwy_sms_msg_send(0,&sms_firm_ver_info_send);

    }
    else if(strstr(sms_resp,Sms_Cmd_list[GET_BATTERY_VOLTAGE]))
    {
         nwy_sms_info_type_t sms_bat_volt_info_send;
         memset(sms_bat_volt_info_send.msg_context,0,sizeof(sms_bat_volt_info_send.msg_context));
         Get_Battery_Volt();
         float battery_volt = Batt_Volt;
         sprintf(sms_bat_volt_info_send.msg_context,"BATTERY VOLTAGE: %.2f V",battery_volt);
         sms_bat_volt_info_send.msg_context_len = strlen(sms_bat_volt_info_send.msg_context);
          sms_bat_volt_info_send.msg_format = NWY_SMS_MSG_FORMAT_GSM7;
        strcpy(sms_bat_volt_info_send.phone_num, sender_num);
        nwy_sms_msg_send(0,&sms_bat_volt_info_send);

    }
    else if(strstr(sms_resp,Sms_Cmd_list[GET_SERVER_STATUS]))
    {
         nwy_sms_info_type_t sms_server_status_info_send = {0};
         sprintf(sms_server_status_info_send.msg_context,"SERVER STATUS: %s",VTS_State == APP_HTTP_RUNNING ? "RUNNING" : "NOT RUNNING");
         sms_server_status_info_send.msg_context_len = strlen(sms_server_status_info_send.msg_context);
          sms_server_status_info_send.msg_format = NWY_SMS_MSG_FORMAT_GSM7;
        strcpy(sms_server_status_info_send.phone_num, sender_num);
        nwy_sms_msg_send(0,&sms_server_status_info_send);

    }
     else if(strstr(sms_resp,Sms_Cmd_list[GET_SERVER_INFO]))
    {
         nwy_sms_info_type_t sms_server_info_send = {0};
         sprintf(sms_server_info_send.msg_context,"SERVER INFO:\nIP: %s\nPORT: %d\nEP: %s<imei>",
                 Http_Config.host,
                 Http_Config.port,
                 Http_Config.endpoint);
         sms_server_info_send.msg_context_len = strlen(sms_server_info_send.msg_context);
          sms_server_info_send.msg_format = NWY_SMS_MSG_FORMAT_GSM7;
        strcpy(sms_server_info_send.phone_num, sender_num);
        nwy_sms_msg_send(0,&sms_server_info_send);

    }
    else if(strstr(sms_resp,Sms_Cmd_list[GET_DEVICE_ID]))
    {
         nwy_sms_info_type_t sms_device_id_info_send = {0};
         sprintf(sms_device_id_info_send.msg_context,"DEVICE ID: %s",Device_ID);
         sms_device_id_info_send.msg_context_len = strlen(sms_device_id_info_send.msg_context);
          sms_device_id_info_send.msg_format = NWY_SMS_MSG_FORMAT_GSM7;
        strcpy(sms_device_id_info_send.phone_num, sender_num);
        nwy_sms_msg_send(0,&sms_device_id_info_send);

    }
    else if(strstr(sms_resp,"1"))
    {
         nwy_sms_info_type_t sms_get_all_info_send = {0};

        for(int i = 0; i < SMS_CMD_LIST_SIZE; i++)
        {
            strcat(sms_get_all_info_send.msg_context, Sms_Cmd_list[i]);
            strcat(sms_get_all_info_send.msg_context, "\n");
        }
        sms_get_all_info_send.msg_context_len = strlen(sms_get_all_info_send.msg_context);
        sms_get_all_info_send.msg_format = NWY_SMS_MSG_FORMAT_GSM7;
        strcpy(sms_get_all_info_send.phone_num, sender_num);
        nwy_sms_msg_send(0,&sms_get_all_info_send);
    }

    else
    {
         nwy_sms_info_type_t sms_wrong_command_info_send;
         memset(sms_wrong_command_info_send.msg_context,0,sizeof(sms_wrong_command_info_send.msg_context));
         sprintf(sms_wrong_command_info_send.msg_context,"INVALID COMMAND\nENTER 1 TO GET ALL AVAILABLE COMMANDS");
         sms_wrong_command_info_send.msg_context_len = strlen(sms_wrong_command_info_send.msg_context);
          sms_wrong_command_info_send.msg_format = NWY_SMS_MSG_FORMAT_GSM7;
        strcpy(sms_wrong_command_info_send.phone_num, sender_num);
        nwy_sms_msg_send(0,&sms_wrong_command_info_send);

    }

}
// void handle_server_sms(char *sms)
// {
//  if (strncmp(sms, "SRV=", 4) == 0)
//     {
//         char ip[64] = {0};
//         char ep[128] = {0};
//         int port = 0;

//         int parsed = sscanf(sms,
//                             "SRV=%63[^,],PORT=%d,EP=%127s",
//                             ip, &port, ep);

//         if (parsed != 3)
//         {
//             odt_vts_dbgPrintf("SERVER SMS FORMAT ERROR\r\n");
//             return;
//         }

//         strcpy(flash_paramss.server.server_ip, ip);
//         flash_paramss.server.port = (uint16_t)port;
//         strcpy(flash_paramss.server.endpoint, ep);
//         flash_paramss.server.valid = true;

//         odt_vts_dbgPrintf("SERVER UPDATED VIA SMS\r\n");
//         odt_vts_dbgPrintf("IP   : %s\r\n", flash_paramss.server.server_ip);
//         odt_vts_dbgPrintf("PORT : %d\r\n", flash_paramss.server.port);
//         odt_vts_dbgPrintf("EP   : %s\r\n", flash_paramss.server.endpoint);

//         // flash_write();
    
//         VTS_State = APP_HTTP_CLOSE;

//     }



//     /* ---------- OTA CONFIG ---------- */
//     // else if (strstr(sms, "OTA"))
//     // {
//     //     odt_vts_dbgPrintf("ENTER INTO THE ");
//     //    Parse_IOT_Server_Params(sms);
//     // }
// }