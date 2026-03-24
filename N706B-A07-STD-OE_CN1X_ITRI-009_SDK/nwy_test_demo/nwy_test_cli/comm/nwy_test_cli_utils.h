
/*
 *****************************************************************************
 * Copyright (c) 2023, Neoway Tech. Co., Ltd. All rights reserved.
 *
 * File Name    : nwy_test_cli_adpt.h
 * Author       : hujun
 * Created      : 2023-5-22
 * Description  : nwy_test_cli_adpt API ��������
 *
 *****************************************************************************
 */

#ifndef __NWY_TEST_CLI_UTILS_H__
#define __NWY_TEST_CLI_UTILS_H__
/*
 *****************************************************************************
 * 1 Other Header File Including
 *****************************************************************************
 */
#include "stdlib.h"
#include "stdarg.h"
#include "string.h"
#include "stdio.h"
#include "nwy_test_cli_adpt.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*
 ********************************************   *********************************
 * 2 Macro Definition
 ****************************************************************************
 */
#define NWY_UART_RECV_SINGLE_MAX            512
#define NWY_EXT_SIO_RX_MAX                  (2*1024)
#define CERT_EFS_CHECKSUM_STR_SIZE          4
#define NWY_CLI_LOG(...)                    nwy_test_cli_dbg(__func__, __LINE__, __VA_ARGS__)
#define NWY_EXT_INPUT_RECV_MSG              (NWY_APP_EVENT_ID_BASE + 1)
#define NWY_EXT_DATA_REC_END_MSG            (NWY_APP_EVENT_ID_BASE + 2)

/*
 *****************************************************************************
 * 3 Enum Type Definition
 *****************************************************************************
 */


/*
 *****************************************************************************
 * 4 Global Variable Declaring
 *****************************************************************************
 */




/*
 *****************************************************************************
 * 5 STRUCT Type Definition
 *****************************************************************************
 */
typedef void (*nwy_sio_trans_cb) (const char *trans_data, int length);


/*
 *****************************************************************************
 * 6 UNION Type Definition
 *****************************************************************************
 */

/*
 *****************************************************************************
 * 7 OTHERS Definition
 *****************************************************************************
 */


/*
 *****************************************************************************
 * 8 Function Declare
 *****************************************************************************
 */


/*
*****************************************************************************
* Prototype     : nwy_test_cli_echo
* Description   : ���������Ϣ
* Input         : fmt��log��ʽ��
* Output        : NA
* Return Value  : NA
* Author        : hujun
*****************************************************************************
*/
void nwy_test_cli_echo(char* fmt, ...);

/*
*****************************************************************************
* Prototype     : nwy_test_cli_output
* Description   : ���ڵȴ�������Ϣ
* Input         : msg����ʾ��Ϣ
* Output        : NA
* Return Value  : ���������ַ�������Ϣ
* Author        : hujun
*****************************************************************************
*/
char* nwy_test_cli_input_gets(char* msg, ...);

/*
*****************************************************************************
* Prototype     : nwy_test_cli_output
* Description   : ���ڵȴ�������Ϣ
* Input         : msg����ʾ��Ϣ
* Output        : NA
* Return Value  : ���������ַ�������Ϣ
* Author        : hujun
*****************************************************************************
*/
void nwy_test_cli_sio_data_proc(const char *data, uint32 length);

/*
*****************************************************************************
* Prototype     : nwy_test_cli_input_len_gets
* Description   : ��ȡ������Ϣ����
* Input         : NA
* Output        : NA
* Return Value  : ������Ϣ����
* Author        : hujun
*****************************************************************************
*/
int nwy_test_cli_input_len_gets(void);

/*
*****************************************************************************
* Prototype     : nwy_test_cli_sio_enter_trans_mode
* Description   : ע�����ݴ���ģʽ����
* Input         : recv_cb���������ݴ������ݴ�������
* Output        : NA
* Return Value  : 0:�ɹ�
                  ������ʧ��
* Author        : hujun
*****************************************************************************
*/
int nwy_test_cli_sio_enter_trans_mode(nwy_sio_trans_cb recv_cb);

/*
*****************************************************************************
* Prototype     : nwy_test_cli_sio_quit_trans_mode
* Description   :�˳�����ģʽ����
* Input         : recv_cb���������ݴ������ݴ�������
* Output        : NA
* Return Value  : NA
* Author        : hujun
*****************************************************************************
*/
void nwy_test_cli_sio_quit_trans_mode(void);
/*
*****************************************************************************
* Prototype     : nwy_cli_str_to_int
* Description   :Convert string to integer
* Input         : The integer string to be processed
* Output        : Integer data
* Return Value  : NA
* Author        : hujun
*****************************************************************************
*/
int nwy_cli_str_to_int(const char* str);

/*
*****************************************************************************
* Prototype     : nwy_cli_get_trans_data
* Description   :Obtain transparent data
* Input         : data��Enter the address of the data to be saved, with a data space size of length
                :length:The size of transparent data transmission
* Output        : data:Output transparent data
* Return Value  : NA
* Author        : hujun
*****************************************************************************
*/
int nwy_cli_get_trans_data(char *data,int length);


#endif
