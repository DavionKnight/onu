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

void plat_init()
{
	gw_osal_core_init();

	init_im_interfaces();

	init_pkt_proc();

	gwd_onu_init();

}
