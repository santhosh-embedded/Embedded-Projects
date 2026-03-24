#include "driver.h"
#include "VTS_App.h"
#include "sms.h"

uint16_t sim_index            = 0;
bool     sms_receive_flag     =false;
uint16_t HeartBeat_counter = 0;


void Sim_Detect_Callback(nwy_sim_id_e sim_id, nwy_sim_urc_type_e urc_type, nwy_sim_info_ind_t *ind_struct)
{
    nwy_test_cli_echo("\r\nrecv sim ind");
    if(NWY_SIM_URC_TYPE_STATUS == urc_type)
    {
        //do customer process
        if(0 == ind_struct->sim_detect_status)
        {
            nwy_test_cli_echo("\r\nsim absent");
        }
        else if(1 == ind_struct->sim_detect_status)
        {
            nwy_test_cli_echo("\r\nsim present");
        }
        nwy_test_cli_echo("\r\nrecv sim status:%d!\r\n", ind_struct->sim_status);
    }

    return;
}


void Usr_Timer_Callback(void)
{

    if(SendMsg > Bootnotification && HeartBeat_interval != 0)
    {
        if(++HeartBeat_counter >= HeartBeat_interval)
        {
            VTS_flags.Heartbeat_Send = true;
            HeartBeat_counter = 0;
        }
    }

}

void nwy_sms_test_recv_cb(nwy_sim_id_e simid, nwy_mt_sms_event_e urc_type, nwy_sms_info_ind_t *ind_struct)
{
    nwy_sms_recv_info_type_t g_sms_data1 = {0};
    nwy_sms_info_ind_t *sms_data = (nwy_sms_info_ind_t *)ind_struct;
    if(NWY_SMS_PP_IND == urc_type)
    {
        if(sms_data->transfer_type == NWY_SMS_TRANSFER_AND_STORE)
        {
            nwy_test_cli_echo("\r\nrecv one sms transfer and store\r\n");
            if(NWY_SMS_STORAGE_TYPE_UIM == sms_data->sms_info.storage_type)
            {
                nwy_test_cli_echo("\r\nrecv sms nIndex:%d, storage:SIM\r\n", sms_data->sms_info.nIndex);
                //  int ret = nwy_sms_msg_read(0, sms_data->sms_info.nIndex, &g_sms_data1) ;
                 sim_index  = sms_data->sms_info.nIndex;
                 sms_receive_flag     = true;
            }
            else if(NWY_SMS_STORAGE_TYPE_NV == sms_data->sms_info.storage_type)
            {
                nwy_test_cli_echo("\r\nrecv sms nIndex:%d, storage:ME\r\n", sms_data->sms_info.nIndex);
            }
        }
        else if(sms_data->transfer_type == NWY_SMS_TRANSFER_ONLY)
        {
            nwy_test_cli_echo("\r\nrecv one sms transfer only\r\n");

            nwy_test_cli_echo("\r\nSMS: %s\r\n",
                sms_data->sms_info.msg_content);

            SMS_Recv_Handler(sms_data->sms_info.msg_content);
        }

        else
        {
            nwy_test_cli_echo("\r\nnwy recv sms transfer type invalid\r\n");
        }
    }

    return;
}

