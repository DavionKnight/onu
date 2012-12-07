/*
 * gwdonuif.c
 *
 *  Created on: 2012-11-5
 *      Author: tommy
 */

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "gw_log.h"
#include "gwdonuif_interval.h"
#include "pkt_main.h"

static gwdonu_im_if_t * g_im_ifs = NULL;


static gw_uint32 g_uni_port_num = 0;
static gw_macaddr_t g_sys_mac;

gw_uint8 gw_onu_read_port_num()
{
	gw_log(GW_LOG_LEVEL_DEBUG, "read onu port num: %d\r\n", g_uni_port_num);
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


gw_status init_im_interfaces()
{
	if(g_im_ifs == NULL)
	{
		g_im_ifs = malloc(sizeof(gwdonu_im_if_t));
		if(g_im_ifs)
		{
			memset(g_im_ifs, 0 ,sizeof(gwdonu_im_if_t));
			return GW_OK;
		}
		else
			return GW_ERROR;
	}

	return GW_OK;
}

gw_status reg_gwdonu_im_interfaces(gwdonu_im_if_t * ifs)
{
	gw_status ret = GW_E_ERROR;

	if(ifs)
	{
		if(init_im_interfaces() == GW_OK)
		{
			memcpy(g_im_ifs, ifs, sizeof(gwdonu_im_if_t));

#if 1
{
	int num = sizeof(gwdonu_im_if_t)/sizeof(int);
	int i = 0;
	int * p= (int*)g_im_ifs;
	gw_printf("dump im ifs:\r\n");
	for(i=0; i<num; i++,p++)
		gw_printf("%d    %08x\r\n", i+1, *p);
		
}
#endif

			call_gwdonu_if_api(LIB_IF_SYSINFO_GET, 2,  g_sys_mac, &g_uni_port_num);
			call_gwdonu_if_api(LIB_IF_SPECIAL_PKT_HANDLER_REGIST, 1, gwlib_sendPktToQueue);
			
			ret = GW_E_OK;
		}
	}

	return ret;
}

gw_status call_gwdonu_if_api(gw_int32 type, gw_int32 argc, ...)
{
	va_list ap;
	gw_status ret = GW_ERROR;

	gw_int8 ifname[32] = "";


	if(!g_im_ifs)
	{
		gw_log(GW_LOG_LEVEL_DEBUG, ("onu import api ifs not init!\r\n"));
		return GW_ERROR;
	}

	va_start(ap, argc);

	switch (type) {
		case LIB_IF_ONU_LLID_GET:
			if(g_im_ifs->onullidget)
				ret = (*g_im_ifs->onullidget)(va_arg(ap, gw_uint32*));
			else
				strcpy(ifname, "onu llid get if");
//				gw_log(GW_LOG_LEVEL_DEBUG,("onu llid get if is null!\r\n"));
			break;	
		case LIB_IF_SYSINFO_GET:
			if(g_im_ifs->sysinfoget)
				ret = (*g_im_ifs->sysinfoget)(va_arg(ap, gw_uint8*), va_arg(ap, gw_uint32*));
			else
//				gw_log(GW_LOG_LEVEL_DEBUG,("sys info get if is null!\r\n"));			
				strcpy(ifname, "sys info get");
			break;	
		case LIB_IF_SYSCONF_SAVE:
			if(g_im_ifs->sysconfsave)
				ret = (*g_im_ifs->sysconfsave)(va_arg(ap, gw_uint8*), va_arg(ap, gw_uint32));
			else
//				gw_log(GW_LOG_LEVEL_DEBUG, "sys conf save if is null!\r\n");
				strcpy(ifname, "sys conf save");
			break;
		case LIB_IF_SYSCONF_RESTORE:
			if(g_im_ifs->sysconfrestore)
				ret = (*g_im_ifs->sysconfrestore)(va_arg(ap, gw_uint8*), va_arg(ap, gw_uint32));
//			else
//				gw_log(GW_LOG_LEVEL_DEBUG, "sys conf restore if is null!\r\n");
			break;
		case LIB_IF_PORTSEND:
			if(g_im_ifs->portsend)
				ret = (*g_im_ifs->portsend)(va_arg(ap, gw_uint32), va_arg(ap, gw_uint8 *), va_arg(ap, gw_uint32));
			else
				gw_log(GW_LOG_LEVEL_DEBUG,("port send if is null!\r\n"));
			break;
		case LIB_IF_OAM_HDR_BUILDER:
			if(g_im_ifs->oamhdrbuilder)
				ret = (*g_im_ifs->oamhdrbuilder)(va_arg(ap, gw_uint8 *), va_arg(ap, gw_uint32));
			else
				gw_log(GW_LOG_LEVEL_DEBUG,("port send if is null!\r\n"));
			break;
		case LIB_IF_PORT_ADMIN_GET:
			if(g_im_ifs->portadminget)
				ret = (*g_im_ifs->portadminget)(va_arg(ap, gw_uint32), va_arg(ap, gw_int32*));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "port admin get if is null!\r\n");
			break;
		case LIB_IF_PORT_ADMIN_SET:
			if(g_im_ifs->portadminget)
				ret = (*g_im_ifs->portadminset)(va_arg(ap, gw_uint32), va_arg(ap, gw_int32));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "port admin set if is null!\r\n");
			break;
		case LIB_IF_PORT_OPER_STATUS_GET:
			if(g_im_ifs->portoperstatusget)
				ret = (*g_im_ifs->portoperstatusget)(va_arg(ap, gw_uint32), va_arg(ap, gw_int32*));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "port oper status get if is null!\r\n");
			break;
		case LIB_IF_VLAN_ENTRY_GETNEXT:
			if(g_im_ifs->vlanentrygetnext)
				ret = (*g_im_ifs->vlanentrygetnext)(va_arg(ap, gw_uint32),  va_arg(ap, gw_uint16*),
				va_arg(ap, gw_uint32*), va_arg(ap, gw_uint32*));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "vlan entry getnext if is null!\r\n");
			break;
			
		case LIB_IF_VLAN_ENTRY_GET:
			if(g_im_ifs->vlanentryget)
				ret = (*g_im_ifs->vlanentryget)(va_arg(ap, gw_uint32),  va_arg(ap, gw_uint32*), va_arg(ap, gw_uint32*));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "vlan entry get if is null!\r\n");
			break;
		case LIB_IF_FDB_ENTRY_GET:
			if(g_im_ifs->fdbentryget)
				ret = (*g_im_ifs->fdbentryget)(va_arg(ap, gw_uint32), va_arg(ap, gw_uint8*),
				va_arg(ap, gw_uint32*));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "fdb entry get if is null!\r\n");
			break;

		case LIB_IF_FDB_MGT_MAC_SET:
			if(g_im_ifs->fdbmgtmacset)
				ret = (*g_im_ifs->fdbmgtmacset)(va_arg(ap, gw_uint8*));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "fdb mgt mac set if is null!\r\n");
			break;			
			break;

		case LIB_IF_PORT_LOOP_EVENT_POST:
			if(g_im_ifs->portloopnotify)
				ret = (*g_im_ifs->portloopnotify)(va_arg(ap, gw_uint32));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "port loop event post if is null!\r\n");
			break;
		case LIB_IF_SPECIAL_PKT_HANDLER_REGIST:
			if(g_im_ifs->specialpkthandler)
				ret = (*g_im_ifs->specialpkthandler)(va_arg(ap, libgwdonu_special_frame_handler_t));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "special pkt handler register if is null!\r\n");
			break;
						
		default:
//			gw_log(GW_LOG_LEVEL_DEBUG, "unkonw if called!\r\n");
			break;
	}

	if(!ifname[0])
		gw_log(GW_LOG_LEVEL_DEBUG, "%s is null!\r\n", ifname);

	va_end(ap);

	return ret;
}

