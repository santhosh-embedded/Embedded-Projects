#include "ota.h"
#include "nwy_osi_api.h"
#include "nwy_test_cli_utils.h"
#include "nwy_fota_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <https_server.h>
#include "driver.h"
#include "Network.h"
#include "sms.h"

/* Global variables */
nwy_ftp_handle_t g_ftp_fota_handl1 = NULL;
nwy_ftp_get_param_t g_ftp_get_param1;
char g_filename1[64];
char echoo_buff[NWY_EXT_SIO_PER_LE + 1] = {0};
uint8_t  frame = 0;
char buff[100];
nwy_ota_package_t g_ota_pack1 = {0};
static   bool     ota_in_progress     = false;

OTA_Server_Params_S Ftp_config = {0};
/* ------------------ OTA CALLBACK -------------------- */

// static void nwy_cli_ftp_fota_result_cb(nwy_ftp_result_t *param)
// {
//     int *size;
//     int result = NWY_GEN_E_UNKNOWN;
//     unsigned short cid = 0;
//     int clean_flg = 0;
//     // memset(&g_ota_pack1, 0, sizeof(g_ota_pack1));
//     if (NULL == param)
//     {
//         odt_vts_dbgPrintf("event is NULL\r\n");
//     }
    
//     // NWY_SDK_LOG_DEBUG("event is %d", param->event);
//     sprintf(buff, "event is: %d", param->event);
//     odt_vts_dbgPrintf(buff);

//     cid= param->channel;
//     // NWY_SDK_LOG_DEBUG("cli cid is %d", param->channel);
//     sprintf(buff, "cli cid is: %d", param->channel);
//     odt_vts_dbgPrintf(buff);

//     switch(param->event)
//     {
//         case NWY_FTP_EVENT_DATA_GET_ERROR:
//             odt_vts_dbgPrintf("\r\nFtp get error:%d",cid);
//             if(g_ftp_fota_handl1 != NULL)
//                 nwy_ftp_logout(g_ftp_fota_handl1);
//             break;
//         case NWY_FTP_EVENT_LOGIN:
//             odt_vts_dbgPrintf("\r\nFtp login success,cid:%d",cid);
//             odt_vts_dbgPrintf("\r\nRESULT:%d",result);
//             result = nwy_ftp_get_filesize(g_ftp_fota_handl1, g_ftp_get_param1.remotefile);
//             if (NWY_SUCCESS == result)
//             {
//                 odt_vts_dbgPrintf("\r\n nwy_ftp_get_filesize success");
//                 frame = 1;
                
//             }
//             else
//             {
//                 nwy_ftp_logout(g_ftp_fota_handl1);
//                 odt_vts_dbgPrintf("\r\n nwy_ftp_get_filesize fail");
//             }
//             break;
//         case NWY_FTP_EVENT_DATA_OPEND:
//             odt_vts_dbgPrintf("\r\nFtp data open,cid:%d",cid);
//             break;
//         case NWY_FTP_EVENT_PASS_ERROR:
//             clean_flg = 1;
//             odt_vts_dbgPrintf("\r\nFtp passwd error,cid:%d",cid);
//             break;
//         case NWY_FTP_EVENT_FILE_NOT_FOUND:
//             odt_vts_dbgPrintf("\r\nFtp file not found,cid:%d",cid);
//             if(g_ftp_fota_handl1 != NULL)
//                 nwy_ftp_logout(g_ftp_fota_handl1);
//             break;
//         case NWY_FTP_EVENT_FILE_SIZE_ERROR:
//             odt_vts_dbgPrintf("\r\nFtp file size error,cid:%d",cid);
//             if(g_ftp_fota_handl1 != NULL)
//                 nwy_ftp_logout(g_ftp_fota_handl1);
//             break;
//         case NWY_FTP_EVENT_DATA_SETUP_ERROR:
//             odt_vts_dbgPrintf("\r\nFtp data setup error,cid:%d",cid);
//             if(g_ftp_fota_handl1 != NULL)
//                 nwy_ftp_logout(g_ftp_fota_handl1);
//             break;
//         case NWY_FTP_EVENT_SIZE:
//             size = (int *)param->data;
//             odt_vts_dbgPrintf("\r\nFtp size is %d,cid:%d", *size,cid);
//             g_ota_pack1.total_size = *size;
//             g_ota_pack1.offset = 0;
//             result = nwy_ftp_get(g_ftp_fota_handl1,&g_ftp_get_param1);
//             if (NWY_SUCCESS == result)
//             {
//                 odt_vts_dbgPrintf("\r\n nwy_ftp_get success");
//             }
//             else
//             {
//                 nwy_ftp_logout(g_ftp_fota_handl1);
//                 odt_vts_dbgPrintf("\r\n nwy_ftp_get fail");
//             }
//             break;
//         case NWY_FTP_EVENT_LOGOUT:
//             clean_flg = 1;
//             odt_vts_dbgPrintf("\r\nFtp logout,cid:%d",cid);
//             if(g_ota_pack1.offset == g_ota_pack1.total_size && g_ota_pack1.offset != 0)
//                 {
//                     odt_vts_dbgPrintf("\r\n update start...\r\n");
//                     result = nwy_fota_update(1);
//                     if(result == NWY_FOTA_CHECK_FAILED)
//                         {
//                             odt_vts_dbgPrintf("\r\n fota package check fail\r\n");
//                         }
//                     else if(result == NWY_FOTA_WRITE_FLAG_FAIL)
//                         {
//                             odt_vts_dbgPrintf("\r\n write fota update flag file fail\r\n");
//                         }
//                 }
//             break;
//         case NWY_FTP_EVENT_CLOSED:
//             clean_flg = 1;
//             odt_vts_dbgPrintf("\r\nFtp connection closed,cid:%d",cid);
//             break;
//         case NWY_FTP_EVENT_DNS_ERR:
//         case NWY_FTP_EVENT_OPEN_FAIL:
//             clean_flg = 1;
//             odt_vts_dbgPrintf("\r\nFtp login fail,cid:%d",cid);
//             break;
//         case NWY_FTP_EVENT_DATA_GET:
//         {
//             if (NULL == param->data)
//                 {
//                     odt_vts_dbgPrintf("\r\ncid:%d,recved data is NULL,datalen:%d",cid,param->data_len);
//                     nwy_ftp_logout(g_ftp_fota_handl1);
//                     return;
//                 }

//             if (param->data_len != 0)
//                 {
//                     g_ota_pack1.len = param->data_len;
//                     g_ota_pack1.data = malloc(param->data_len);
//                     if(g_ota_pack1.data == NULL)
//                     {
//                         odt_vts_dbgPrintf("\r\n data malloc error:\r\n");
//                         nwy_ftp_logout(g_ftp_fota_handl1);
//                         break;
//                     }
//                     memcpy(g_ota_pack1.data, param->data, param->data_len);
//                     result = nwy_fota_write(&g_ota_pack1);
//                     if(result == NWY_FOTA_CHECK_FAILED)
//                     {
//                         odt_vts_dbgPrintf("\r\n The fota package does not matche the current version\r\n");
//                         free(g_ota_pack1.data);
//                         nwy_ftp_logout(g_ftp_fota_handl1);
//                         break;
//                     }
//                     else if(result == NWY_FOTA_WRITE_FAILED)
//                     {
//                         odt_vts_dbgPrintf("\r\n fota write fail\r\n");
//                         free(g_ota_pack1.data);
//                         nwy_ftp_logout(g_ftp_fota_handl1);
//                         break;
//                     }
//                     else if(result == NWY_FS_SPACE_NOT_ENOUGH)
//                     {
//                         odt_vts_dbgPrintf("\r\n file system space not enough\r\n");
//                         free(g_ota_pack1.data);
//                         nwy_ftp_logout(g_ftp_fota_handl1);
//                         break;
//                     }
//                     else if(result == NWY_GEN_E_INVALID_PARA)
//                     {
//                         odt_vts_dbgPrintf("\r\n function input parameters error\r\n");
//                         free(g_ota_pack1.data);
//                         nwy_ftp_logout(g_ftp_fota_handl1);
//                         break;
//                     }
//                     else if(result == NWY_GEN_E_MALLOC_FAIL)
//                     {
//                         odt_vts_dbgPrintf("\r\n failed to apply for memory.\r\n");
//                         free(g_ota_pack1.data);
//                         nwy_ftp_logout(g_ftp_fota_handl1);
//                         break;
//                     }

//                     free(g_ota_pack1.data);
//                     g_ota_pack1.offset += g_ota_pack1.len;
//                 }
//         }
//         break;
//         case NWY_FTP_EVENT_DATA_GET_FINISHED:
//             odt_vts_dbgPrintf("\r\ncid:%d,get completed, total data size is :%d",cid,param->data_len);
//             break;
//         case NWY_FTP_EVENT_DATA_CLOSED:
//             if(g_ftp_fota_handl1 != NULL)
//                 nwy_ftp_logout(g_ftp_fota_handl1);
//             odt_vts_dbgPrintf("\r\nftp data close,cid:%d",cid);
//             break;
//         case NWY_FTP_EVENT_SSL_CONNECT_FAIL:
//             clean_flg = 1;
//             odt_vts_dbgPrintf("\r\nftp ssl error,cid:%d",cid);
//             break;
//         case NWY_FTP_EVENT_SSL_CONNECTED:
//             odt_vts_dbgPrintf("\r\nftp ssl connected,cid:%d",cid);
//             break;
//         default:
//             odt_vts_dbgPrintf("\r\nFtp unknown error,cid:%d",cid);
//             break;

//     }

//     if( clean_flg == 1)
//     {
//         //NWY_SDK_LOG_DEBUG("clean ftp client cid:%d", cid);
//         sprintf(buff, "clean ftp client cid: %d", cid);
//         odt_vts_dbgPrintf(buff);
//         odt_vts_dbgPrintf("\r\nclean ftp client cid:%d", cid);
//         g_ftp_fota_handl1 = NULL;
//     }
//     return;
// }


static void nwy_cli_ftp_fota_result_cb(nwy_ftp_result_t *param)
{
    int *size;
    int result = NWY_GEN_E_UNKNOWN;
    unsigned short cid = 0;
    int clean_flg = 0;
    if (NULL == param)
    {
        odt_vts_dbgPrintf("event is NULL\r\n");
    }
    
    // NWY_SDK_LOG_DEBUG("event is %d", param->event);

    cid= param->channel;
    // NWY_SDK_LOG_DEBUG("cli cid is %d", param->channel);

    switch(param->event)
    {
        case NWY_FTP_EVENT_DATA_GET_ERROR:
            odt_vts_dbgPrintf("Ftp get error:%d",cid);
            if(g_ftp_fota_handl1 != NULL)
                nwy_ftp_logout(g_ftp_fota_handl1);
            break;
        case NWY_FTP_EVENT_LOGIN:
            odt_vts_dbgPrintf("Ftp login success,cid:%d",cid);
            odt_vts_dbgPrintf("RESULT:%d",result);
            result = nwy_ftp_get_filesize(g_ftp_fota_handl1, g_ftp_get_param1.remotefile);
            if (NWY_SUCCESS == result)
            {
                odt_vts_dbgPrintf("nwy_ftp_get_filesize success");
                frame = 1;
                
            }
            else
            {
                nwy_ftp_logout(g_ftp_fota_handl1);
                odt_vts_dbgPrintf("nwy_ftp_get_filesize fail");
            }
            break;
        case NWY_FTP_EVENT_DATA_OPEND:
            odt_vts_dbgPrintf("Ftp data open,cid:%d",cid);
            break;
        case NWY_FTP_EVENT_PASS_ERROR:
            clean_flg = 1;
            odt_vts_dbgPrintf("Ftp passwd error,cid:%d",cid);
            break;
        case NWY_FTP_EVENT_FILE_NOT_FOUND:
            odt_vts_dbgPrintf("Ftp file not found,cid:%d",cid);
            if(g_ftp_fota_handl1 != NULL)
                nwy_ftp_logout(g_ftp_fota_handl1);
            break;
        case NWY_FTP_EVENT_FILE_SIZE_ERROR:
            odt_vts_dbgPrintf("Ftp file size error,cid:%d",cid);
            if(g_ftp_fota_handl1 != NULL)
                nwy_ftp_logout(g_ftp_fota_handl1);
            break;
        case NWY_FTP_EVENT_DATA_SETUP_ERROR:
            odt_vts_dbgPrintf("Ftp data setup error,cid:%d",cid);
            if(g_ftp_fota_handl1 != NULL)
                nwy_ftp_logout(g_ftp_fota_handl1);
            break;
        case NWY_FTP_EVENT_SIZE:
            size = (int *)param->data;
            odt_vts_dbgPrintf("Ftp size is %d,cid:%d", *size,cid);
            g_ota_pack1.total_size = *size;
            g_ota_pack1.offset = 0;
            result = nwy_ftp_get(g_ftp_fota_handl1,&g_ftp_get_param1);
            if (NWY_SUCCESS == result)
            {
                odt_vts_dbgPrintf("nwy_ftp_get success");
            }
            else
            {
                nwy_ftp_logout(g_ftp_fota_handl1);
                odt_vts_dbgPrintf("nwy_ftp_get fail");
            }
            break;
        case NWY_FTP_EVENT_LOGOUT:
            clean_flg = 1;
            odt_vts_dbgPrintf("Ftp logout,cid:%d",cid);
            if(g_ota_pack1.offset == g_ota_pack1.total_size && g_ota_pack1.offset != 0)
                {
                    // flash_paramss.ota_params.ota_flag = 0;
                    // flash_write();
                    odt_vts_dbgPrintf(" update start...");
                    result = nwy_fota_update(1);
                    if(result == NWY_FOTA_CHECK_FAILED)
                        {
                            odt_vts_dbgPrintf(" fota package check fail");
                        }
                    else if(result == NWY_FOTA_WRITE_FLAG_FAIL)
                        {
                            odt_vts_dbgPrintf(" write fota update flag file fail");
                        }
                }
            break;
        case NWY_FTP_EVENT_CLOSED:
            clean_flg = 1;
            odt_vts_dbgPrintf("Ftp connection closed,cid:%d",cid);
            break;
        case NWY_FTP_EVENT_DNS_ERR:
        case NWY_FTP_EVENT_OPEN_FAIL:
            clean_flg = 1;
            odt_vts_dbgPrintf("Ftp login fail,cid:%d",cid);
            break;
        case NWY_FTP_EVENT_DATA_GET:
        {
            if (NULL == param->data)
                {
                    odt_vts_dbgPrintf("cid:%d,recved data is NULL,datalen:%d",cid,param->data_len);
                    nwy_ftp_logout(g_ftp_fota_handl1);
                    return;
                }

            if (param->data_len != 0)
                {
                    g_ota_pack1.len = param->data_len;
                    g_ota_pack1.data = malloc(param->data_len);
                    if(g_ota_pack1.data == NULL)
                    {
                        odt_vts_dbgPrintf(" data malloc error");
                        nwy_ftp_logout(g_ftp_fota_handl1);
                        break;
                    }
                    memcpy(g_ota_pack1.data, param->data, param->data_len);
                    result = nwy_fota_write(&g_ota_pack1);
                    if(result == NWY_FOTA_CHECK_FAILED)
                    {
                        odt_vts_dbgPrintf(" The fota package does not matche the current version");
                        free(g_ota_pack1.data);
                        nwy_ftp_logout(g_ftp_fota_handl1);
                        break;
                    }
                    else if(result == NWY_FOTA_WRITE_FAILED)
                    {
                        odt_vts_dbgPrintf(" fota write fail");
                        free(g_ota_pack1.data);
                        nwy_ftp_logout(g_ftp_fota_handl1);
                        break;
                    }
                    else if(result == NWY_FS_SPACE_NOT_ENOUGH)
                    {
                        odt_vts_dbgPrintf(" file system space not enough");
                        free(g_ota_pack1.data);
                        nwy_ftp_logout(g_ftp_fota_handl1);
                        break;
                    }
                    else if(result == NWY_GEN_E_INVALID_PARA)
                    {
                        odt_vts_dbgPrintf(" function input parameters error");
                        free(g_ota_pack1.data);
                        nwy_ftp_logout(g_ftp_fota_handl1);
                        break;
                    }
                    else if(result == NWY_GEN_E_MALLOC_FAIL)
                    {
                        odt_vts_dbgPrintf(" failed to apply for memory.");
                        free(g_ota_pack1.data);
                        nwy_ftp_logout(g_ftp_fota_handl1);
                        break;
                    }

                    free(g_ota_pack1.data);
                    g_ota_pack1.offset += g_ota_pack1.len;
                }
        }
        break;
        case NWY_FTP_EVENT_DATA_GET_FINISHED:
            odt_vts_dbgPrintf("cid:%d,get completed, total data size is :%d",cid,param->data_len);
            break;
        case NWY_FTP_EVENT_DATA_CLOSED:
            if(g_ftp_fota_handl1 != NULL)
                nwy_ftp_logout(g_ftp_fota_handl1);
            odt_vts_dbgPrintf("ftp data close,cid:%d",cid);
            break;
        case NWY_FTP_EVENT_SSL_CONNECT_FAIL:
            clean_flg = 1;
            odt_vts_dbgPrintf("ftp ssl error,cid:%d",cid);
            break;
        case NWY_FTP_EVENT_SSL_CONNECTED:
            odt_vts_dbgPrintf("ftp ssl connected,cid:%d",cid);
            break;
        default:
            odt_vts_dbgPrintf("Ftp unknown error,cid:%d",cid);
            break;

    }

    if( clean_flg == 1)
    {
        // NWY_SDK_LOG_DEBUG("clean ftp client cid:%d", cid);
        odt_vts_dbgPrintf("clean ftp client cid:%d", cid);
        g_ftp_fota_handl1 = NULL;
    }
    return;
}

/* ------------------ MAIN OTA FUNCTION -------------------- */


// void Ota_init(void)
//     {
//         // memset(&g_ota_pack1, 0, sizeof(g_ota_pack1));
//         if(ota_in_progress)return;
//             ota_in_progress = true;
//             nwy_ftp_param_t ftp_param;
//             ftp_param.cid = 7;                    // PDP context ID
//             ftp_param.host = (char *)host;
//             ftp_param.port =         port;
//             ftp_param.username = (char *)username;
//             ftp_param.password = (char *)password;
//             ftp_param.timeout_s = 60;
//             ftp_param.mode = 0;                   // Active mode
//             ftp_param.cb = nwy_cli_ftp_fota_result_cb;
//         strcpy(remote_filename, filename);
//         memset(&g_ota_pack1, 0, sizeof(g_ota_pack1));
//         /* Start FTP Login */
//         g_ftp_fota_handl1 = nwy_ftp_login(&ftp_param, NULL);
//         if (!g_ftp_fota_handl1)
//             {
//                 odt_vts_dbgPrintf("\r\n[OTA] FTP Login Failed!\n");
//                 return;
//             }
//         /* Prepare file download */
//         g_ftp_get_param1.len = 0;
//         g_ftp_get_param1.offset = 0;
//         g_ftp_get_param1.type = 2;
//         g_ftp_get_param1.remotefile = remote_filename;

//         odt_vts_dbgPrintf("\r\n[OTA] FTP Login Success, download starting...\n");
       

//     }


    void Ota_init(void)
    {
        if(ota_in_progress)return;
        ota_in_progress = true;
         int s_nwy_ftp_choice1 = 0;
    // set_flag = 0;
    frame = 0;
    odt_vts_dbgPrintf("ENTERED INTO THE FOTA DOWNLOAD STATE\r\n");
        char *sptr = NULL;
        nwy_ftp_param_t ftp_param1;
        nwy_ftp_ssl_cfg_t sslcfg1;
        memset(&sslcfg1,0x00,sizeof(sslcfg1));
        memset(&ftp_param1, 0x00, sizeof(ftp_param1));
        ftp_param1.cid = 1;

        ftp_param1.host = Ftp_config.host;
        odt_vts_dbgPrintf("FTP HOST IP: %s",ftp_param1.host);

        ftp_param1.port = Ftp_config.port;
        odt_vts_dbgPrintf("FTP PORT: %d",ftp_param1.port);
        ftp_param1.mode = 0;

        ftp_param1.username = Ftp_config.username;
        odt_vts_dbgPrintf("FTP USERNAME: %s",ftp_param1.username);

        ftp_param1.password = Ftp_config.password;
        odt_vts_dbgPrintf("FTP PASSWORD: %s", ftp_param1.password);

        s_nwy_ftp_choice1 = 0;
        ftp_param1.timeout_s = 60;
        ftp_param1.cb = nwy_cli_ftp_fota_result_cb;
        memset(g_filename1, 0, sizeof(g_filename1));
        
        // strcpy(g_filename1, Ftp_config.filename);
        sprintf(g_filename1,"nwy_open_app_fota_v%s.pkt", received_version);
        odt_vts_dbgPrintf("FTP FILENAME: %s",g_filename1);
        memset(&g_ota_pack1, 0, sizeof(g_ota_pack1));
        g_ftp_fota_handl1 = nwy_ftp_login(&ftp_param1,NULL);
        g_ftp_get_param1.len = 0;
        g_ftp_get_param1.offset = 0;
        g_ftp_get_param1.remotefile = g_filename1;
        g_ftp_get_param1.type = 2;
        if(g_ftp_fota_handl1 == NULL)
        {
            odt_vts_dbgPrintf("FTP login fail\r\n");
            ota_in_progress = false;
            VTS_State = APP_HTTP_CLOSE;
        }
        else{
            odt_vts_dbgPrintf("FTP LOGIN SUCCESS");
            //  firmware_success_flag = true;

            //  Eventstate = 7;
        }
    }


// void Ota_init()
// {
//     if(ota_in_progress)return;
//         ota_in_progress = true;
//     /* ----- Hard-coded FTP details ----- */
//     // const char * host       = "192.185.129.79";
//     // int         port       = 21;
//     // const char *userName   = "dhanush@outdidunified.com";
//     // const char *password   = "Dhanush@2024";
//     // const char *filename   = "nwy_open_app_fota.pkt";

//     odt_vts_dbgPrintf("\r\n[OTA] Using hardcoded FTP details:");
//     odt_vts_dbgPrintf("\r\nHost: %s", host);
//     odt_vts_dbgPrintf("\r\nPort: %d", port);
//     odt_vts_dbgPrintf("\r\nUser: %s", username);
//     odt_vts_dbgPrintf("\r\nFile: %s\r\n", filename);

//     /* Prepare FTP parameters */
//     nwy_ftp_param_t ftp_param1 = {0};
//     ftp_param1.cid = 1;                    // PDP context ID
//     ftp_param1.host = host;
//     ftp_param1.port = port;
//     ftp_param1.username = username;
//     ftp_param1.password = password;
//     ftp_param1.timeout_s = 60;
//     ftp_param1.mode = 0;                   // Active mode
//     ftp_param1.cb = nwy_cli_ftp_fota_result_cb;

//     strcpy(g_filename1, filename);
//     memset(&g_ota_pack1, 0, sizeof(g_ota_pack1));

//     /* Start FTP Login */
//     g_ftp_fota_handl1 = nwy_ftp_login(&ftp_param1, NULL);

//     if (!g_ftp_fota_handl1)
//     {
//         odt_vts_dbgPrintf("\r\n[OTA] FTP Login Failed!\n");
//         return;
//     }

//     /* Prepare file download */
//     g_ftp_get_param1.len = 0;
//     g_ftp_get_param1.offset = 0;
//     g_ftp_get_param1.type = 2;
//     g_ftp_get_param1.remotefile = g_filename1;

//     odt_vts_dbgPrintf("\r\n[OTA] FTP Login Success, download starting...\n");
// }


// void ota_start_from_json(uint8_t *IotServerParams)
// {
//     char *fwurl_ptr = strstr((char *)IotServerParams, "\"FWURL\"");
//     if (!fwurl_ptr)
//     {
//         odt_vts_dbgPrintf("FWURL not found\n");
//         return;
//     }

//     char url[256] = {0};
//     sscanf(fwurl_ptr, "\"FWURL\":\"%[^\"]\"", url);

//     char protocol[10] = {0}, host[64] = {0}, port[10] = {0}, 
//          userName[64] = {0}, password[64] = {0}, filename[64] = {0};

//     int parsed = sscanf(url, "%[^:]://%[^:]:%[^:]:%[^:]:%[^:]:%s",
//                         protocol, host, port, userName, password, filename);

//     if (parsed != 6)
//     {
//         odt_vts_dbgPrintf("FWURL parse failed\n");
//         return;
//     }

//     /* Prepare FTP parameters */
//     nwy_ftp_param_t ftp_param1 = {0};
//     ftp_param1.cid = 1;
//     ftp_param1.host = host;
//     ftp_param1.port = atoi(port);
//     ftp_param1.username = userName;
//     ftp_param1.password = password;
//     ftp_param1.timeout_s = 60;
//     ftp_param1.mode = 0;
//     ftp_param1.cb = nwy_cli_ftp_fota_result_cb;

//     strcpy(g_filename1, filename);
//     memset(&g_ota_pack1, 0, sizeof(g_ota_pack1));

//     g_ftp_fota_handl1 = nwy_ftp_login(&ftp_param1, NULL);

//     if (!g_ftp_fota_handl1)
//     {
//         odt_vts_dbgPrintf("FTP Login Failed!\n");
//         return;
//     }

//     /* Prepare file download */
//     g_ftp_get_param1.len = 0;
//     g_ftp_get_param1.offset = 0;
//     g_ftp_get_param1.type = 2;
//     g_ftp_get_param1.remotefile = g_filename1;

//     odt_vts_dbgPrintf("FOTA Login Success, download starting...\n");
// }

/* optional */
// void ota_init(void)
// {
//     odt_vts_dbgPrintf("OTA Module Initialized\n");
// }
