/*
 * oamsnmp.c
 *
 *  Created on: 2013-3-25
 *      Author: tommy
 */

#include <sys/socket.h>

#include <netinet/in.h>

#include "../include/gw_timer.h"
#include "gw_log.h"
#include "gwdonuif_interval.h"

#include "oamsnmp.h"
#define SNMP_P 0
extern int CommOnuMsgSend(unsigned char GwOpcode, unsigned int SendSerNo, unsigned char *pSentData,const unsigned short SendDataSize, unsigned char  *pSessionIdfield);

static gw_uint16 s_oamsnmp_ser_port = 8000;
static gw_int32 s_oamsnmp_ser_sock = 0;
static struct sockaddr_in s_oamsnmp_ser_sin;

static gw_uint32 s_oamsnmp_rx_thread_id = 0, s_oamsnmp_rx_thread_stack_size = 3*2048,
		s_oamsnmp_rx_thread_pri = 14;

static gw_uint8 s_oamsnmp_rx_thread_name[] = "oamsnmprx";

static gw_uint32 s_oamsnmp_send_no = 0;
static gw_uint8 s_oamsnmp_session_id[8] = "";

static gw_status init_oamsnmp_ser_socket()
{
	s_oamsnmp_ser_sock = socket(AF_INET, SOCK_DGRAM, PF_UNSPEC);
	if(s_oamsnmp_ser_sock == -1)
	{
		gw_log(GW_LOG_LEVEL_DEBUG, ("oamsnmp service sock fail!\r\n"));
		return (GW_E_RESOURCE);
	}
	memset(&s_oamsnmp_ser_sin,0,sizeof(struct sockaddr_in));
	s_oamsnmp_ser_sin.sin_family = AF_INET;
	s_oamsnmp_ser_sin.sin_port = htons(s_oamsnmp_ser_port);
	s_oamsnmp_ser_sin.sin_addr.s_addr = INADDR_ANY;

	if(bind(s_oamsnmp_ser_sock, (struct sockaddr*)&s_oamsnmp_ser_sin, sizeof(struct sockaddr)) == -1)
	{
		gw_log(GW_LOG_LEVEL_DEBUG, ("init_oamsnmp_ser_socket bind sock fail!\r\n"));
		close(s_oamsnmp_ser_sock);
		return (GW_E_ERROR);
	}

	return GW_E_OK;
}

int gw_snmp_rx_print(int rxnum,unsigned char *buf)
{
	int j = 0;
	if(SNMP_P)
	{
		gw_printf("%s %d\n",__func__,__LINE__);

		for(j=0;j<rxnum;j++)
			{
				if(j%20 ==0)
					gw_printf("\n");
				gw_printf("0x%02x ",*buf);
				buf++;
			}
		gw_printf("\n");
	}
	return 0;

}
static void gw_oam_snmp_rx_thread_entry(gw_uint32 * para)
{
	gw_uint8 buf[2*1024] = "";
	gw_int32 len = 2*1024;
	gw_int32 i =0;
	gw_int32 j =0;
	gw_uint8 *pri_buf;
	while(1)
	{
		socklen_t socklen = sizeof(struct sockaddr);
		memset(buf,0,len);
		gw_int32 rxnum = recvfrom(s_oamsnmp_ser_sock, buf, len, 0, (struct sockaddr*)&s_oamsnmp_ser_sin, &socklen);

		if(rxnum > 0)
		{	
			pri_buf = buf;
			gw_snmp_rx_print(rxnum,pri_buf);
			CommOnuMsgSend(SNMP_TRAN_RESP, get_oamsnmp_send_no(), buf, rxnum, get_oamsnmp_session_id());
		}

		gw_thread_delay(10);
	}
}

static gw_status init_oamsnmp_ser_rx_thread()
{

	if(gw_thread_create(&s_oamsnmp_rx_thread_id, s_oamsnmp_rx_thread_name,
			gw_oam_snmp_rx_thread_entry, NULL, s_oamsnmp_rx_thread_stack_size,
			s_oamsnmp_rx_thread_pri, 0) != GW_OK)
	{
		gw_log(GW_LOG_LEVEL_DEBUG, "create %s fail !\r\n", s_oamsnmp_rx_thread_name);
		return GW_E_ERROR;
	}

	return GW_E_OK;
}

gw_int32 get_oamsnmp_socket()
{
	return s_oamsnmp_ser_sock;
}

gw_uint32 get_oamsnmp_send_no()
{
	return s_oamsnmp_send_no;
}

gw_uint32 set_oamsnmp_send_no(gw_uint32 no)
{
	s_oamsnmp_send_no = no;
	return s_oamsnmp_send_no;
}

void set_oamsnmp_session_id(gw_int8 * sid, gw_int32 len)
{
	if(len > sizeof(s_oamsnmp_session_id))
		len = sizeof(s_oamsnmp_session_id);
	memcpy(s_oamsnmp_session_id, sid, len);
}

gw_int8* get_oamsnmp_session_id()
{
	return s_oamsnmp_session_id;
}

gw_status gwd_oamsnmp_handle(GWTT_OAM_MESSAGE_NODE * msg)
{

	struct sockaddr_in peer;
	int ret;
	memset(&peer, 0, sizeof(struct sockaddr_in));
	peer.sin_family = AF_INET;
	peer.sin_port = htons(161);
	peer.sin_addr.s_addr = INADDR_ANY;

	set_oamsnmp_send_no(msg->SendSerNo);
	set_oamsnmp_session_id(msg->SessionID, sizeof(msg->SessionID));
	if(SNMP_P)
	{
		gw_printf("%s %d\n",__func__,__LINE__);
		gw_printf("msg->WholePktLen:%d\n",msg->WholePktLen);
	}
	ret = sendto(s_oamsnmp_ser_sock, msg->pPayLoad, msg->WholePktLen, 0, (struct sockaddr*)&peer, sizeof(struct sockaddr));
	if(ret == -1)
		{
			if(SNMP_P)
			{
			    gw_printf("errno=%d\n",errno);
				char * mesg = strerror(errno);
	  			gw_printf("Mesg:%s\n",mesg);
			}

		}
	return GW_OK;

}

void init_oamsnmp()
{
	if(init_oamsnmp_ser_socket() == GW_E_OK)
	{
		if(init_oamsnmp_ser_rx_thread() != GW_OK)
			close(s_oamsnmp_ser_sock);
	}
}
