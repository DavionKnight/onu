/*
 * gw_access_identifier.c
 *
 *  Created on: Sep 17, 2014
 *      Author: wangzhw
 */
#include "gw_log.h"
#include "gwdonuif_interval.h"
#include "../cli_lib/cli_common.h"
#include <stdlib.h>
#include "../include/gw_os_api_core.h"
#include "../include/gw_types.h"
#include "gw_access_identifier.h"
#include "pkt_main.h"
#include <stdio.h>
#include <unistd.h>
static unsigned int DhcpProxyMode = 0;

dhcp_option82_data_info_t option82info;

#define DHCP_PROXY_MODE_CHACK(mode) if(mode > DHCP_RELAY_GWD_MODE)
#define FUNC_RETURN_VALUE_CHECK(ret) if(ret != GW_OK)
unsigned int Gwd_func_dhcp_local_option82_data_info_init()
{
	memset(&option82info,0,sizeof(dhcp_option82_data_info_t));
	return GW_OK;
}
unsigned int Gwd_func_dhcp_local_option82_data_info_get(dhcp_option82_data_info_t *local_option82_data)
{
	unsigned int ret = GW_ERROR;
    if(local_option82_data==NULL)
    {
    	gw_printf("%s %d is NULL \r\n",__func__,__LINE__);
    	return ret;
    }
    memcpy(local_option82_data,&option82info,sizeof(dhcp_option82_data_info_t));
	return GW_OK;
}
unsigned int Gwd_func_dhcp_local_option82_data_info_set(dhcp_option82_data_info_t *local_option82_data)
{
	unsigned int ret = GW_ERROR;
    if(local_option82_data==NULL)
    {
    	gw_printf("%s %d is NULL \r\n",__func__,__LINE__);
    	return ret;
    }
    memcpy(&option82info,local_option82_data,sizeof(dhcp_option82_data_info_t));
	return GW_OK;
}
unsigned int Gwd_Func_Dhcp_Proxy_Mode_set(unsigned int mode)
{
	unsigned int ret = GW_ERROR;
	DHCP_PROXY_MODE_CHACK(mode)
	{
		gw_printf("%s %d is error\r\n",__func__,__LINE__);
		return ret;
	}
	DhcpProxyMode = mode;
	return GW_OK;
}
unsigned int Gwd_Func_Dhcp_Proxy_Mode_get(unsigned int *mode)
{
	unsigned int ret = GW_ERROR;
	if(mode == NULL)
	{
		gw_printf("%s %d is NULL \r\n",__func__,__LINE__);
		return ret;
	}
	*mode = DhcpProxyMode;
	return GW_OK;
}
unsigned int Gwd_Func_Dhcp_Proxy_Mode_Process(unsigned char*option82_data,unsigned int proxy_mode,unsigned int *option82len)
{
	unsigned int ret =GW_ERROR;
	unsigned int i =0;
	unsigned int len = 0;
	unsigned char *ptr_front=NULL;
	unsigned char clv_code=0;
	unsigned char clv_len = 0;
	unsigned char  re_len = 0;
	unsigned char cumulative_len = 0;
	unsigned char dhcp_discover_pkt_flag = 0;
	unsigned char dhcpmessagetype = 0;
	dhcp_option82_data_info_t local_option82_data;
	dhcp_option82_t *clv = NULL;
	dhcpOption82_ctc_str_t *option82_str = NULL;

	gw_log(GW_LOG_LEVEL_DEBUG,"%s %d 0x%02x\r\n",__func__,__LINE__,*option82_data);
	if((option82_data == NULL) || (proxy_mode > DHCP_RELAY_CTC_MODE))
	{
		gw_printf("%s %d is NULL\r\n",__func__,__LINE__);
		return ret;
	}
	do{
		ptr_front = (unsigned char*)option82_data;
		clv = (dhcp_option82_t*)option82_data;
		gw_log(GW_LOG_LEVEL_DEBUG,"%s %d clv->option_cod:0x%02x clv->option_len:0x%02x\r\n",__func__,__LINE__,clv->option_code,clv->option_len);
		clv_code = clv->option_code;
		clv_len = clv->option_len;
		cumulative_len +=clv->option_len;
		cumulative_len +=sizeof(dhcp_option82_t);
		option82_data +=sizeof(dhcp_option82_t);
		gw_log(GW_LOG_LEVEL_DEBUG,"%s %d clv_code:0x%02x clv_len:0x%02x\r\n",__func__,__LINE__,clv_code,clv_len);
		if(clv_code == DHCP_MESSAGE_TYPE)
		{
			dhcpmessagetype = *option82_data;
			if(dhcpmessagetype != DHCPDISCOVER)
			{
				gw_log(GW_LOG_LEVEL_DEBUG,"%s %d dhcpmessagetype:0x%02x is not dhcp discover pkt drop\r\n",__func__,__LINE__,dhcpmessagetype);
				goto FUNC_END;
			}
			dhcp_discover_pkt_flag = DHCP_PKT_FLAG;
			gw_log(GW_LOG_LEVEL_DEBUG,"%s %d dhcp_discover_pkt_flag:%d clv_len:%d\r\n",__func__,__LINE__,dhcp_discover_pkt_flag,clv_len);
		}
		else
		{
			if(clv_code == DHCP_OPTION82_TYPE)
			{
				gw_log(GW_LOG_LEVEL_DEBUG,"%s %d rcv dhcp pkt is have DHCP_OPTION82_TYPE\r\n",__func__,__LINE__);
				goto FUNC_END;
			}
		}
		option82_data += clv_len;
	}while(clv_code != DHCP_OPTION_END);

	if(dhcp_discover_pkt_flag != DHCP_PKT_FLAG)
	{
		gw_printf("%s %d is not dhcp pkt\r\n",__func__,__LINE__);
		goto FUNC_END;
	}

	memset(&local_option82_data,0,sizeof(dhcp_option82_data_info_t));
	ret = Gwd_func_dhcp_local_option82_data_info_get(&local_option82_data);
	FUNC_RETURN_VALUE_CHECK(ret)
	{
		gw_printf("%s %d return error\r\n",__func__,__LINE__);
		return ret;
	}
	gw_log(GW_LOG_LEVEL_DEBUG,"%s %d ",__func__,__LINE__);
	option82_str = (dhcpOption82_ctc_str_t*)ptr_front;
	/*
	 * 0 0/0/0:0.0 OLT_MAC_ADDRESS/0/0/1/0/1/ONU_MAC_ADRESS 1/0/PORT_ON_ONU/SWITCH MAC/SWITCH_PORT:VLAN_ID EP
	 * */
	option82_str->option82.option_code=DHCP_OPTION82_TYPE;
	option82_str->cir_id.subOp = DHCP_SUB_OPTION82_1;

	gw_log(GW_LOG_LEVEL_DEBUG,"%s %d re_len:%d",__func__,__LINE__,re_len);
	memcpy(&option82_str->cir_id.str_info[0],&local_option82_data.dhcprelayidentifierinfo[0],local_option82_data.datalen);
	gw_log(GW_LOG_LEVEL_DEBUG,"%s %d dhcp_discover_pkt_flag:0x%02x datalen:%d\r\n",__func__,__LINE__,dhcp_discover_pkt_flag,local_option82_data.datalen);

	re_len += (local_option82_data.datalen-1);

	if(proxy_mode == DHCP_RELAY_GWD_MODE)
	{

	}
	else
	{
		unsigned char mac[6]={0x00,0x0f,0xe9,0x01,0x02,0x03};
		len = sprintf(&option82_str->cir_id.str_info[re_len],"%02X%02X%02X%02X%02X%02X 1/0/1/O00000000000:1 EP",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],24);
		re_len +=len;
	}
	option82_str->cir_id.len=re_len;
	gw_log(GW_LOG_LEVEL_DEBUG,"%s %d option82_str->cir_id.len:%d",__func__,__LINE__,option82_str->cir_id.len);

    option82_str->option82.option_len=(option82_str->cir_id.len+Option82HeadLen);
    ptr_front += (option82_str->cir_id.len+Option82HeadLen+SubOptionHeadLen);
    *ptr_front =EndOpion;
    ptr_front++;
    *ptr_front =Padding;
    ptr_front++;
	FUNC_END:
	*option82len = (option82_str->cir_id.len+Option82HeadLen+SubOptionHeadLen+cumulative_len);
	if(GW_LOG_LEVEL_DEBUG >= getGwlogLevel())
	{
		printf("local dhcp info:\r\n");
		printf("----------------------------------------------------------------------------------------------\r\n");
		for(i=0;i<re_len ;i++)
		{
			if(i%16 == 0)
				printf("\r\n");
			printf("0x%02x ",option82_data[i]);
		}
		printf("\r\n");

		for(i=0;i< local_option82_data.datalen;i++)
		{
			if(i%16 == 0)
				printf("\r\n");
			printf("0x%02x ",local_option82_data.dhcprelayidentifierinfo[i]);
		}
		printf("\r\n");
		printf("----------------------------------------------------------------------------------------------\r\n");
	}
	gw_log(GW_LOG_LEVEL_DEBUG,"%s %d sendlen:%d\r\n",__func__,__LINE__,*option82len);
	return GW_OK;
}

unsigned int GwdDhcpRelayAdmnSet(unsigned char *data,unsigned int len,unsigned int mode)
{
	unsigned int ret =GW_ERROR;
	unsigned char *ptr = NULL;
	unsigned int i =0;
	dhcp_option82_data_info_t gw_option82info;
	if((data == NULL) || (mode > DHCP_RELAY_GWD_MODE))
	{
		gw_printf("%s %d is NULL\r\n",__func__,__LINE__);
		return ret;
	}

	ret = Gwd_func_dhcp_local_option82_data_info_init();
	FUNC_RETURN_VALUE_CHECK(ret)
	{
		gw_printf("%s %d return error\r\n",__func__,__LINE__);
		return ret;
	}
	ret = Gwd_Func_Dhcp_Proxy_Mode_set(mode);
	FUNC_RETURN_VALUE_CHECK(ret)
	{
		gw_printf("%s %d return error\r\n",__func__,__LINE__);
		return ret;
	}
	if(mode !=DHCP_RELAY_DISABLE)
	{
		memset(&gw_option82info,0,sizeof(dhcp_option82_data_info_t));
		ptr = (unsigned char*)gw_option82info.dhcprelayidentifierinfo;
		memcpy(ptr,data,len);
		gw_option82info.datalen=len;
		if(GW_LOG_LEVEL_DEBUG >= getGwlogLevel())
		{
			printf("local dhcp admin %d:\r\n",len);
			printf("----------------------------------------------------------------------------------------------\r\n");
			for(i=0;i<len ;i++)
			{
				if(i%16 == 0)
					printf("\r\n");
				printf("0x%02x ",data[i]);
			}
			printf("\r\n");

			for(i=0;i< gw_option82info.datalen;i++)
			{
				if(i%16 == 0)
					printf("\r\n");
				printf("0x%02x ",ptr[i]);
			}
			printf("\r\n");
			printf("----------------------------------------------------------------------------------------------\r\n");
		}
		ret = Gwd_func_dhcp_local_option82_data_info_set(&gw_option82info);
		FUNC_RETURN_VALUE_CHECK(ret)
		{
			gw_printf("%s %d return error\r\n",__func__,__LINE__);
			return ret;
		}
	}
	else
	{

	}
	return GW_OK;
}
unsigned int Gwd_Func_Dhcp_relay_Oam_Admin_Process(unsigned char *pReq,unsigned int preq_len)
{
	int ret = GW_OK;
	unsigned char proxymode = 0;
	access_identifier_admin_t AdminInfoHead;

	if(NULL == pReq)
	{
		gw_printf("%s %d is NULL\r\n",__func__,__LINE__);
		return GW_ERROR;
	}
	memset(&AdminInfoHead,0,sizeof(access_identifier_admin_t));
	memcpy(&AdminInfoHead,pReq,sizeof(access_identifier_admin_t));
	pReq += sizeof(access_identifier_admin_t);
	preq_len -= sizeof(access_identifier_admin_t);
	proxymode = AdminInfoHead.proxymode;
	 gw_log(GW_LOG_LEVEL_DEBUG,"%s %d proxymode:%d\r\n",__func__,__LINE__,proxymode);
	switch(proxymode)
	{
		case DHCP_RELAY_DISABLE:
		case DHCP_RELAY_GWD_MODE:
		case DHCP_RELAY_CTC_MODE:
			 gw_log(GW_LOG_LEVEL_DEBUG,"%s %d preq_len:%d proxymode:%d\r\n",__func__,__LINE__,preq_len,proxymode);
			ret = GwdDhcpRelayAdmnSet(pReq,preq_len,proxymode);
			FUNC_RETURN_VALUE_CHECK(ret)
			{
				gw_printf("%s %d return error\r\n",__func__,__LINE__);
				return ret;
			}
			break;
		default:
			ret = GW_ERROR;
			break;
	}
	return ret;
}
int Gwd_func_dhcp_pkt_parser(unsigned char *dhcp_pkt,unsigned int dhcp_len)
{
  int ret = GW_PKT_MAX;
  unsigned int dhcpheadlen=0;
  unsigned short ethertype = 0;
  unsigned short dhcp_server_port = 0;
  unsigned short dhcp_client_port = 0;
  unsigned char ipverlen = 0;
  unsigned char ipver = 0;
  gwd_dhcp_pkt_info_head_t* dhcphead = NULL;

  dhcpheadlen = sizeof(gwd_dhcp_pkt_info_head_t);

  if((dhcp_pkt == NULL)|| (dhcp_len < dhcpheadlen))
  {
	  gw_log(GW_LOG_LEVEL_DEBUG,"%s %d  %d %d is NULL \r\n",__func__,__LINE__,dhcp_len,dhcpheadlen);
	  return ret;
  }

  dhcphead = (gwd_dhcp_pkt_info_head_t*)dhcp_pkt;
  ipverlen = dhcphead->iphead.VerHeadLen;
  gw_log(GW_LOG_LEVEL_DEBUG,"%s %d ipverlen:(0x%02x 0x%x) \r\n",__func__,__LINE__,ipverlen,dhcphead->iphead.VerHeadLen);
  ipver = ((ipverlen >> 4)&(0x0f));
  gw_log(GW_LOG_LEVEL_DEBUG,"%s %d ipverlen:(0x%02x 0x%x) \r\n",__func__,__LINE__,ipver,ipver);
  if(ipver == IpV6Version)
  {
	  gw_log(GW_LOG_LEVEL_DEBUG,"%s %d ipverlen:(0x%02x 0x%x) is ipv6 no proc\r\n",__func__,__LINE__,ipver,ipver);
	  return ret;
  }


  ethertype = ntohs(dhcphead->ethhead.ethtype);
  dhcp_server_port = ntohs(dhcphead->udphead.destUdpPort);
  dhcp_client_port = ntohs(dhcphead->udphead.srcUdpPort);

  if((ethertype != ETH_TYPE_IP) || (dhcp_server_port != DhcpSvrPortNum) || (dhcp_client_port != DhcpCliPortNum))
  {
	  gw_log(GW_LOG_LEVEL_DEBUG,"%s %d ethertype:%0x04x dhcp_server_port:%d dhcp_client_port:%d\r\n",ethertype,dhcp_server_port,dhcp_client_port);
	  return ret;
  }


  return GW_PKT_DHCP;
}
#if 0
static unsigned int crc_table[256];
static void init_crc_table(void)
{
    unsigned int c;
    unsigned int i, j;

    for (i = 0; i < 256; i++) {
        c = (unsigned int)i;
        for (j = 0; j < 8; j++) {
            if (c & 1)
                c = 0xedb88320L ^ (c >> 1);
            else
                c = c >> 1;
        }
        crc_table[i] = c;
    }
}

static unsigned int crc32(unsigned int crc,unsigned char *buffer, unsigned int size)
{
    unsigned int i;
    for (i = 0; i < size; i++) {
        crc = crc_table[(crc ^ buffer[i]) & 0xff] ^ (crc >> 8);
    }
    return crc ;
}
#endif
extern unsigned int gwd_crc32(unsigned int crc,  const unsigned char *buf, unsigned int len);
unsigned short Gwd_func_checksum_get(unsigned short *buf,unsigned int nword)
{
	unsigned int ret = GW_ERROR;
	unsigned long cksum = 0;
	if((buf == NULL) || (nword < 1))
	{
		gw_log(GW_LOG_LEVEL_DEBUG,"%s %d buflen:%d  is null error\r\n",__func__,__LINE__,nword);
		return ret;
	}

	for(cksum = 0;nword > 0;nword--)
	{
		cksum += *buf++;
	}
    cksum = (cksum >> 16) + (cksum & 0xffff);

    cksum += (cksum >>16);
    gw_log(GW_LOG_LEVEL_DEBUG,"%s %d cksum:0x%04x\r\n",__func__,__LINE__,cksum);
    return (~cksum);
}
unsigned int Gwd_func_ip_header_checksum_process(unsigned char *response_dhcp_pkt,unsigned int responselen)
{
	unsigned int ret = GW_ERROR;
	unsigned int nword = 0;
	eth_iphead_info_t *ipheader=NULL;
	gwd_dhcp_pkt_info_head_t *response_head = NULL;

	if((response_dhcp_pkt == NULL) || (responselen < DHCP_PKT_DEF_LEN))
	{
		gw_log(GW_LOG_LEVEL_DEBUG,"%s %d responselen:%d  is null error\r\n",__func__,__LINE__,responselen);
		return ret;
	}
	response_head = (gwd_dhcp_pkt_info_head_t*)response_dhcp_pkt;
	ipheader=(eth_iphead_info_t*)&response_head->iphead;
	/*把IP头的校验和清空*/
	ipheader->Checksum = 0;
	/*IP TOT_LENTH*/
	/*responselen = ETHLEN+IPLEN+UDPLEN+PAYLOAD+FCS*/
	gw_log(GW_LOG_LEVEL_DEBUG,"%s %d ipheader->Totlength:%d  \r\n",__func__,__LINE__,ipheader->Totlength);
	ipheader->Totlength = (responselen-EtherHeadLen-ETHFCSLEN);
	nword = (IPHEADERLEN/2);
	ipheader->Checksum = Gwd_func_checksum_get((unsigned short*)ipheader,nword);
	gw_log(GW_LOG_LEVEL_DEBUG,"%s %d ipheader->Totlength:0x%04x ipheader->Checksum:0x%04x nword:%d\r\n",
			__func__,__LINE__,ipheader->Totlength,ipheader->Checksum,nword);
	return GW_OK;
}
unsigned int Gwd_func_udp_header_checksum_process(unsigned char *response_dhcp_pkt,unsigned int responselen)
{
	unsigned int ret = GW_ERROR;
	gwd_dhcp_pkt_info_head_t *response_head = NULL;
	udp_head_info_t *udpheader=NULL;
	eth_iphead_info_t *ipheader=NULL;
	unsigned char *udpchecksumbuf = NULL;
	unsigned char *ptr = NULL;
	unsigned char *phead=NULL;
	unsigned char *payload = NULL;
	unsigned int payloadlen = 0;
	unsigned int checknumber =0;
	unsigned int nword =0;
	tsd_header_t tsdheader;
	if((response_dhcp_pkt == NULL) || (responselen < DHCP_PKT_DEF_LEN))
	{
		gw_log(GW_LOG_LEVEL_DEBUG,"%s %d responselen:%d  is null error\r\n",__func__,__LINE__,responselen);
		return ret;
	}
	udpchecksumbuf =(unsigned char*)malloc(responselen);
	if(udpchecksumbuf == NULL)
	{
		gw_log(GW_LOG_LEVEL_DEBUG,"%s %d  is null error\r\n",__func__,__LINE__);
		return ret;
	}
	ptr = udpchecksumbuf;
	phead = response_dhcp_pkt;
	response_head = (gwd_dhcp_pkt_info_head_t*)response_dhcp_pkt;
	ipheader = (eth_iphead_info_t*)&response_head->ethhead;
	udpheader=(udp_head_info_t*)&response_head->udphead;
	payload = (phead+EtherHeadLen+IPHEADERLEN+UDPHEADERLEN);
	/*清空UDP 校验和*/
	udpheader->checkSum = 0;
	/*UDP LENGTH*/
	udpheader->length = (responselen-EtherHeadLen-IPHEADERLEN-ETHFCSLEN);
    /*UDP 校验伪头部*/
	memset(&tsdheader,0,sizeof(tsd_header_t));
	tsdheader.destip = htonl(ipheader->DestIP);
	tsdheader.sourceip = htonl(ipheader->SourceIP);
	tsdheader.mzero = 0;
	tsdheader.ptcl = ipheader->Protocol;
	tsdheader.udplen = htons(udpheader->length);
	memcpy(ptr,&tsdheader,sizeof(tsd_header_t));
	ptr +=sizeof(tsd_header_t);
	checknumber +=sizeof(tsd_header_t);
	/*UDP头长度*/
	memcpy(ptr,&udpheader,sizeof(udp_head_info_t));
	ptr +=sizeof(udp_head_info_t);
	checknumber +=sizeof(udp_head_info_t);
	/*数据长度*/
	payloadlen = (responselen-EtherHeadLen-IPHEADERLEN-UDPHEADERLEN-ETHFCSLEN);
	gw_log(GW_LOG_LEVEL_DEBUG,"%s %d payloadlen:%d\r\n",__func__,__LINE__,payloadlen);
	memcpy(ptr,payload,payloadlen);
	ptr +=payloadlen;
	checknumber +=payloadlen;
	gw_log(GW_LOG_LEVEL_DEBUG,"%s %d [ checknumber:%d]\r\n",__func__,__LINE__,checknumber);
	if((checknumber%2) != 0)
	{
		nword = ((checknumber/2) +1);
	}
	else
	{
		nword = (checknumber/2);
	}
	gw_log(GW_LOG_LEVEL_DEBUG,"%s %d [ nword:%d]\r\n",__func__,__LINE__,nword);
	udpheader->checkSum = Gwd_func_checksum_get((unsigned short*)udpchecksumbuf,nword);
	free(udpchecksumbuf);
	gw_log(GW_LOG_LEVEL_DEBUG,"%s %d udpheader->checkSum:0x%04x nword:%d\r\n",__func__,__LINE__,udpheader->checkSum,nword);
	return GW_OK;
}
unsigned int Gwd_func_eth_pkt_checksum_process(unsigned char *response_dhcp_pkt,unsigned int responselen)
{
	unsigned int ret = GW_ERROR;
	unsigned int ethfcs = 0;
	gwd_dhcp_pkt_info_head_t *response_head = NULL;
	if((response_dhcp_pkt == NULL) || (responselen < DHCP_PKT_DEF_LEN))
	{
		gw_log(GW_LOG_LEVEL_DEBUG,"%s %d responselen:%d  is null error\r\n",__func__,__LINE__,responselen);
		return ret;
	}
	ret = Gwd_func_ip_header_checksum_process(response_dhcp_pkt,responselen);
    FUNC_RETURN_VALUE_CHECK(ret)
    {
	  gw_printf("%s %d return error\r\n",__func__,__LINE__);
	  return ret;
    }
	ret = Gwd_func_udp_header_checksum_process(response_dhcp_pkt,responselen);
    FUNC_RETURN_VALUE_CHECK(ret)
    {
	  gw_printf("%s %d return error\r\n",__func__,__LINE__);
	  return ret;
    }
	response_head = (gwd_dhcp_pkt_info_head_t*)response_dhcp_pkt;
    ethfcs =gwd_crc32(ethfcs,response_dhcp_pkt,(responselen-ETHFCSLEN));
    gw_log(GW_LOG_LEVEL_DEBUG,"%s %d pktlen:0x%08x  len:%d\r\n",__func__,__LINE__,ethfcs,responselen);
	memcpy(&response_dhcp_pkt[responselen-ETHFCSLEN],&ethfcs,ETHFCSLEN);
	return GW_OK;
}
unsigned int Gwd_func_dhcp_pkt_handler(unsigned char *dhcp_pkt,unsigned int dhcp_len,int ulport)
{
  unsigned int ret = GW_ERROR;
  unsigned int dhcpheadlen=0;
  unsigned char *option82 = NULL;
  unsigned char *dhcphead =NULL;
  unsigned int option82_len = 0;
  unsigned int dhcp_proxy_mode = 0;
  unsigned int response_len=0;
  unsigned char *response_dhcp_pkt = NULL;

  dhcpheadlen = sizeof(gwd_dhcp_pkt_info_head_t);
  gw_log(GW_LOG_LEVEL_DEBUG,"%s %d dhcp rec:dhcp_len:%d ulport:%d\r\n",__func__,__LINE__,dhcp_len,ulport);
  if((dhcp_pkt == NULL)|| (dhcp_len < dhcpheadlen))
  {
	  gw_printf("%s %d  %d %d is NULL \r\n",__func__,__LINE__,dhcp_len,dhcpheadlen);
	  return ret;
  }
  response_dhcp_pkt = (unsigned char*)malloc(DHCP_MALLOC_RESPONSE_LEN);
  if(response_dhcp_pkt == NULL)
  {
	  gw_printf("%s %d  %d %d is NULL \r\n",__func__,__LINE__,dhcp_len,dhcpheadlen);
	  return ret;
  }
  memcpy(response_dhcp_pkt,dhcp_pkt,dhcp_len);
  dhcphead = response_dhcp_pkt;

  option82 = (unsigned char*)(dhcphead+dhcpheadlen);
  ret = Gwd_Func_Dhcp_Proxy_Mode_get(&dhcp_proxy_mode);
  FUNC_RETURN_VALUE_CHECK(ret)
  {
	gw_printf("%s %d dhcp_proxy_mode:%d return error\r\n",__func__,__LINE__,dhcp_proxy_mode);
	free(response_dhcp_pkt);
	return ret;
  }
  gw_log(GW_LOG_LEVEL_DEBUG,"%s %d dhcp_proxy_mode:%d\r\n",__func__,__LINE__,dhcp_proxy_mode);
	switch(dhcp_proxy_mode)
	{
		case DHCP_RELAY_DISABLE:
			gw_log(GW_LOG_LEVEL_DEBUG,"%s %d dhcp_proxy_mode=DHCP_RELAY_DISABLE\r\n",__func__,__LINE__);
			break;
		case DHCP_RELAY_GWD_MODE:
		case DHCP_RELAY_CTC_MODE:
			ret = Gwd_Func_Dhcp_Proxy_Mode_Process(option82,dhcp_proxy_mode,&option82_len);
			FUNC_RETURN_VALUE_CHECK(ret)
			{
				gw_printf("%s %d return error\r\n",__func__,__LINE__);
				free(response_dhcp_pkt);
				return ret;
			}
			gw_log(GW_LOG_LEVEL_DEBUG,"%s %d sendlen:%d %d\r\n",__func__,__LINE__,option82_len,dhcp_len);
			response_len = (dhcpheadlen+option82_len+ETHFCSLEN);
			ret = Gwd_func_eth_pkt_checksum_process(response_dhcp_pkt,response_len);
			FUNC_RETURN_VALUE_CHECK(ret)
			{
				gw_printf("%s %d return error\r\n",__func__,__LINE__);
				free(response_dhcp_pkt);
				return ret;
			}
			ret = call_gwdonu_if_api(LIB_IF_PORTSEND, 3,GW_PON_PORT_ID, response_dhcp_pkt,response_len);
			FUNC_RETURN_VALUE_CHECK(ret)
			{
				gw_printf("%s %d return error\r\n",__func__,__LINE__);
				free(response_dhcp_pkt);
				return ret;
			}
			break;
		default:
			ret = GW_ERROR;
			break;
	}
  free(response_dhcp_pkt);
  return GW_OK;
}
int Gwd_func_dhcp_pkt_process_init()
{
	int ret = GW_ERROR;
//	init_crc_table();
	ret = gw_reg_pkt_parse(GW_PKT_DHCP, Gwd_func_dhcp_pkt_parser);
	FUNC_RETURN_VALUE_CHECK(ret)
	{
		gw_printf("%s %d return error\r\n",__func__,__LINE__);
		return ret;
	}
	ret = gw_reg_pkt_handler(GW_PKT_DHCP, Gwd_func_dhcp_pkt_handler);
	FUNC_RETURN_VALUE_CHECK(ret)
	{
		gw_printf("%s %d return error\r\n",__func__,__LINE__);
		return ret;
	}
	return GW_OK;
}
