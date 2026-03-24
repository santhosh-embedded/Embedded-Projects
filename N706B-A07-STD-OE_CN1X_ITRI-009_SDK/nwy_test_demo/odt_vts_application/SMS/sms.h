#ifndef SMS_H
#define SMS_H


#include "nwy_sms_api.h"
extern char received_version[20];

// Your code here

void Init_SMS(void);
void Read_SMS(void);
void handle_server_sms(char *sms);
void nwy_sms_test_recv_cb(nwy_sim_id_e simid, nwy_mt_sms_event_e urc_type, nwy_sms_info_ind_t *ind_struct);

#endif // SMS_H