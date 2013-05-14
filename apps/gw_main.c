/*
 * gw_main.c
 *
 *  Created on: 2012-11-7
 *      Author: tommy
 */

#include "../include/gw_os_api_core.h"
#include "gwdonuif_interval.h"
#include "pkt_main.h"

void gwd_onu_init();
extern void gwd_thread_init(void);
extern gw_int32 gw_timer_init();
void plat_init()
{
	gw_osal_core_init();

	init_im_interfaces();
	gw_timer_init();
	init_pkt_proc();

	gwd_onu_init();
	gwd_thread_init();

}
