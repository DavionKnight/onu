
#ifdef CYG_LINUX
#include <network.h>
#include <cyg/kernel/kapi.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_io.h>
#include <cyg/io/file.h>
#include <pkgconf/hal.h>
#include <pkgconf/system.h>
#include <pkgconf/memalloc.h>
#include <pkgconf/isoinfra.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socketvar.h>
#include <cyg/fileio/fileio.h>
#include <pkgconf/io_fileio.h>
#include <cyg/io/file.h>
#endif
#include "../include/gw_types.h"
#include "oam.h"
#include "../include/gw_os_api_core.h"
#include "../include/gwdonuif.h"
#include "gwdonuif_interval.h"
#include "gw_log.h"

//#define __DEBUG__

extern int CommOnuMsgSend(unsigned char GwOpcode, unsigned int SendSerNo, unsigned char *pSentData,const unsigned short SendDataSize, unsigned char  *pSessionIdfield);
extern gw_macaddr_t g_sys_mac;
extern gw_uint32 g_uni_port_num;


#define NUM_PORTS_PER_SYSTEM 26
gw_uint64 gulCurrentpktCntIn[NUM_PORTS_PER_SYSTEM-1] = {0};
gw_uint64 gulCurrentpktCntOut[NUM_PORTS_PER_SYSTEM-1] = {0};
gw_uint64 gulOctRateIn[NUM_PORTS_PER_SYSTEM-1] = {0};
gw_uint64 gulOctRateOut[NUM_PORTS_PER_SYSTEM-1] = {0};
gw_uint32 gulLastTick4PortMon[NUM_PORTS_PER_SYSTEM-1] = {0};
gw_uint32 gulCurrentTick4PortMon[NUM_PORTS_PER_SYSTEM-1] = {0};
#define ENABLE 1

gw_return_code_t gwd_onu_port_bcstorm_date_clear(gw_uint32 port)
{
	gulCurrentpktCntOut[port]= 0;
	gulCurrentpktCntIn[port] = 0;
	gulOctRateOut[port] = 0;
	gulOctRateIn[port] = 0;
	gulCurrentTick4PortMon[port] = 0;
	gulLastTick4PortMon[port] = 0;
	return GW_RETURN_SUCCESS;
}
gw_return_code_t gwd_onu_sw_bcstorm_msg_send(gw_uint32 slot, gw_uint32 port,gw_uint8 operate, gw_uint8 state,gw_uint8*session)
{
	char temp[16]={0};

	*(temp) = 22;
	*(temp+3) = slot;
	*(temp+4) = port;
	*(temp+5) = state;
	*(temp+6) = operate;
	if(GW_OK != CommOnuMsgSend(ALARM_REQ,0,temp,7, session))
		{
			gw_printf("Send ALARM_REQ for port %d broadcast storm detect trap failed\n", port);
			return GW_RETURN_FAIL;
		}
	return GW_RETURN_SUCCESS;
}
gw_return_code_t gwd_logical_to_physical(gw_uint32 lport, gw_uint32 *pport)
{
    *pport = (lport-1);

    /* check whether logical port number is legal or not */
    if ((*pport < 0) || (*pport >= NUM_PORTS_PER_SYSTEM)) {
        return GW_RETURN_FAIL;
    }
    return GW_RETURN_SUCCESS;
}
gw_return_code_t gwd_port_current_pkt_status_save(gw_uint32 port,gw_onu_counter_t stat_date)
{
	gulCurrentpktCntIn[port] = stat_date.RxBroadcasts;
	gulOctRateOut[port] = stat_date.TxBroadcasts;
	return GW_RETURN_SUCCESS;
}
gw_return_code_t gwd_port_rate_update(gw_uint32 port)
{
	gw_uint64 ulIntervalTick;
	gw_float fRate;
	gw_int32 len = sizeof(gw_onu_port_counter_t);
	gw_onu_port_counter_t * pd;
	char *data = malloc(len);
	if (data) {
			memset(data, 0, len);
			if (GW_OK != call_gwdonu_if_api(LIB_IF_PORT_STATISTIC_GET, 3, port+1,
							data, &len))
				{
					gw_printf("get port %d statistic fail!\r\n", port+1);
				}
			else
				{
					 pd = (gw_onu_port_counter_t*)data;
				}
		}
	gw_log(GW_LOG_LEVEL_INFO, "port %d broadcoast rx counter: %llu\r\n", port+1, pd->counter.RxBroadcasts);
#ifdef __NOT_USE__
	if(!gwd_portstats_get_current(port, stat_date)){
	gw_printf("get gwd current port status fail\n");
	return GW_RETURN_FAIL;
	}
#endif
	call_gwdonu_if_api(LIB_IF_SYSTERM_CURRENT_TIME_GET, 1, &gulCurrentTick4PortMon[port]);
	gw_log(GW_LOG_LEVEL_INFO, "port %d time flag: %u\r\n", port+1, gulCurrentTick4PortMon[port]);
#ifdef __NOT_USE__
	gulCurrentTick4PortMon[port] = cyg_current_time();
#endif
	if (gulCurrentTick4PortMon[port] > gulLastTick4PortMon[port])
		{
    		ulIntervalTick = gulCurrentTick4PortMon[port] - gulLastTick4PortMon[port];
			#ifdef __DEBUG__
			if(port == 0)
				{
					gw_printf("tick:%lld\n",ulIntervalTick);
				}
			#endif
		}
    else
    	{
    		ulIntervalTick = 0xFFFFFFFF - (gulLastTick4PortMon[port] - gulCurrentTick4PortMon[port]);
			#ifdef __DEBUG__
			if(port == 0)
				{
					gw_printf("else tick:%lld\n",ulIntervalTick);
				}
			#endif
    	}

	if (pd->counter.RxBroadcasts >= gulCurrentpktCntIn[port] )
	{
	#ifdef __DEBUG__
		if(port == 0)
			{
				gw_printf("RX RATE:%lld\n",pd->counter.RxBroadcasts);
				gw_printf("His RATE:%lld\n",gulCurrentpktCntIn[port]);
				gw_printf("rate:%lld\n",(pd->counter.RxBroadcasts - gulCurrentpktCntIn[port]));
			}
	#endif
		fRate = (gw_float)((pd->counter.RxBroadcasts - gulCurrentpktCntIn[port]))/(gw_float)ulIntervalTick*100;
	}
	else
	{
	#ifdef __DEBUG__
			if(port == 0)
			{
				gw_printf("else RX RATE:%lld\n",pd->counter.RxBroadcasts);
				gw_printf("else His RATE:%lld\n",gulCurrentpktCntIn[port]);
				gw_printf("else rate:%lld\n",(pd->counter.RxBroadcasts - gulCurrentpktCntIn[port]));
			}
	#endif
		fRate = (gw_float)((0xFFFFFFFF - (gulCurrentpktCntIn[port] - pd->counter.RxBroadcasts)))/(gw_float)ulIntervalTick*1000;
	}

	gulCurrentpktCntIn[port] = pd->counter.RxBroadcasts;
	#ifdef __DEBUG__
	if(port == 0)
		{
			gw_printf("frate:%lld\n",(gw_uint64)fRate);
		}
	#endif
	gulOctRateIn[port] = (gw_uint64)fRate;

	gw_log(GW_LOG_LEVEL_INFO, "port %d broadcoast rate: %llu\r\n", port+1, gulOctRateIn[port]);
	if ( pd->counter.TxBroadcasts >= gulCurrentpktCntOut[port] )
	{
		fRate = (gw_float)((pd->counter.TxBroadcasts - gulCurrentpktCntOut[port]))/(gw_float)ulIntervalTick*100;
	}
	else
	{
		fRate = (gw_float)((0xFFFFFFFF - (gulCurrentpktCntOut[port] - pd->counter.TxBroadcasts)))/(gw_float)ulIntervalTick*1000;
	}
	gulCurrentpktCntOut[port] = pd->counter.TxBroadcasts;
	gulOctRateOut[port] = (gw_uint64)fRate;
	gulLastTick4PortMon[port] = gulCurrentTick4PortMon[port];
	free(data);
	return GW_RETURN_SUCCESS;

}
void broad_storm_thread(void* data)
{
    gw_uint32 logical_port;
    gw_uint32 physical_port;
    gwd_port_admin_t port_status;
    gw_return_code_t ret;
    gw_uint32 slot = 1;
    gw_uint8 OAMsession[8] = "";
    gw_uint16 ulBcStormEventCnt[NUM_PORTS_PER_SYSTEM - 1] =
        { 0 };
    gw_uint16 ulBcStormStopCnt[NUM_PORTS_PER_SYSTEM - 1] =
        { 0 };
    gw_uint16 havebroadcaststorm[NUM_PORTS_PER_SYSTEM - 1] =
        { 0 };
    gw_uint16 havebroadcaststorm_end[NUM_PORTS_PER_SYSTEM - 1] =
        { 0 };
    gw_uint16 timeCouter[NUM_PORTS_PER_SYSTEM - 1] =
        { 0 };
    gw_uint16 startCouter[NUM_PORTS_PER_SYSTEM - 1] =
        { 0 };
    call_gwdonu_if_api(LIB_IF_SYSINFO_GET, 2, g_sys_mac, &g_uni_port_num);
    while (GW_TRUE)
    {
        for (logical_port = 1; logical_port <= gw_onu_read_port_num(); logical_port++)
        {
            if (gwd_logical_to_physical(logical_port, &physical_port))
                continue;
            ret = call_gwdonu_if_api(LIB_IF_PORT_OPER_STATUS_GET, 2, logical_port, &port_status);
            //ret = gwd_onu_sw_port_admin_status_get(logical_port,&port_status);
            if (ret)
                continue;
            //gw_printf("get gwd onu port admin status fail\n");

            if (port_status == GW_PORT_ADMIN_UP)
            {
                if (gwd_port_rate_update(physical_port))
                    continue;
                //gwd_port_current_pkt_status_save(physical_port,dsts);
#ifdef __DEBUG__
                if(logical_port == 1)
                {
                    gw_printf("port rate:%d\n",gulOctRateIn[physical_port]);
                }
#else
                if (gulOctRateIn[physical_port] > broad_storm.gulBcStormThreshold)
#endif
#ifdef __DEBUG__
                if(logical_port == 1)
                {
                    gw_printf("port rate:%d\n",gulOctRateIn[physical_port]);
                }
                if(gulOctRateIn[physical_port] > 1000)
#endif
                {
#ifdef __DEBUG__
                    if(logical_port == 1)
                    {
                        gw_printf("event counter:%d\n",ulBcStormEventCnt[physical_port]);
                    }
#endif
                    ulBcStormEventCnt[physical_port]++;
                    ulBcStormStopCnt[physical_port] = 0;
                    timeCouter[physical_port] = 0;
                }
                else
                {
                    ulBcStormEventCnt[physical_port] = 0;
                    if (startCouter[physical_port] == 0)
                        timeCouter[physical_port] = 0;
                    else
                        timeCouter[physical_port]++;
                    if (havebroadcaststorm_end[physical_port])
                    {
                        ulBcStormStopCnt[physical_port]++;
                        if (ulBcStormStopCnt[physical_port] > 3)
                        {
                            call_gwdonu_if_api(LIB_IF_BROADCAST_SPEED_LIMIT, 3, logical_port, 3, 0);
                            //gw_printf("stop strom speed limit\n");
#ifdef __NOT_USE__
                            epon_onu_sw_set_port_stormctrl(logical_port, 3, 0);
#endif
                            startCouter[physical_port] = 1;
                            timeCouter[physical_port] = 0;
                            havebroadcaststorm_end[physical_port] = 0;
                            havebroadcaststorm[physical_port] = 0;
                        }
                    }
                }
                if (ulBcStormEventCnt[physical_port] > 3)
                {
                    if (broad_storm.gulBcStormStat == ENABLE)
                    {
                        ret = call_gwdonu_if_api(LIB_IF_PORT_ADMIN_SET, 2, logical_port, GW_PORT_ADMIN_DOWN);
#ifdef __NOT_USE__
                        ret = gwd_onu_sw_port_admin_status_set(logical_port,GW_PORT_ADMIN_DOWN);
                        if(ret)
                        return;
#endif
                        gwd_onu_sw_bcstorm_msg_send(slot, logical_port, 2, 1, OAMsession);
                        //gw_printf("shutdown gwd onu port %d\n",logical_port);
                        //gw_time_get(&tm);
                        gw_log(GW_LOG_LEVEL_MAJOR, "Interface  eth1/%d detected Broadcast Storm,port shutdown", logical_port);
                    }
                    else
                    {
                        printf("input port limit speed\n");
                        if (call_gwdonu_if_api(LIB_IF_BROADCAST_SPEED_LIMIT, 3, logical_port, 3, 64))
                            gw_printf("Broadcast storm speed limit failure\n");
#ifdef __NOT_USE__
                        if(epon_onu_sw_set_port_stormctrl(logical_port, 3, 64))
                        gw_printf("Broadcast storm speed limit failure\n");
#endif
                        if (!havebroadcaststorm[physical_port])
                        {
                            gwd_onu_sw_bcstorm_msg_send(slot, logical_port, 1, 1, OAMsession);
                            //gw_printf("discover gwd onu broadcast storm speed limit\n");
                            //gw_time_get(&tm);
                            gw_log(GW_LOG_LEVEL_MAJOR, "Interface  eth1/%d detected Broadcast Storm,rate limited to 64K.", logical_port);
                            //gw_log(GW_LOG_LEVEL_MAJOR, "Interface  eth0/%d detected Broadcast Storm,rate limited to 64K.",logical_port);
                            havebroadcaststorm[physical_port] = 1;
                            havebroadcaststorm_end[physical_port] = 1;
                        }
                    }
                    ulBcStormEventCnt[physical_port] = 0;
                }
                if (timeCouter[physical_port] > 2)
                {
                    gwd_onu_sw_bcstorm_msg_send(slot, logical_port, 1, 2, OAMsession);
                    if (gulOctRateIn[physical_port])
                    {
                        //gw_time_get(&tm);
                        gw_log(GW_LOG_LEVEL_MAJOR, "Interface  eth1/%d Broadcast Storm stopped, rate back to %dKbps.", logical_port, gulOctRateIn[physical_port]);
                        //gw_log(GW_LOG_LEVEL_MAJOR,"Interface  eth0/%d Broadcast Storm stopped, rate back to %dKbps.",logical_port,gulOctRateIn[physical_port]);
                    }
                    else
                    {
                        //gw_time_get(&tm);
                        gw_log(GW_LOG_LEVEL_MAJOR, "Interface  eth1/%d Broadcast Storm stopped, rate back to No Limit.", logical_port, gulOctRateIn[physical_port]);
                        //gw_log(GW_LOG_LEVEL_MAJOR,"Interface  eth0/%d Broadcast Storm stopped, rate back to No Limit.",logical_port);
                    }
                    startCouter[physical_port] = 0;
                    timeCouter[physical_port] = 0;
//									havebroadcaststorm[physical_port] = 0;
                }
            }
            else
            {
                gwd_onu_port_bcstorm_date_clear(physical_port);
            }
        }
        gw_thread_delay(10000);
    }
    gw_printf("=======================================\n");
    gw_printf("====Broadcast storm thread exit========\n");
    gw_printf("=======================================\n");
}





