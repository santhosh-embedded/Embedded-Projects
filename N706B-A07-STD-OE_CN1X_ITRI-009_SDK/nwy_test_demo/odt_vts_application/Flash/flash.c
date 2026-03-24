#include "flash.h"
#include  "http.h"
#include "OTA/ota.h"
#include "nwy_file_api.h"

uint8_t Check_VTSflash(void)
{
    nwy_dir_info_t dir_hdl;
    nwy_dirent_t info;
    int ret;
    int found = 0;

    /* Opening root directory */
    ret = nwy_dir_open("/", &dir_hdl);
    if (ret != NWY_SUCCESS)   return 0;
 
    /* Read entries */
    while ((ret = nwy_dir_read(&dir_hdl, &info)) == NWY_SUCCESS)
    {
        if (strcmp(info.d_name, FLASH_DIR_NAME) == 0)
        {
            found = 1;
            break;   // Stop reading
        }
    }

    /* Always close if opened */
    nwy_dir_close(&dir_hdl);

    return found;
}

void Make_VTSflash_Dir(void)
{
    nwy_error_e ret = nwy_dir_mk(FLASH_DIR_NAME);
    if (ret ==  NWY_SUCCESS) 
    {
        odt_vts_dbgPrintf("dir %s make sucess\r\n", FLASH_DIR_NAME);
    } 
    else 
    {
        odt_vts_dbgPrintf("dir %s make fail\r\n", FLASH_DIR_NAME);
        return;
    }
}

void Flash_Read(void)
{
    Read_from_file(FLASH_SERVER_CONFIG_FILE);
    Read_from_file(FLASH_OTA_CONFIG_FILE);
}

void Read_from_file(const char *filename)
{
    // HTTP_Config_S temp_http = {0};
    // OTA_Server_Params_t temp_ftp = {0};

    odt_vts_dbgPrintf("===== OTA PARAMETERS FROM FLASH =====");

    int fd = nwy_file_open(filename, NWY_RDONLY);
    if (fd >= 0)
    {
        if(strcmp(filename, FLASH_SERVER_CONFIG_FILE) == 0)
        {
            nwy_file_read(fd, &Http_Config, sizeof(HTTP_Config_S));
            odt_vts_dbgPrintf("HTTP Endpoint :%s", Http_Config.endpoint);
            odt_vts_dbgPrintf("HTTP Host     : %s", Http_Config.host);
            odt_vts_dbgPrintf("HTTP Port     : %d", Http_Config.port);
        }
        else if(strcmp(filename, FLASH_OTA_CONFIG_FILE) == 0)
        {
            nwy_file_read(fd, &Ftp_config, sizeof(OTA_Server_Params_S));
            odt_vts_dbgPrintf("FTP Host : %s", Ftp_config.host);
            odt_vts_dbgPrintf("FTP Port : %d", Ftp_config.port);
            odt_vts_dbgPrintf("FTP Username : %s", Ftp_config.username);
            odt_vts_dbgPrintf("FTP Password : %s", Ftp_config.password);
            odt_vts_dbgPrintf("Filename : %s", Ftp_config.filename);
        }
        else
        {
            odt_vts_dbgPrintf("Unknown file\r\n");
            nwy_file_close(fd);
            return;
        }

        nwy_file_close(fd);
    }
    else
    {
        odt_vts_dbgPrintf("fail read failed");
    }

    }
void Flash_Write(void)
{
    static nwy_dir_info_t flash_dir_hdl;
    nwy_error_e ret;

    /***************OPENING THE DIRECTORY****************** */
    ret = nwy_dir_open(FLASH_DIR_NAME, &flash_dir_hdl);
    if (ret != NWY_SUCCESS) 
    {
        odt_vts_dbgPrintf("dir %s open error\r\n", FLASH_DIR_NAME);
    } 
    else 
    {
        odt_vts_dbgPrintf("dir %s open success,id = %d\r\n", FLASH_DIR_NAME, flash_dir_hdl.fd);
    }

    /* writing the Server parameters*/
    Write_to_File(FLASH_SERVER_CONFIG_FILE);
    Write_to_File(FLASH_OTA_CONFIG_FILE);
}

void Write_to_File(const char *filename)
{
    nwy_error_e file_hdl;

/*****************OPENING SERVER CONFIG FILE*********************** */    
    file_hdl  = nwy_file_open(filename, NWY_WB_PLUS_MODE);
    if (file_hdl < 0) 
    {
        odt_vts_dbgPrintf("file %s open error:%d\r\n", filename, file_hdl);
    } 
    else 
    {
        odt_vts_dbgPrintf("file %s open success:%d\r\n", filename, file_hdl);
    }
/***********WRITING SERVER CONFIG FILE ******************************/  

    if(strcmp(filename, FLASH_SERVER_CONFIG_FILE) == 0)
    {
        int rtn = nwy_file_write(file_hdl, &Http_Config, sizeof(Http_Config));
        if (rtn != sizeof(Http_Config))
            odt_vts_dbgPrintf("file %s write error:%d\r\n", filename, rtn);
        else
            odt_vts_dbgPrintf("file %s write success:%d\r\n", filename, rtn);
    }
    else if(strcmp(filename, FLASH_OTA_CONFIG_FILE) == 0)
    {
        int rtn = nwy_file_write(file_hdl, &Ftp_config, sizeof(Ftp_config));
        if (rtn != sizeof(Ftp_config))
            odt_vts_dbgPrintf("file %s write error:%d\r\n", filename, rtn);
        else
            odt_vts_dbgPrintf("file %s write success:%d\r\n", filename, rtn);
    }

    /**********CLOSE SERVER CONFIG FILE****************************** */  
    nwy_file_close(file_hdl);
}
