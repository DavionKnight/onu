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
#include <stdio.h>
void gwd_onu_init();
extern void gwd_thread_init(void);

void plat_init()
{
    unsigned int stat_val;
    unsigned int uni_port_num = 0;
    unsigned int lport = 0;
    unsigned int stat = 0;
	printf("%s %d [1]\r\n",__func__,__LINE__);
	gw_osal_core_init();
	printf("%s %d [2]\r\n",__func__,__LINE__);

	if(gw_timer_init() == GW_OK)
		gw_printf("gw timer init ok!\r\n");

	init_im_interfaces();
	printf("%s %d [3]\r\n",__func__,__LINE__);
	gw_conf_file_init();

#if(RPU_MODULE_POE == RPU_YES)
    gwd_onu_poe_cpld_cheak();
    Gwd_onu_poe_exist_stat_get(&stat_val);
    if(stat_val)
    {
        uni_port_num = gw_onu_read_port_num();
        gw_poe_config_init();
        for(lport = 1; lport <= uni_port_num; lport++)
        {
            stat = 0;
            Gwd_onu_port_poe_operation_stat_set(lport,stat);
        }
    }
#endif

	printf("%s %d [4]\r\n",__func__,__LINE__);
	gw_qos_init();
	printf("%s %d [5]\r\n",__func__,__LINE__);
	gw_conf_restore();
	printf("%s %d [6]\r\n",__func__,__LINE__);
	init_pkt_proc();
	printf("%s %d [7]\r\n",__func__,__LINE__);
	gwd_onu_init();
	printf("%s %d [8]\r\n",__func__,__LINE__);
	gwd_thread_init();
	printf("%s %d [9]\r\n",__func__,__LINE__);

}
