

#ifndef _osal_confige_h_
#define _osal_confige_h_

#include "gw_config.h"
/*
** Platform Configuration Parameters for the OS API
*/
#define GW_OSAL_MAX_THREAD              32
#define GW_OSAL_MAX_COUNT_SEM           50
#define GW_OSAL_MAX_PRI_QUEUE           2
#define GW_OSAL_MAX_QUEUE               10
#define GW_OSAL_MAX_MEMPOOL             (20+GW_OSAL_MAX_QUEUE)
#define GW_OSAL_MAX_MUTEX               (30+GW_OSAL_MAX_MEMPOOL)

#define GW_NORMAL_QUEUE_TYPE 0
#define GW_PRI_QUEUE_TYPE 1
/*
** Maximum number for timer
*/
#define GW_OSAL_MAX_TIMER               128

/*
** Maximum length for an absolute path name
*/
#define GW_OSAL_MAX_PATH_LEN     64

/*
** The maxium length allowed for a object (task,queue....) name
*/
#define GW_OSAL_MAX_API_NAME     20

#define GW_OSAL_MAX_QUEUE_PRI  8
/*
** The maximum length for a file name
*/
#define GW_OSAL_MAX_FILE_NAME    20

/*
** These defines are for OS_printf
*/
#define GW_OSAL_BUFFER_SIZE         172
#define GW_OSAL_BUFFER_MSG_DEPTH    100

#define __BIG_ENDIAN__


#endif /* _osal_confige_h_ */

