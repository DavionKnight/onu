#include "../include/gw_config.h"
#include "../include/gw_types.h"
#include "oam.h"
#include "rcp_gwd.h"
#include "gw_usermac.h"
#include "gwdonuif_interval.h"
#include "gw_log.h"
#include "gw_aux.h"
#include "gw_port.h"
#if 0
int onu_phyport_to_logport_get(unsigned int *egports)
{
	int logport = 0;
	
	switch(*egports)
	{
		case 1:
			logport = 1;
			break;
		case 2:
			logport = 2;
			break;
		case 4:
			logport = 3;
			break;
		case 8:
			logport = 4;
			break;
			default:
				logport = 0xff;
				break;
	}
	if(logport == 0xff)
	{
		return GW_ERROR;
	}
	else
	{
		*egports = logport;
		return GW_OK;
	}
		
}
#endif
#if (RPU_MODULE_PPPOE_RELAY == RPU_YES)
int user_mac_onu_fdb_get(localMacsave_t macbuf[USR_MAC_MAX_T],unsigned char *lastmac,int *macnumberget,int *ifhavemac)
{
    gw_uint32 vid = 0, egports = 0,statics=0;
	gw_uint32 i=0,ret=0,logport=0;
	gw_uint32 phyport = 0;
	gw_uint32 macnumber = 0;
	
	
	unsigned char phyportmember[PHY_PORT_MAX]={0};
	
	macnumber = *macnumberget;
	*macnumberget = 0;
	
	if(macbuf == NULL || lastmac == NULL || macnumberget == NULL || ifhavemac == NULL)
	{
		func_pointer_error_syslog("function NULL pointer error (%s %d)\n",__func__,__LINE__);
		return GW_ERROR;
	}
	
	*ifhavemac= HAVEMAC;
	
	while(call_gwdonu_if_api(LIB_IF_FDB_ENTRY_GETNEXT, 6, vid, lastmac, &vid, lastmac, &egports,&statics) == GW_OK)
	{
		if(onu_mulimac_cheak(lastmac) == GW_OK)
		{
			continue;
		}

		ret = onu_bitport_phyport_get(egports,phyportmember);/*bitλת��Ϊ�����ַ*/
		
		if(GW_ERROR == ret)/*���Ϸ�������˿�*/
		{
			continue;
		}
		
		for(phyport = 0; phyport < PHY_PORT_MAX; phyport++)
		{
			if(PHY_OK == phyportmember[phyport])
			{
				if(!boards_physical_to_logical(0, phyport, &logport))/*�����ַת��Ϊ�߼���ַ*/
				{
					continue;
				}
				else
				{
					if(logport >= NUM_PORTS_PER_SYSTEM || logport < NUM_PORTS_MINIMUM_SYSYTEM)
					{
						continue;
					}
					else
					{
						macbuf[i].egport = logport;
						break;
					}
				}
			}
		}
		
		if(0 == logport)
		{
			continue;
		}
		
		memcpy(macbuf[i].swmac,lastmac,GW_MACADDR_LEN);	
		i++;
		
		if(i >= USR_MAC_MAX_T)
		{			
			break;
		}
		else
		{
			*ifhavemac= NOMAC;
		}
	}

	*macnumberget = i;
	
	return GW_OK;
}
int locateUserMac(char * mac, localMacsave_t *macbuf,int macnumberget,int * onuslot, int * onuport, unsigned char * subsw, char *sw_mac, int * sw_port)	
{

	int i=0;
	int ret = GW_ERROR;
	
	if(mac == NULL || macbuf == NULL || onuslot == NULL || onuport == NULL
		|| subsw == NULL || sw_mac == NULL || sw_port == NULL)
	{
		func_pointer_error_syslog("function NULL pointer error (%s %d)\n",__func__,__LINE__);
		return GW_ERROR;
	}
	
	for(i=0;i < macnumberget;i++)
	{
	   if(!memcmp(mac,macbuf[i].swmac,GW_MACADDR_LEN))
	   {				    
			if(macbuf[i].egport >= NUM_PORTS_PER_SYSTEM || macbuf[i].egport < NUM_PORTS_MINIMUM_SYSYTEM)
			{
					continue;
			}

			*onuslot = PORTNO_TO_ETH_SLOT(macbuf[i].egport);
			*onuport = PORTNO_TO_ETH_PORTID(macbuf[i].egport);
			
#if (RPU_MODULE_RCP_SWITCH)
			if( 1 == RCP_Dev_Is_Exist(*onuport))/*�������˿����Ƿ���ڽ�����*/
        	{	
				*subsw = 1;
				memcpy(sw_mac, rcpDevList[*onuport]->switchMac, GW_MACADDR_LEN);
				*sw_port = 0;
				ret = GW_OK;
				break;
        	}
			else
#endif
			{
				*subsw = 0;
				ret = GW_OK;
				break;
			}

		}
		else
		{
			continue;
		}
	}

	return ret;
}
#endif


