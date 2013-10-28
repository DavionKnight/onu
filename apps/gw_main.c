/*
 * gw_main.c
 *
 *  Created on: 2012-11-7
 *      Author: tommy
 */

#include "../include/gw_os_api_core.h"
#include "gwdonuif_interval.h"
#include "pkt_main.h"

#include "gw_conf_file.h"
#include "../qos/qos.h"

void gwd_onu_init();
extern void gwd_thread_init(void);

void plat_init()
{
	gw_osal_core_init();
	if(gw_timer_init() == GW_OK)
		gw_printf("gw timer init ok!\r\n");

	init_im_interfaces();

	gw_conf_file_init();
#if(RPU_MODULE_POE == RPU_YES)
    gw_poe_config_init();
    gwd_onu_poe_cpld_cheak();   
#endif
    
	gw_qos_init();

	gw_conf_restore();

	init_pkt_proc();

	gwd_onu_init();

	gwd_thread_init();

}
