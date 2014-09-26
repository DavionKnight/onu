
/*
 *
 * gw_os_common.h
 *
 */

#ifndef _GW_OS_COMMON_
#define _GW_OS_COMMON_

/*
** Common include files
*/
//#include "iros_config.h"
#ifdef printf
#undef printf
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>
#include "stdlib.h"
#include "string.h"

#include "gw_os_config.h"
#include "gw_types.h"
#include "gw_os_api_core.h"
#include "util_list.h"



/* Utility for return codes */
#define GW_ERR_DEFINE(mod, sub_mod, code)   ( ((mod)<<24) | ((sub_mod)<<16) | (code) )
#define GW_ERR_DEFINE_OSAL(code)            (  GW_ERR_DEFINE(/*IROS_MID_OSAL*/0, 0, code) )


#define GW_E_OSAL_OK                    GW_E_OK
#define GW_E_OSAL_ERR                   GW_E_ERROR
#define GW_E_OSAL_INVALID_POINTER       GW_ERR_DEFINE_OSAL(0x1)
#define GW_E_OSAL_ERR_ADDR_MISALIGNED   GW_ERR_DEFINE_OSAL(0x2)
#define GW_E_OSAL_ERR_TIMEOUT           GW_ERR_DEFINE_OSAL(0x3)
#define GW_E_OSAL_INVALID_INT_NUM       GW_ERR_DEFINE_OSAL(0x4)
#define GW_E_OSAL_SEM_FAILURE           GW_ERR_DEFINE_OSAL(0x5)
#define GW_E_OSAL_SEM_TIMEOUT           GW_ERR_DEFINE_OSAL(0x6)
#define GW_E_OSAL_QUEUE_EMPTY           GW_ERR_DEFINE_OSAL(0x7)
#define GW_E_OSAL_QUEUE_FULL            GW_ERR_DEFINE_OSAL(0x8)
#define GW_E_OSAL_QUEUE_TIMEOUT         GW_ERR_DEFINE_OSAL(0x9)
#define GW_E_OSAL_QUEUE_INVALID_SIZE    GW_ERR_DEFINE_OSAL(0xA)
#define GW_E_OSAL_QUEUE_ID_ERROR        GW_ERR_DEFINE_OSAL(0xB)
#define GW_E_OSAL_ERR_NAME_TOO_LONG     GW_ERR_DEFINE_OSAL(0xC)
#define GW_E_OSAL_ERR_NO_FREE_IDS       GW_ERR_DEFINE_OSAL(0xD)
#define GW_E_OSAL_ERR_NAME_TAKEN        GW_ERR_DEFINE_OSAL(0xE)
#define GW_E_OSAL_ERR_INVALID_ID        GW_ERR_DEFINE_OSAL(0xF)
#define GW_E_OSAL_ERR_NAME_NOT_FOUND    GW_ERR_DEFINE_OSAL(0x10)
#define GW_E_OSAL_ERR_SEM_NOT_FULL      GW_ERR_DEFINE_OSAL(0x11)
#define GW_E_OSAL_ERR_INVALID_PRIORITY  GW_ERR_DEFINE_OSAL(0x12)

typedef void (*gw_io_hdl_t)(gw_uint8*, gw_uint32);

extern void gw_sys_capability_show();
extern void gw_bit_set(gw_int8 *pbuf , gw_int32 bit_pos);
extern void gw_bit_clr(gw_int8 *pbuf , gw_int32 bit_pos);
extern gw_int32 gw_bit_tst(gw_int8 *pbuf , gw_int32 bit_pos);
extern gw_int32 gw_bit_get_0(gw_int8 *pbuf , gw_int32 total_bit);
extern gw_int32 gw_bit_get_1(gw_int8 *pbuf , gw_int32 total_bit);
extern gw_int32 gw_bit_tst_all(gw_int8 *pbuf , gw_int32 total_bit);


#endif

