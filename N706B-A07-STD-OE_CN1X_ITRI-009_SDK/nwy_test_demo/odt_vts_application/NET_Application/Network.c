#include "Network.h"
#include "VTS_App.h"
#include "nwy_test_cli_adpt.h"
#include "driver.h"
#include "http.h"
#include "ota.h"
#include "sms.h"
#include "flash.h"


APP_State_t VTS_State = {0};
uint8_t http_reconnect_count = 0;

 nwy_sim_status_e GET_SIM_Status(void)
{
    nwy_sim_status_e sim_status;
    if (nwy_sim_status_get(NWY_SIM_ID_SLOT_1, &sim_status) == NWY_SUCCESS)
    {
        switch (sim_status)
        {
        case NWY_SIM_READY:
            odt_vts_dbgPrintf("SIM_STATUS: READY!\r\n");
             GPIO_Set_Output(69, 1);
            break;
        case NWY_SIM_NOT_INSERTED:
            odt_vts_dbgPrintf("SIM_STATUS: NOT_INSERT!\r\n");
             GPIO_Set_Output(69, 0);
            break;
        default:
            odt_vts_dbgPrintf("SIM_STATUS: ERROR!\r\n");
             GPIO_Set_Output(69, 0);
            break;
        }
    }
    // if(Read_SIM_Presence())
    // {
    //    sim_status =  NWY_SIM_READY;
    //    odt_vts_dbgPrintf("SIM_STATUS: READY!\r\n");
    //    GPIO_Set_Output(69, 1);
    // }
    // else
    // {
    //     sim_status = NWY_SIM_NOT_INSERTED;
    //     odt_vts_dbgPrintf("SIM_STATUS: NOT_INSERT!\r\n");
    //     GPIO_Set_Output(69, 0);
    // }
        
    return sim_status;
}

 uint8_t Set_Network_Reg(void)
{
    nwy_nw_regstatus_t reg;
    memset(&reg, 0, sizeof(reg));

    if (nwy_nw_regstatus_get(NWY_SIM_ID_SLOT_1, &reg) == NWY_SUCCESS)
    {
        if (reg.ps_regs_valid && reg.ps_regs.regs_state == NWY_NW_NET_STAT_CURRENT)
        {
            odt_vts_dbgPrintf("Network Registration Status: Success\r\n");
             GPIO_Set_Output(69, 1);
            return 1;
        }
        else
        {
           GPIO_Set_Output(69, 0);
            odt_vts_dbgPrintf("Network Registration Status: Failed\r\n");
            return 0;
        }
    }
    else
    {
        odt_vts_dbgPrintf("Network Registration Status: Error\r\n");
    }

    return 0;
    // nwy_data_start_call_t param_t = {0};
    // param_t.cid = 1;
    // param_t.call_auto_type = NWY_DATA_CALL_AUTO_TYPE_DISABLE;
    // param_t.action = NWY_DATA_CALL_ACT;
    // param_t.trigger_type = NWY_DATA_TRIGGER_OPEN;
    // param_t.if_internal_call = 1;
    // param_t.set_profile.pdp_type = NWY_DATA_PDP_TYPE_MIN ;
    // param_t.set_profile.auth_proto = NWY_DATA_AUTH_PROTO_MIN ;    
    
    // nwy_data_reg_cb(ACTIVE_SIM_SLOT, PROFILE_ID, Data_Start_CallBack);
    // odt_net_dbgPrintf("Starting data call ...\r\n");
}

static void Set_profile(void)
{
    char get_apn[100] ={0};
    nwy_data_pdp_type_e ip_type = NWY_DATA_PDP_TYPE_IPV4V6;
    nwy_data_eps_dyn_apn_get(NWY_SIM_ID_SLOT_1, &ip_type, get_apn);

    nwy_data_profile_info_t profile_info;
    profile_info.auth_proto = NWY_DATA_AUTH_PROTO_NONE;
    profile_info.pdp_type   = NWY_DATA_PDP_TYPE_IPV4;
    strcpy(profile_info.apn, get_apn);
    strcpy(profile_info.user_name, "outdid");
    strcpy(profile_info.pwd, "1234");
    nwy_data_profile_set(NWY_SIM_ID_SLOT_1, 1, &profile_info);

    nwy_data_profile_info_t get_profile_info = {0};
    nwy_data_profile_get(NWY_SIM_ID_SLOT_1, 1, &get_profile_info);

    odt_vts_dbgPrintf("Profile %d info: <pdp_type>,<auth_proto>,<apn>,<user_name>,<password>\r\n%d,%d,%s,%s,%s\r\n", 1, get_profile_info.pdp_type,
                get_profile_info.auth_proto, get_profile_info.apn, get_profile_info.user_name, get_profile_info.pwd);
}


static uint8_t Activate_PDP(void)
{
    odt_vts_dbgPrintf("...............\r\n");
    nwy_data_start_call_t call = {0};
    nwy_error_e ret;
    call.cid = 1;
    call.action = NWY_DATA_CALL_ACT;
    odt_vts_dbgPrintf("PDP Activation Start\r\n");
    ret = nwy_data_call_start(NWY_SIM_ID_SLOT_1, &call);
    if (ret == NWY_SUCCESS || ret == NWY_GEN_E_AREADY_CONNECT)
    {
        odt_vts_dbgPrintf("PDP Activation Success: %d\r\n", ret);
        return 1;
    }
    else
    {
        odt_vts_dbgPrintf("\r\nPDP Activation Error: %d\r\n", ret);
        return 0;
    } 
    // nwy_test_cli_get_dhcp_info();
}


static void GET_IMEI(void)
{
    char imei[16] = {0};
    if (nwy_dm_imei_get(NWY_SIM_ID_SLOT_1, imei, sizeof(imei)) == NWY_SUCCESS)
    {
        strcpy(Device_ID, imei);
        // strcpy(Device_ID, " N7068_VTS_001");
        odt_vts_dbgPrintf("IMEI No: %s\r\n", Device_ID);  
    }
    else
    {
        odt_vts_dbgPrintf("IMEI Read Error\r\n");
    }
}

static void Module_and_Version()
{
    char model[50] = {0};
    if (nwy_dm_dev_model_get(model, sizeof(model)) == NWY_SUCCESS)
    {
        odt_vts_dbgPrintf("Module Model: %s\r\n", model);
        // strcpy(Boot_param.Module_model, model);
    }
    else
    {
        odt_vts_dbgPrintf("Module Model Read Error\r\n");
    }

    char hw_ver[20] = {0};
    if (nwy_dm_hw_version_get(hw_ver, sizeof(hw_ver)) == NWY_SUCCESS)
    {
        odt_vts_dbgPrintf("Hardware Version: %s\r\n", hw_ver);
        // strcpy(Boot_param.Hardware_Ver, hw_ver);
    }
    else
    {
        odt_vts_dbgPrintf("Hardware Version Read Error\r\n");
    }
    
    char sw_ver[64] = {0};
    if (nwy_dm_inner_version_get(sw_ver, sizeof(sw_ver)) == NWY_SUCCESS)
    {
        odt_vts_dbgPrintf("Software Version: %s\r\n", sw_ver);
        // strcpy(Boot_param.Software_Ver, sw_ver);
    }
    else
    {
        odt_vts_dbgPrintf("Software Version Read Error\r\n");
    }
}

void Network_Polling(void)
{
    switch (VTS_State)
    {
        case APP_INIT:
            VTS_State = APP_FLASH_READ;
            break;

        case APP_FLASH_READ:
            if(Check_VTSflash())
            {
                Flash_Read();
            }
            else{
                Get_Default_Config();
                Make_VTSflash_Dir();
                Flash_Write();
            }
            VTS_State = APP_WAIT_SIM;
            break;

        case APP_WAIT_SIM:
            if (GET_SIM_Status() == NWY_SIM_READY)
                VTS_State = APP_WAIT_NETWORK;
            break;

        case APP_WAIT_NETWORK:
            if (Set_Network_Reg())
                VTS_State = APP_PDP_ACTIVATE;
            break;

        case APP_PDP_ACTIVATE:
            Set_profile();
            if (Activate_PDP())
            VTS_State = APP_GET_IMEI;
            break;

        case APP_GET_IMEI:
            GET_IMEI();
            VTS_State = APP_SMS_INIT;
            break;
        case APP_SMS_INIT:
            Init_SMS();
            VTS_State = APP_GET_VERSION;
            break;    

        case APP_GET_VERSION:
            Module_and_Version();
            //  VTS_State = OTA_INIT;              // testing
            VTS_State = APP_HTTP_SETUP;
            http_reconnect_count = HTTP_RECONNECT_MAX_COUNT;
            // Flash_Read();
            break;

        case APP_HTTP_SETUP: 
            if(HTTP_Setup())
            {
                odt_vts_dbgPrintf("Waiting for the Connection...!\r\n");
                VTS_State = APP_HTTP_WAIT_CONNECT;
            }
            if(http_reconnect_count <= 0)
            {
                VTS_State = APP_WAIT_SIM;
            } 
            break;

        case APP_HTTP_WAIT_CONNECT:     
            break;

        case APP_HTTP_RUNNING:
            VTS_App_Polling();
            break;
            
        case APP_HTTP_CLOSE:
            HTTP_Close();
            SendMsg = StartNavigation;
            if(VTS_flags.OTA_req)
            {
                VTS_flags.OTA_req = false;
                VTS_State = OTA_INIT;
            }
            else
            {
               VTS_State =  APP_HTTP_SETUP;
                http_reconnect_count = HTTP_RECONNECT_MAX_COUNT;
            }

            break;
                
        case OTA_INIT:
            odt_vts_dbgPrintf("OTA INIT SUCCESSFULL\r\n");
            Ota_init();
            VTS_State = OTA_WAIT;
            break;
         
        case OTA_WAIT:
             break;    
        default:
            break;
    }
    if(sms_receive_flag)
        {
            Read_SMS();
            sms_receive_flag = false;
        }

nwy_thread_sleep(250); // fast FSM cycle
}