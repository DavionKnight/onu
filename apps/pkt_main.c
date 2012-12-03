/*
 * pkt_main.c
 *
 *  Created on: 2012-11-1
 *      Author: tommy
 */

#include "../include/gw_os_api_core.h"

#include "pkt_main.h"
#include "gw_log.h"

typedef struct queue_para{
	gw_int32 portid;
	gw_int32 pkt_len;
	gw_int8 * pkt;
}queue_para_t;


gw_pkt_cb_func_t gw_pkt_cb_func_tab[GW_PKT_MAX];

#define GW_PKT_THREAD_STACK_SIZE (8*1024)
static gw_uint32 gw_pkt_threadid, gw_pkt_thread_prio = 13;
static gw_uint8 gw_pkt_thread_name[]="GW_PKT_THREAD";

static gw_uint32 gw_pkt_queueid, gw_pkt_queue_depth = 128, gw_pkt_msg_size = sizeof(queue_para_t);
static gw_uint8 gw_pkt_queue_name[]="GW_PKT_QUEUE";


void gw_dump_pkt(gw_int8 *pkt, gw_int16 len, gw_uint8 width)
{
	gw_int16 i;
	for(i=0; i<len; i++)
	{
		if(!(i%width))
			gw_log(GW_LOG_LEVEL_DEBUG, "\r\n");
		gw_log(GW_LOG_LEVEL_DEBUG, "%02X ", pkt[i]);
	}
}

void gw_pkt_proc_main(gw_uint32 * para)
{
	gw_uint8 * buf = malloc(gw_pkt_msg_size);
	gw_uint32 len = 0;

	if(buf)
	{
		while(1)
		{
			if(gw_pri_queue_get(gw_pkt_queueid, buf, gw_pkt_msg_size, &len, GW_OSAL_WAIT_FOREVER ) == GW_OK)
			{
				if(len < sizeof(queue_para_t))
				{
					gw_log(GW_LOG_LEVEL_MINOR,"too short msg size!\r\n");
				}
				else
				{
					queue_para_t * p = (queue_para_t *)buf;
					gw_int32 type = gw_pkt_parser_call(p->pkt, p->pkt_len);
					if(type != GW_PKT_MAX)
					{
						gw_status ret = gw_pkt_handler_call(type, p->pkt, p->pkt_len, p->portid);
						if(ret != GW_OK)
						{
							gw_log(GW_LOG_LEVEL_MINOR, "pkt handler fail, type %d\r\n", type);

							gw_dump_pkt(p->pkt, p->pkt_len, 16);
						}
					}

					free(p->pkt);
				}
			}
			else
				gw_log(GW_LOG_LEVEL_DEBUG, "get queue error\r\n");

			gw_thread_delay(50);
		}
		free(buf);
	}
}

gw_status init_pkt_proc(void)
{

	memset(gw_pkt_cb_func_tab, 0, sizeof(gw_pkt_cb_func_tab));

	if(gw_pri_queue_create(&gw_pkt_queueid, gw_pkt_queue_name, gw_pkt_queue_depth, gw_pkt_msg_size, 4) != GW_OK)
		return GW_ERROR;

	gw_log(GW_LOG_LEVEL_DEBUG, "lib_gwdonu pkt queue create ok!\r\n");

	if(gw_thread_create(&gw_pkt_threadid, gw_pkt_thread_name, gw_pkt_proc_main, NULL, GW_PKT_THREAD_STACK_SIZE, gw_pkt_thread_prio, 0) != GW_OK)
		return GW_ERROR;

	gw_log(GW_LOG_LEVEL_DEBUG, "lib_gwdonu pkt thread create ok!\r\n");

	return GW_OK;
}


gw_status gw_reg_pkt_parse(GW_PKT_TYPE type, PKT_PARSE parser)
{
	if(type < GW_PKT_MAX && type >= GW_PKT_OAM)
	{
		gw_pkt_cb_func_tab[type].parser = parser;
		return GW_OK;
	}
	else
		return GW_ERROR;
}

gw_status gw_reg_pkt_handler(GW_PKT_TYPE type, PKT_HANDLER handler)
{
	if(type < GW_PKT_MAX && type >= GW_PKT_OAM)
	{
		gw_pkt_cb_func_tab[type].handler = handler;
		return GW_OK;
	}
	else
		return GW_ERROR;
}

GW_PKT_TYPE gw_pkt_parser_call(gw_int8 *pkt, const gw_int32 len)
{
	gw_int32 ret = GW_PKT_MAX, i;

	for(i = 0; i < GW_PKT_MAX; i++)
	{
		if(gw_pkt_cb_func_tab[i].parser)
		{
			ret = (*gw_pkt_cb_func_tab[i].parser)(pkt, len);
			if(ret != GW_PKT_MAX)
				break;
		}

	}

	return ret;

}

gw_status gw_pkt_handler_call(GW_PKT_TYPE type, gw_int8 *pkt, const gw_int32 len, gw_int32 portid)
{
	gw_int32 ret = GW_ERROR;

	if(type >= 0 && type < GW_PKT_MAX)
	{
		if(gw_pkt_cb_func_tab[type].handler)
			ret = (*gw_pkt_cb_func_tab[type].handler)(pkt, len, portid);
	}

	return ret;
}

gw_int32 gwlib_sendPktToQueue(gw_int8 *pkt, const gw_int32 len, gw_int32 portid)
{
	queue_para_t * pdata = NULL;
	gw_int8 * data =  malloc(len);
	if(data)
	{
		memcpy(data, pkt, len);
	}
	else
	{
		gw_log(GW_LOG_LEVEL_DEBUG,("gwlib_sendPktToQueue malloc fail!\r\n"));
		return GW_ERROR;
	}

	pdata = malloc(sizeof(queue_para_t));
	if(pdata)
	{
		pdata->portid = portid;
		pdata->pkt_len = len;
		pdata->pkt = data;
		if(gw_pri_queue_put(gw_pkt_queueid, pdata, sizeof(queue_para_t), GW_OSAL_WAIT_FOREVER, 0) != GW_OK)
		{
			free(data);
			free(pdata);
			gw_log(GW_LOG_LEVEL_DEBUG, ("gwlib_sendPktToQueue put msg fail!\r\n"));
			return GW_ERROR;
		}
		else
			return GW_OK;
	}
	else
	{
		gw_log(GW_LOG_LEVEL_DEBUG,("gwlib_sendPktToQueue malloc fail!\r\n"));
		free(data);
		return GW_ERROR;
	}
}
