#include <stdio.h>
#include <cyg/kernel/kapi.h>
#include <cyg/hal/hal_arch.h>
#include <pkgconf/hal.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_io.h>
#include <pkgconf/system.h>
#include <pkgconf/memalloc.h>
#include <pkgconf/isoinfra.h>
#include <stdlib.h>
#include <sys/time.h>

#include "../include/gw_types.h"
#include "../include/gw_os_api_core.h"

#define BROAD_STORM_THREAD_PRIORITY 25
#define BROADCAST_STORM_THREAD_STACKSIZE (2*4096)
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
	gw_printf("=============================================\n");
	gw_printf("====  gw_borad_storm_thread_create===========\n");
	gw_printf("=============================================\n");
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




		
