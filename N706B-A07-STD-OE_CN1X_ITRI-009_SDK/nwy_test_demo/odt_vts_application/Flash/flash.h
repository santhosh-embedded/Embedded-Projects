#ifndef FLASH_H
#define FLASH_H

#include "stdlib.h"
#include "stdarg.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"


#define FLASH_DIR_NAME               "VTSflash"
#define FLASH_SERVER_CONFIG_FILE     "ServerConfig"
#define FLASH_OTA_CONFIG_FILE        "OTAConfig"

// Add your code here

uint8_t Check_VTSflash(void);
void Make_VTSflash_Dir(void);
void Write_to_File(const char *filename);
void Flash_Read(void);
void Flash_Write(void);




#endif // FLASH_H