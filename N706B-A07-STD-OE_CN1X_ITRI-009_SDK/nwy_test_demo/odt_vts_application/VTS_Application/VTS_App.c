#include "VTS_App.h"
#include "http.h"
#include "nwy_http_api.h"
#include "driver.h"
#include "Network.h"

/* Struct */
VTS_Flags_S      VTS_flags              = {0};
Status_Param_S   Status_Param           = {0}; 
Location_Param_S Loc_param              = {0};
Location_Param_S Prev_Loc_param         = {0};

/* Enum */
FrameType_E     SendMsg                = 0;
FixStatus_E     Fixstate               = 0;
FixStatus_E     Prev_Fixstate          = 0;

/* Integer*/
uint32_t        HeartBeat_interval     = 0;
float           Batt_Volt              = 0;

/* Char Array */
uint8_t      Device_ID[20]            = {0};

static uint16_t Prepare_JsonFrame(const char *fmt, ...)
{
    uint16_t tx_len;

    // Append formatted message
    va_list args;
    va_start(args, fmt);
    tx_len = vsnprintf(HttpS.tx_buf, HTTP_TX_LEN, fmt, args);
    va_end(args);

    return tx_len;  
}

 void Get_Battery_Volt(void)
{
    Batt_Volt = Read_CP_ADC(NWY_ADC_CHANNEL1, NWY_ADC_SCALE_1V250);
    // Batt_Volt /= 1000; // Convert mV to V
    // odt_vts_dbgPrintf("\r\n mV : %.2f", Batt_Volt);
    Batt_Volt *= VBAT_GAIN;
    // odt_vts_dbgPrintf("\r\n Battery Voltage :  %.2f\r\n", Batt_Volt);
}

static void Get_FixStatus(void)
{
    switch(Fixstate)
    {
        case NoFix:
        strcpy(Status_Param.Fix_Status, "no_fix"); // Not enough satellites // No satellite lock
        break;

        case Valid:
        strcpy(Status_Param.Fix_Status, "valid");  // GPS fix (standard)  // GPS fix available
        break;

        case BadFix:
        strcpy(Status_Param.Fix_Status, "bad_fix");  // DGPS or low accuracy  //Low accuracy signal
        break;

        // case Invalid:
        // strcpy(Status_Param.Fix_Status, "invalid"); // Anything else
        // break;
    }
}

static void Set_Navigation_mode(void)
{
    // nwy_loc_mode_set(GPS);
    // nwy_loc_mode_set(GPS_BD_GLONASS);
    nwy_loc_mode_set(GPS_BD_GALILEO);
    nwy_loc_navigation_start();
}

 void Get_Location(void)
{
     nwy_gnss_nmea_info_t gnss_info = {0};

    nwy_loc_nmea_parse(&gnss_info);

    Loc_param.latitude      = gnss_info.latitude;
    Loc_param.longitude     = gnss_info.longitude;
    Loc_param.altitude      = gnss_info.altitude;
    Loc_param.satellites    = gnss_info.satellite_num;
    Loc_param.hdop          = gnss_info.m_nHDOP;

    if(gnss_info.speed >= 3.0)
    {
       Loc_param.speed   = gnss_info.speed; 
       Loc_param.course  = gnss_info.course;
    }
    else
    {
        Loc_param.speed  = 0.0; // gnss_info.speed;
        Loc_param.course = 0.0;
    }

    switch(gnss_info.fix_flag)  
    {
        case 0: 
         strcpy(Loc_param.fix_quality, "No fix"); 
         Fixstate = NoFix;
         break;

        case 1:  
          strcpy(Loc_param.fix_quality, "GPS fix"); 
          Fixstate = Valid;
          break;
          
        case 2:
          strcpy(Loc_param.fix_quality, "DGPS");
          Fixstate = BadFix; 
          break;
        // case 3:  
        //   strcpy(Loc_param.fix_quality, "PPS fix"); 
        //   Fixstate = Invalid; 
          break;
        // case 4:  
        //   strcpy(Loc_param.fix_quality, "RTK Fix"); 
        //   Fixstate = Invalid; 
        //   break;
        case 5:  
          strcpy(Loc_param.fix_quality, "RTK Float"); 
          Fixstate = BadFix; 
          break;
        default: 
        //    strcpy(Loc_param.fix_quality, "Unknown"); 
        //    Fixstate = Invalid; 
           break;
    }
    // nwy_test_cli_echo("\r\n Latitude : %f", Loc_param.latitude);
    // nwy_test_cli_echo("\r\n Longitude: %f", Loc_param.longitude);
    // nwy_test_cli_echo("\r\n Altitude : %f", Loc_param.altitude);
    // nwy_test_cli_echo("\r\n Speed    : %f", Loc_param.speed);
    // nwy_test_cli_echo("\r\n Course   : %f", Loc_param.course);
    // nwy_test_cli_echo("\r\n Satellites: %d", Loc_param.satellites);
    // nwy_test_cli_echo("\r\n FIX FLAG = %s", Loc_param.fix_quality);
    // nwy_test_cli_echo("\r\n locMsg1.m_nHDOP:%f! \r\n", Loc_param.hdop);
}

static unsigned long convert_to_epoch(int year, int month, int day, int hour, int min, int sec) {
  if (month < 3) {
        year -= 1;
        month += 12;
    }
     unsigned long days = 365UL * year + year / 4 - year / 100 + year / 400 +
                         (153 * month - 457) / 5 + day - 719469;

    return (days * 86400UL) + hour * 3600 + min * 60 + sec;
}
static void Get_Date_Time(void)
{
    nwy_time_t                    get_time;
    // unsigned long epoch;

    RTC_Date_Time_Get(&get_time, TZ_UTC);
    sprintf(Loc_param.timestamp,"%04d-%02d-%02dT%02d:%02d:%02dZ",
        get_time.year, get_time.mon, get_time.day,
        get_time.hour, get_time.min, get_time.sec);

    // epoch = convert_to_epoch(get_time.year,get_time.mon, get_time.day,
    //                  get_time.hour, get_time.min, get_time.sec);
    // snprintf((char *)Loc_param.timestamp, sizeof(Loc_param.timestamp), "%s", timestamp_rtc);
    //  nwy_test_cli_echo("\r\n Epoch Time : %s", Loc_param.epoch_time);
}



static void Get_Cell_Info(void)
{
    nwy_nw_cellinfo_t cell_info = {0};
    nwy_nw_cellinfo_get(NWY_SIM_ID_SLOT_1, NWY_NW_GET_SCELL, &cell_info);

    Loc_param.MCC  = cell_info.cell_info.lte_cell.serv_cell.plmn.mcc;
    Loc_param.MNC  = cell_info.cell_info.lte_cell.serv_cell.plmn.mcc;
    Loc_param.CID  = cell_info.cell_info.lte_cell.serv_cell.cell_id;
    Loc_param.RSRP = cell_info.cell_info.lte_cell.serv_cell.rsrp;

    // nwy_test_cli_echo("\r\n MCC  : %d", Loc_param.MCC);
    // nwy_test_cli_echo("\r\n MNC  : %d", Loc_param.MNC);
    // nwy_test_cli_echo("\r\n CID  : %x", Loc_param.CID);
    // nwy_test_cli_echo("\r\n RSRP : %d", Loc_param.RSRP);
}

static void Send_BootNotification(void)
{
    Prepare_http_frame(Bootnotification);
}

static void Send_StatusNotificaton(void)
{
    Prepare_http_frame(Statusnotification);
}

static void Send_HeartBeat(void)
{
    Prepare_http_frame(HeartBeat);
}

static void Send_LocNotification(void)
{
    Prepare_http_frame(Locationnotification);
}

void VTS_App_Polling(void)
{
    Get_Battery_Volt();
    Get_Date_Time();
    Get_Cell_Info();
    Get_Location();

    switch(SendMsg)
    {
        case StartNavigation:
            Set_Navigation_mode();
            SendMsg = Bootnotification;
            break; 

        case Bootnotification:
            Send_BootNotification();
            SendMsg = Idle;
            break;

        case Statusnotification:
            // Get_FixStatus();
            // Send_StatusNotificaton();
            SendMsg = Idle;
            // SendMsg = Locationnotification;
            break;  
   
        case Locationnotification:
            if(!VTS_flags.tx_Flag)
            {
                Send_LocNotification();
                SendMsg = Idle;
            }
            break;

         case Idle:
            break;        
    }

    if(VTS_flags.Heartbeat_Send)
    {
        VTS_flags.Heartbeat_Send = false;
        odt_vts_dbgPrintf("\r\n Latitude : %f", Loc_param.latitude);
        odt_vts_dbgPrintf("\r\n Longitude: %f", Loc_param.longitude);
        odt_vts_dbgPrintf("\r\n Altitude : %f", Loc_param.altitude);
        odt_vts_dbgPrintf("\r\n Speed    : %f", Loc_param.speed);
        odt_vts_dbgPrintf("\r\n Course   : %f", Loc_param.course);
        odt_vts_dbgPrintf("\r\n Satellites: %d", Loc_param.satellites); 
        odt_vts_dbgPrintf("\r\n FIX FLAG = %s", Loc_param.fix_quality);
        odt_vts_dbgPrintf("\r\n locMsg1.m_nHDOP:%f! \r\n", Loc_param.hdop);
        // odt_vts_dbgPrintf("This is the new Chnage\r\n");
        
        Send_HeartBeat();
        // SendMsg = Locationnotification;
    }

    if(Prev_Fixstate != Fixstate)
    {
        // SendMsg = Statusnotification;
        Get_FixStatus();
        Send_StatusNotificaton();
        Prev_Fixstate = Fixstate;
    }

    if((Prev_Loc_param.latitude != Loc_param.latitude) || (Prev_Loc_param.longitude != Loc_param.longitude) ||
       (Prev_Loc_param.course != Loc_param.course) || (Prev_Loc_param.speed != Loc_param.speed)) 
       {
            SendMsg = Locationnotification;
            Prev_Loc_param.latitude  = Loc_param.latitude;
            Prev_Loc_param.longitude = Loc_param.longitude;
            Prev_Loc_param.course    = Loc_param.course;
            Prev_Loc_param.speed     = Loc_param.speed;
       }
    
    if(VTS_flags.OTA_req)
    {
        VTS_State = APP_HTTP_CLOSE;
    }
}
 
void Prepare_http_frame(FrameType_E frameType)
{
    uint16  http_txLen  =  0;

    switch(frameType)
    {
        case Bootnotification:
            http_txLen = Prepare_JsonFrame(
                        "[2,"
                        "\"msg-001\",\"BootNotification\","
                        "{"
                        "\"tracker_id\":\"%s\","
                        "\"firmware_version\":\"%s\""
                        "}]",
                        Device_ID,
                        FW_VERSION
                        );
            break;

        case Statusnotification:

            http_txLen = Prepare_JsonFrame(
                    "[2,"
                    "\"msg-103\",\"StatusNotification\","
                    "{"
                    "\"tracker_id\":\"%s\","
                    "\"fix_status\":\"%s\","
                    "\"satellites\":%d,"
                    "\"hdop\":%.2f,"
                    "\"timestamp\":\"%s\""
                    "}]",
                    Device_ID,
                    Status_Param.Fix_Status,
                    Loc_param.satellites,
                    Loc_param.hdop,
                    Loc_param.timestamp
                );

            break;
        
        case HeartBeat:

        http_txLen = Prepare_JsonFrame(
                "[2,"
                "\"msg-003\",\"Heartbeat\","
                "{"
                "\"tracker_id\":\"%s\","
                "\"battery_level\":%.2f,"
                "\"timestamp\":\"%s\""
                "}]",
                Device_ID,
                Batt_Volt,
                Loc_param.timestamp
               );
            break;

        case Locationnotification:

        http_txLen = Prepare_JsonFrame(
                "[2,"
                "\"msg-101\",\"LocationUpdate\","
                "{"
                "\"tracker_id\":\"%s\","
                "\"latitude\":%.6f,"
                "\"longitude\":%.6f,"
                "\"speed_kmh\":%.2f,"
                "\"course\":%.2f,"
                "\"altitude\":%.2f,"
                "\"satellites\":%d,"
                "\"hdop\":%.2f,"
                "\"timestamp\":\"%s\","
                "\"battery_level\":%.2f"
                "}]",
                Device_ID,
                Loc_param.latitude,
                Loc_param.longitude,
                Loc_param.speed,
                Loc_param.course,
                Loc_param.altitude,
                Loc_param.satellites,
                Loc_param.hdop,
                Loc_param.timestamp,
                Batt_Volt
               );
            break;        

        default:
        break;
    }

    uint8_t temp_uri[50] = {0};
    strcpy(temp_uri, Http_Config.endpoint);
    strcat(temp_uri, Device_ID);
    
    http_post_param_t param = {0};
    param.keepalive = 1;
    // param.uri = ENDPOINT VEHICLE_ID;
    param.uri = temp_uri;
    param.data = HttpS.tx_buf;
    param.len = http_txLen;
    param.type = NWY_HTTP_CONTENT_TYPE_JSON;

    if(nwy_http_post(http_hdl1, &param) == NWY_SUCCESS)
    {
        VTS_flags.tx_Flag = true;
        odt_vts_dbgPrintf("[VTS] Post MSg --->%d: %s\r\n",http_txLen, HttpS.tx_buf);
    }
    else 
    {
       odt_vts_dbgPrintf("[VTS] Post Failed\r\n");
    //    GPIO_Set_Output(70, 0);

    }

    memset(HttpS.tx_buf, 0, sizeof(HttpS.tx_buf));

}

/****************************************** EOF ************************************************************ */


    

