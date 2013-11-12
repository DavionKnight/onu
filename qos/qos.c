/*
 * qos.c
 *
 *  Created on: 2013年9月5日
 *      Author: tommy
 */

#include "qos.h"

typedef struct {
	gw_node node;
	gw_uint32 port;
	gw_uint32 vid;
	gw_uint32 queue;
}qos_vlan_t;

static gw_list s_qos_vlan_queue_list;

static gw_list *plstqvq = &s_qos_vlan_queue_list;

gw_int32 gw_qos_vlan_queue_add(gw_int32 port, gw_int32 vlanid, gw_int32 queue)
{
	gw_int32 ret = GW_ERROR;

	gw_uint32 key = (port<<16) | vlanid;

	if(gw_lst_find(plstqvq, key) == NULL)
	{
		qos_vlan_t * p = malloc(sizeof(qos_vlan_t));

		if(p)
		{
			p->port = port;
			p->vid = vlanid;
			p->queue = queue;
			gw_lst_add(plstqvq, (gw_node*)p);

			ret = GW_OK;
		}
	}

	return ret;
}

gw_int32 gw_qos_vlan_queue_remove(gw_int32 port, gw_int32 vlanid)
{
	gw_int32 ret = GW_ERROR;
	gw_uint32 key = (port<<16)|vlanid;
	qos_vlan_t *p = NULL;

	if((p = gw_lst_find(plstqvq, key)))
	{
		gw_lst_delete((gw_list*)plstqvq, (gw_node*)p);
		free(p);
		ret = 0;
	}

	return ret;
}

gw_int32 gw_qos_vlan_queue_entry_get_by_port(gw_uint8 port, gw_qos_vlan_queue_data_t ** pv)
{
	gw_int32 ret = 0;
	gw_list l, *plist = NULL;
	qos_vlan_t * p;
	gw_int32 count = 0, i;

	gw_lst_init(&l, NULL);

	if(port == 0xff)
		plist = plstqvq;
	else
	{
		plist = &l;

		gw_lst_scan(plstqvq, p, qos_vlan_t * )
		{
			if(p->port == port)
			{
				qos_vlan_t * lv = malloc(sizeof(qos_vlan_t));

				if(lv)
				{
					memcpy(lv, p, sizeof(qos_vlan_t));
					gw_lst_add(&l, (gw_node*)lv);
				}
			}
		}
	}

	count = gw_lst_count(plist);

	if(count > 0)
	{
		gw_qos_vlan_queue_data_t * pd = malloc(count*sizeof(gw_qos_vlan_queue_data_t));

		if(pd)
		{
			i = 0;
			gw_lst_scan(plist, p, qos_vlan_t*)
			{
				pd[i].port = p->port;
				pd[i].vid = p->vid;
				pd[i].queue = p->queue;
				i++;
			}

			*pv = pd;
			ret = count;
		}
	}

	while((p = gw_lst_get(&l)))
		free(p);

	return ret;
}

gw_int32 gw_qos_vlan_queue_rules_apply( gw_int32 reset )
{
	gw_int32 count = 0, ret = GW_OK;
	gw_qos_vlan_queue_data_t *pd = NULL;

	if(reset)
		count = 0;
	else
		count = gw_qos_vlan_queue_entry_get_by_port(0xff, &pd);

	if(count >= 0)
	{

		if(call_gwdonu_if_api(LIB_IF_QOS_VLAN_QUEUE_MAP, 2, count, pd) != GW_OK)
			ret = GW_ERROR;

		if(pd)
			free(pd);
	}
	else
		ret = GW_ERROR;

	return ret;
}

static gw_int32 qos_vlan_queue_compare(void * data, gw_uint32 key)
{
	gw_uint32 port = key >> 16;
	gw_uint32 vlan = key & 0xffff;

	qos_vlan_t * pv = (qos_vlan_t*)data;

	if(pv->port == port && pv->vid == vlan)
		return 0;
	else
		return (-1);
}

gw_int32 gw_qos_vlan_showrun(gw_int32 * len, gw_uint8 ** pv)
{
	gw_int32 ret = GW_ERROR;
	gw_uint32 count = gw_lst_count(plstqvq);

	if(len && pv)
	{
		gw_uint32 * p = NULL;
		*len = count*3*sizeof(gw_uint32);
		p = malloc(*len);

		if(p)
		{
			qos_vlan_t *pl = NULL;
			gw_uint32 * pp = p;
			gw_lst_scan(plstqvq, pl, qos_vlan_t *)
			{
				*pp++ = pl->port;
				*pp++ = pl->vid;
				*pp++ = pl->queue;
			}

			*pv = (gw_uint8*)p;

			ret = GW_OK;
		}
	}

	return ret;

}

gw_int32 gw_qos_vlan_restore(gw_int32 len, gw_uint8 * pv)
{

	gw_uint32 * p = (gw_uint32*)pv;

	gw_int32 num = len/(sizeof(gw_uint32)*3), i;

	for(i=0; i<num; i++)
	{
		gw_uint8 port = *p++;
		gw_uint32 vid = *p++;
		gw_uint32 queue = *p++;

		gw_qos_vlan_queue_add(port, vid, queue);

	}

	gw_qos_vlan_queue_rules_apply(0);

	return 0;
}

void gw_qos_init()
{
	gw_lst_init(&s_qos_vlan_queue_list, qos_vlan_queue_compare);

	gw_register_conf_handlers(GW_CONF_TYPE_QOSVLANQUEUE, gw_qos_vlan_showrun, gw_qos_vlan_restore);
}
