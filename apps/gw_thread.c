#include <stdio.h>
#ifdef CYG_LINUX
#include <cyg/kernel/kapi.h>
#include <cyg/hal/hal_arch.h>
#include <pkgconf/hal.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_io.h>
#include <pkgconf/system.h>
#include <pkgconf/memalloc.h>
#include <pkgconf/isoinfra.h>
#endif
#include <stdlib.h>
#include <sys/time.h>

#include "../include/gw_types.h"
#include "../include/gw_os_api_core.h"

#define BROAD_STORM_THREAD_PRIORITY GW_OSAL_THREAD_PRIO_HIGH
#define BROADCAST_STORM_THREAD_STACKSIZE GW_OSAL_THREAD_STACK_SIZE_NORMAL
gw_uint32 gw_broad_storm_id;
extern void broad_storm_thread(void* data);
/****************************************************
**	 timer:2013:02:20 11:50
**	 �㲥�籩�����̴߳���
** 	create ONU broadcast storm thread
** 	����:wangzhiwei
****************************************************/
void gwd_thread_init(void)
{
	if(gw_thread_create(&gw_broad_storm_id,
				  "gwd_broad_storm",
	              broad_storm_thread,
	              NULL,              
	              BROADCAST_STORM_THREAD_STACKSIZE,
	              BROAD_STORM_THREAD_PRIORITY,
	              0) != GW_OK)
	{
		gw_printf("create thread broad storm fail\n");
	}
	else
	{
		gw_printf("create thread broad storm succuss\n");
	}
}




		
