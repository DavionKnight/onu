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
extern void init_oam_pty();
extern int Gwd_func_dhcp_pkt_process_init();
extern int Gwd_func_tlv_static_mac_init();
void plat_init()
{
    unsigned int stat_val;
    unsigned int uni_port_num = 0;
    unsigned int lport = 0;
    unsigned int stat = 0;
    
	gw_osal_core_init();

	if(gw_timer_init() == GW_OK)
		gw_printf("gw timer init ok!\r\n");

	init_im_interfaces();

	gw_conf_file_init();
#if(RPU_MODULE_POE == RPU_YES)
    gwd_onu_poe_cpld_check(); 
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

	init_oam_pty();
	init_pkt_proc();
	gwd_onu_init();
	gw_qos_init();
    gw_port_ioslation_init();
    Gwd_func_tlv_static_mac_init();
#ifdef __IPCONFIG__
    gw_onu_ifconfig_init();
#endif
	gw_conf_restore();
	Gwd_func_dhcp_pkt_process_init();
	gwd_thread_init();

}
