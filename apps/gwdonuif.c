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
#include "../include/gwdonuif.h"
#include "../cli_lib/cli_common.h"
#include "oam.h"
#include "../include/gw_os_api_core.h"
static gwdonu_im_if_t * g_im_ifs = NULL;

gw_uint32 g_uni_port_num = 0;
gw_macaddr_t g_sys_mac;

extern ONU_SYS_INFO_TOTAL gw_onu_system_info_total;
extern gw_uint32 g_pkt_send_sem;

extern int GW_Onu_Sysinfo_Get();
extern void cli_console_start();


gw_int32 gw_onu_poe_api_register_check()
{
    if((g_im_ifs->cpldread == NULL) || (g_im_ifs->cpldwrite == NULL) || (g_im_ifs->poeportoperation == NULL))
    {
        return GW_ERROR;
    }

    return GW_OK;
}
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

gw_status gw_onu_set_local_mac( gw_macaddr_t *mac)
{
	if(mac)
	{
		memcpy(g_sys_mac, mac, GW_MACADDR_LEN);
		return GW_RETURN_SUCCESS;
	}
	else
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

gw_status gwd_onu_out_if_hwver_get(gw_int8 * hwbuf, const gw_int32 len)
{
	if(hwbuf)
	{
		snprintf(hwbuf, len, "%s", gw_onu_system_info_total.hw_version);
		return GW_OK;
	}
	else
		return GW_ERROR;
}

static libgwdonu_console_read_t sr=NULL;
static libgwdonu_console_write_t sw=NULL;

gw_int32 gwd_console_read(gw_uint8 *buf, gw_uint32 count)
{
    gw_int32 ret = 0;

    if(sr)
        ret = (*sr)(buf, count);
    return ret;
}

gw_int32 gwd_console_write(gw_uint8 *buf, gw_uint32 count)
{
    gw_int32 ret = 0;

    if(sw)
        ret = (*sw)(buf, count);

    return ret;
}

gw_status gwd_onu_console_cli_entry(gw_int32 type, gw_int32 fd, libgwdonu_console_read_t r, libgwdonu_console_write_t w)
{
#if 0
    if(r != NULL && w != NULL)
    {
        sr = r;
        sw = w;
        cli_console_start();
        sr = NULL;
        sw = NULL;

        return GW_OK;
    }
    else
        return GW_ERROR;
#endif

    sr = r;
    sw = w;
    cli_console_start(type, fd);
    sr = NULL;
    sw = NULL;

    return GW_OK;
}

gw_status reg_gwdonu_im_interfaces(gwdonu_im_if_t * ifs, gw_int32 size)
{
	gw_status ret = GW_E_ERROR;
	if(ifs)
	{
		if(init_im_interfaces() == GW_OK)
		{
			if(size != sizeof(gwdonu_im_if_t))
			{
				gw_log(GW_LOG_LEVEL_DEBUG, "im ifs vesion not match!!\r\n");
#if 1
				{
					int num = size/sizeof(int);
					int i = 0;
					int * p= (int*)ifs;
					gw_printf("dump im ifs:\r\n");
					for(i=0; i<num; i++,p++)
						gw_printf("%d    %08x\r\n", i+1, *p);

				}
#endif
			}
			else
			{
				memcpy(g_im_ifs, ifs, sizeof(gwdonu_im_if_t));

				call_gwdonu_if_api(LIB_IF_SYSINFO_GET, 2,  g_sys_mac, &g_uni_port_num);
				call_gwdonu_if_api(LIB_IF_SPECIAL_PKT_HANDLER_REGIST, 1, gwlib_sendPktToQueue);
				call_gwdonu_if_api(LIB_IF_ONU_SYSLOG_REGIST, 1, gw_syslog);
				call_gwdonu_if_api(LIB_IF_REG_OUT_HWVER_GET, 1, gwd_onu_out_if_hwver_get);
				call_gwdonu_if_api(LIB_IF_REG_CONSOLE_ENTRY, 1, gwd_onu_console_cli_entry);

				GW_Onu_Sysinfo_Get();
				call_gwdonu_if_api(LIB_IF_ONU_VER_GET, 2, gw_onu_system_info_total.sw_version, sizeof(gw_onu_system_info_total.sw_version));
				ret = GW_E_OK;
			}
		}
	}
	
	return ret;
}

gw_status call_gwdonu_if_api(gw_int32 type, gw_int32 argc, ...)
{
	va_list ap;
	gw_status ret = GW_ERROR;
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
//				strcpy(ifname, "onu llid get if");
				gw_log(GW_LOG_LEVEL_DEBUG,("onu llid get if is null!\r\n"));
			break;	
		case LIB_IF_SYSINFO_GET:
			if(g_im_ifs->sysinfoget)
				ret = (*g_im_ifs->sysinfoget)(va_arg(ap, gw_uint8*), va_arg(ap, gw_uint32*));
			else
				gw_log(GW_LOG_LEVEL_DEBUG,("sys info get if is null!\r\n"));			
//				strcpy(ifname, "sys info get");
			break;	
		case LIB_IF_SWCONF_SAVE:
			if(g_im_ifs->swconfsave)
				ret = (*g_im_ifs->swconfsave)(va_arg(ap, gw_uint8*), va_arg(ap, gw_uint32));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "sw conf save if is null!\r\n");
//				strcpy(ifname, "sys conf save");
			break;
		case LIB_IF_SWCONF_RESTORE:
			if(g_im_ifs->swconfrestore)
				ret = (*g_im_ifs->swconfrestore)(va_arg(ap,gw_uint8*),va_arg(ap,gw_uint32));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "sw conf restore if is null!\r\n");
			break;

		case LIB_IF_SYSCONF_SAVE:
			if(g_im_ifs->sysconfsave)
				ret = (*g_im_ifs->sysconfsave)(va_arg(ap, gw_uint8*), va_arg(ap, gw_uint32));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "sys conf save if is null!\r\n");
//				strcpy(ifname, "sys conf save");
			break;
		case LIB_IF_SYSCONF_RESTORE:
			if(g_im_ifs->sysconfrestore)
				ret = (*g_im_ifs->sysconfrestore)(va_arg(ap,gw_uint8*),va_arg(ap,gw_uint32));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "sys conf restore if is null!\r\n");
			break;

		case LIB_IF_PORTSEND:
			if(g_im_ifs->portsend)
			{
			    gw_semaphore_wait(g_pkt_send_sem, GW_OSAL_WAIT_FOREVER);
				ret = (*g_im_ifs->portsend)(va_arg(ap, gw_uint32), va_arg(ap, gw_uint8 *), va_arg(ap, gw_uint32));
				gw_semaphore_post(g_pkt_send_sem);
			}
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
				ret = (*g_im_ifs->portadminget)(va_arg(ap, gw_uint32), va_arg(ap, gwd_port_admin_t*));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "port admin get if is null!\r\n");
			break;
		case LIB_IF_PORT_ADMIN_SET:
			if(g_im_ifs->portadminset)
				ret = (*g_im_ifs->portadminset)(va_arg(ap, gw_uint32), va_arg(ap, gw_int32));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "port admin set if is null!\r\n");
			break;
		case LIB_IF_PORT_OPER_STATUS_GET:
			if(g_im_ifs->portoperstatusget)
				ret = (*g_im_ifs->portoperstatusget)(va_arg(ap, gw_uint32), va_arg(ap, gwd_port_oper_status_t*));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "port oper status get if is null!\r\n");
			break;
		case LIB_IF_PORT_MODE_GET:
			if(g_im_ifs->portmodeget)
				ret = (*g_im_ifs->portmodeget)(va_arg(ap, gw_uint32), va_arg(ap, gw_int32*), va_arg(ap, gw_int32*), va_arg(ap, gw_int32 *));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "port mode get if is null!\r\n");
			break;			
		case LIB_IF_PORT_MODE_SET:
			if(g_im_ifs->portmodeset)
				ret = (*g_im_ifs->portmodeset)(va_arg(ap, gw_uint32), va_arg(ap, gw_int32), va_arg(ap, gw_int32 ));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "port mode set if is null!\r\n");
			break;	
		case LIB_IF_PORT_ISOLATE_GET:
			if(g_im_ifs->portisolateget)
				ret = (*g_im_ifs->portisolateget)(va_arg(ap, gw_uint32), va_arg(ap, gw_int32*));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "port isolate get if is null!\r\n");
			break;
		case LIB_IF_PORT_ISOLATE_SET:
			if(g_im_ifs->portisolateset)
				ret = (*g_im_ifs->portisolateset)(va_arg(ap, gw_uint32), va_arg(ap, gw_int32));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "port isolate set if is null!\r\n");
			break;			
		case LIB_IF_PORT_STATISTIC_GET:
			if(g_im_ifs->portstatget)
				ret = (*g_im_ifs->portstatget)(va_arg(ap, gw_int32), va_arg(ap, gw_int8 *), va_arg(ap, gw_int32 *));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "port statistic get if is null!\r\n");
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

		case LIB_IF_FDB_ENTRY_GETNEXT:
			if(g_im_ifs->fdbentrygetnext)
				ret = (*g_im_ifs->fdbentrygetnext)(va_arg(ap, gw_uint32), va_arg(ap, gw_uint8*),
				va_arg(ap, gw_uint32*), va_arg(ap, gw_uint8*), va_arg(ap, gw_uint32*),va_arg(ap,gw_uint32*));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "fdb entry getnext if is null!\r\n");
			break;	
		case LIB_IF_FDB_MGT_MAC_SET:
			if(g_im_ifs->fdbmgtmacset)
				ret = (*g_im_ifs->fdbmgtmacset)(va_arg(ap,gw_uint8*));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "fdb mgt mac set if is null!\r\n");
			break;
		case LIB_IF_ATU_LEARN_GET:
			if(g_im_ifs->atulearnget)
				ret = (*g_im_ifs->atulearnget)(va_arg(ap, gw_int32), va_arg(ap, gw_int32*));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "atu learn get if is null!\r\n");
			break;			
		case LIB_IF_ATU_LEARN_SET:
			if(g_im_ifs->atulearnget)
				ret = (*g_im_ifs->atulearnset)(va_arg(ap, gw_int32), va_arg(ap, gw_uint32));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "atu learn set if is null!\r\n");
			break;	
		case LIB_IF_ATU_AGE_GET:
			if(g_im_ifs->atuageget)
				ret = (*g_im_ifs->atuageget)(va_arg(ap, gw_uint32*));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "atu age get if is null!\r\n");
			break;			
		case LIB_IF_ATU_AGE_SET:
			if(g_im_ifs->atuageget)
				ret = (*g_im_ifs->atuageset)(va_arg(ap, gw_uint32));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "atu age set if is null!\r\n");
			break;				
		case LIB_IF_ONU_MAC_SET:
			if(g_im_ifs->onumacset)
				ret = (*g_im_ifs->onumacset)(va_arg(ap, gw_uint8 *));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "onu mac set if is null!\r\n");
			break;

		case LIB_IF_OPM_GET:
			if(g_im_ifs->opmget)
				ret = (*g_im_ifs->opmget)(va_arg(ap, gw_uint16*), va_arg(ap, gw_uint16*), va_arg(ap, gw_uint16*),
				va_arg(ap, gw_uint16*), va_arg(ap, gw_uint16*));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "opm get if is null!\r\n");
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
		case LIB_IF_SYSTERM_CURRENT_TIME_GET:
			if(g_im_ifs->currenttimeget)
				ret = (*g_im_ifs->currenttimeget)(va_arg(ap,gw_uint32*));
			else
				gw_log(GW_LOG_LEVEL_DEBUG,"current timer get if is null!\r\n");
			break;
		case LIB_IF_BROADCAST_SPEED_LIMIT:
			if(g_im_ifs->broadlimit)
				ret = (*g_im_ifs->broadlimit)(va_arg(ap,gw_uint32),va_arg(ap,gwd_sw_port_inratelimit_mode_t),va_arg(ap,gw_uint32));
			else
				gw_log(GW_LOG_LEVEL_DEBUG,"set broadcast speed limit if is null!\r\n");
			break;
		case LIB_IF_LOCALTIME_GET:
			if(g_im_ifs->localtimeget)
				ret = (*g_im_ifs->localtimeget)(va_arg(ap,localtime_tm*));
			else
				gw_log(GW_LOG_LEVEL_DEBUG,"get local time if is null!\r\n");
			break;
		case LIB_IF_STATIC_MAC_ADD:
			if(g_im_ifs->staticmacadd)
				ret = (*g_im_ifs->staticmacadd)(va_arg(ap,gw_int8*),va_arg(ap,gw_uint32),va_arg(ap,gw_uint32));
			else
				gw_log(GW_LOG_LEVEL_DEBUG,"add static mac if is null!\r\n");
			break;
		case LIB_IF_STATIC_MAC_DEL:
			if(g_im_ifs->staticmacdel)
				ret = (*g_im_ifs->staticmacdel)(va_arg(ap,gw_int8*),va_arg(ap,gw_uint32));
			else
				gw_log(GW_LOG_LEVEL_DEBUG,"del static mac if is null!\r\n");
			break;
		case LIB_IF_ONU_REGISTER_GET:
			if(g_im_ifs->registerget)
				ret = (*g_im_ifs->registerget)(va_arg(ap,gw_uint8*));
			else
			{
				gw_log(GW_LOG_LEVEL_DEBUG,"get onu register if is null!\r\n");
			}
			break;
			#ifndef CYG_LINUX
		case LIB_IF_ONU_REBOOT:
			if(g_im_ifs->onureset)
				ret = (*g_im_ifs->onureset)(va_arg(ap,gw_int32));
			else
				gw_log(GW_LOG_LEVEL_DEBUG,"onu reboot if is null!\r\n");
			break;
			#endif

		case LIB_IF_ONU_START_LOOP_LED:
			if(g_im_ifs->startloopled)
			{
				(*g_im_ifs->startloopled)();
				ret = GW_OK;
			}
//			else
//				gw_log(GW_LOG_LEVEL_DEBUG,"get start loop led if is null!\r\n");
			break;

		case LIB_IF_ONU_STOP_LOOP_LED:
			if(g_im_ifs->stoploopled)
			{
				(*g_im_ifs->stoploopled)();
				ret = GW_OK;
			}
//			else
//				gw_log(GW_LOG_LEVEL_DEBUG,"get stop loop led if is null!\r\n");
			break;


		case LIB_IF_PORT_PVID_GET:
			if(g_im_ifs->portpvidget)
				ret = (*g_im_ifs->portpvidget)(va_arg(ap, gw_int32), va_arg(ap, gw_int16*));
			else
			{
				gw_log(GW_LOG_LEVEL_DEBUG, "pvid get if is null!\r\n");
			}
			break;

		case LIB_IF_OLT_MAC_GET:
			if(g_im_ifs->oltmacget)
				ret = (*g_im_ifs->oltmacget)(va_arg(ap, gw_uint8*));
			else
			{
				gw_log(GW_LOG_LEVEL_DEBUG, "olt mac get if is null!\r\n");
			}
			break;
		case LIB_IF_ONU_VER_GET:

			if(g_im_ifs->onuverget)
				ret = (*g_im_ifs->onuverget)(va_arg(ap, gw_int8*),
						va_arg(ap, const gw_int32));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "onu ver get if is null!\r\n");

			break;

		case LIB_IF_LASER_GET:
			if(g_im_ifs->laserget)
				ret =(*g_im_ifs->laserget)(va_arg(ap, gw_EponTxLaserStatus*));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "onu laser get if is null!\r\n");
			break;
		case LIB_IF_LASER_SET:
			if(g_im_ifs->laserset)
				ret =(*g_im_ifs->laserset)(va_arg(ap, gw_int32));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "onu laser set if is null!\r\n");			
			break;
		case LIB_IF_REG_OUT_HWVER_GET:
			if(g_im_ifs->onuhwverget)
				return (*g_im_ifs->onuhwverget)(va_arg(ap, void*));
			else
				gw_log(GW_LOG_LEVEL_DEBUG,"onu register out hw ver if null!\r\n");
			break;

		case LIB_IF_ONU_SYSLOG_REGIST:
			if(g_im_ifs->sysloghandler)
				ret=(*g_im_ifs->sysloghandler)(va_arg(ap,libgwdonu_syslog_heandler_t));
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "gwonu syslog handler if is null!\r\n");
			break;
		case LIB_IF_REG_CONSOLE_ENTRY:
		    if(g_im_ifs->console_cli_register)
		        ret = (*g_im_ifs->console_cli_register)(va_arg(ap, void*));
		    break;
		case LIB_IF_VFILE_OPEN:
			if(g_im_ifs->vfileopen)
				ret = (*g_im_ifs->vfileopen)(va_arg(ap, gw_uint8*), va_arg(ap, gw_int32), va_arg(ap, gw_int32*), va_arg(ap, gw_uint8 **));
			else
				printf("gwdonu_vfile_open is NULL\n");
			break;

		case LIB_IF_VFILE_CLOSE:
			if(g_im_ifs->vfileclose)
				ret = (*g_im_ifs->vfileclose)(va_arg(ap, void*));
			else
				printf("gwdonu_vfile_close is NULL\n");
			break;
		case LIB_IF_QOS_VLAN_QUEUE_MAP:
			if(g_im_ifs->qosvlanqueuemap)
				ret = (*g_im_ifs->qosvlanqueuemap)(va_arg(ap, gw_int32), va_arg(ap, gw_qos_vlan_queue_data_t *));
			else
				printf("gwdonu qos vlan queue map is NULL\n");
			break;
		case LIB_IF_CONF_WR_FLASH:
			if(g_im_ifs->wrflash)
				ret = (*g_im_ifs->wrflash)();
			else
				printf("gwdonu write flash is NULL\n");
			break;

#if(RPU_MODULE_POE == RPU_YES)
        case LIB_IF_CPLD_REGISTER_READ:
            if(g_im_ifs->cpldread)
                ret = (*g_im_ifs->cpldread)(va_arg(ap,gw_uint32),va_arg(ap,gw_uint8*));
            else
                printf("gwdonu read cpld register is NULL\n");
            break;

        case LIB_IF_CPLD_REGISTER_WRITE:
            if(g_im_ifs->cpldwrite)
                ret = (*g_im_ifs->cpldwrite)(va_arg(ap,gw_uint32),va_arg(ap,gw_uint32));
            else
                printf("gwdonu write cpld register is NULL\n");
            break;

        case LIB_IF_POE_PORT_OPERATION_SET:
            if(g_im_ifs->poeportoperation)
                ret = (*g_im_ifs->poeportoperation)(va_arg(ap,gw_int32),va_arg(ap,gw_int32));
            else
                printf("gwdonu set poe port operation is NULL\n");
            break;
#endif
		default:
//			gw_log(GW_LOG_LEVEL_DEBUG, "unkonw if called!\r\n");		
			break;
	}

//	if(!ifname[0])
//		gw_log(GW_LOG_LEVEL_DEBUG, "%s is null!\r\n", ifname);

	va_end(ap);

	return ret;
}

