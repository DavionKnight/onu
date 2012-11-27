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

static gw_uint8 g_uni_port_num = 0;
static gw_macaddr_t g_sys_mac;

gw_uint8 gw_onu_read_port_num()
{
	return g_uni_port_num;
}

gw_status gw_onu_get_local_mac( gw_macaddr_t * mac)
{
	if(mac)
	{
		memcpy(mac, g_sys_mac, GW_MACADDR_LEN);
		return GW_RETURN_SUCCESS;
	}
	return GW_RETURN_FAIL;
}

void plat_init()
{
	gw_osal_core_init();

	init_im_interfaces();

	init_pkt_proc();

	gwd_onu_init();

}
