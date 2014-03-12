#include "gwdonuif_interval.h"
#include "oam.h"
#include "../cli_lib/cli_common.h"
#include "../include/gwdmgtif.h"
#include "../include/gwdonuif.h"
#include "gw_conf_file.h"

inetconfig_tlv_t ifconfigtlvsave;


gw_int32 gw_onu_ifconfig_showrun(gw_int32* len,gw_uint8**pv)
{
    gw_int32 ret = GW_ERROR;
    inetconfig_tlv_t *p = NULL;

	if(len && pv)
	{
        *len = sizeof(inetconfig_tlv_t);
        p = (inetconfig_tlv_t*)malloc(*len);
        if(p == NULL)
        {
            gw_printf("malloc error\r\n");
            return ret;
        }
        memcpy(p,&ifconfigtlvsave,sizeof(inetconfig_tlv_t));
	    *pv = (gw_uint8*)p;
		ret = GW_OK;
	}
    return ret;
}
gw_int32 gw_onu_ifconfig_restore(gw_int32 len, gw_uint8 *pv)
{
    gw_uint32 ret = GW_ERROR;
    inetconfig_tlv_t* p = 0;
    gw_int8 gwport = 0x7fffff;
    p = (inetconfig_tlv_t*)pv;
    GwdUMnGlobalParameter inetconfig;

    if(pv == NULL)
        return ret;
    if(p->Defaultflag == DEFAULT_IFCONFIG_ENABLE)
    {
        memcpy(&inetconfig,&p->inetconfig,sizeof(GwdUMnGlobalParameter));
        if(call_gwdonu_if_api(LIB_IF_MGTIF_INETCONFIG_ADD,2,inetconfig,&gwport)!= GW_OK)
        {
            gw_printf("set inet config fail\r\n");
            return ret;
        }      
    }
    return GW_OK;
}
int gw_onu_ifconfig_init()
{
    memset(&ifconfigtlvsave,0,sizeof(inetconfig_tlv_t));
    gw_register_conf_handlers(GW_CONF_TYPE_MGTIF_CONFIG, gw_onu_ifconfig_showrun, gw_onu_ifconfig_restore);
    return GW_OK;
}
int gwd_onu_tlv_ifconfig_info_get(GwdUMnGlobalParameter* mgtifconfig,gw_uint32* defaultflag)
{
    gw_uint32 ret = GW_ERROR;
    if((mgtifconfig == NULL) ||(defaultflag == NULL))
        return ret;
    memcpy(mgtifconfig,&ifconfigtlvsave.inetconfig,sizeof(GwdUMnGlobalParameter));
    *defaultflag = ifconfigtlvsave.Defaultflag;
    return GW_OK;
}

int gwd_onu_tlv_ifconfig_info_set(GwdUMnGlobalParameter mgtifconfig,gw_uint32 defaultflag)
{
    memcpy(&ifconfigtlvsave.inetconfig,&mgtifconfig,sizeof(GwdUMnGlobalParameter));
    ifconfigtlvsave.Defaultflag = defaultflag;
    return GW_OK;
}
int cmd_onu_ifconfig_show(struct cli_def *cli, char *command, char *argv[], int argc)
{
    char str[80]="";
    gw_uint32 ret=GW_ERROR;
    #if 0
    gw_uint8 phyportmember[PHY_PORT_MAX] = {0};
    gw_uint32 phyport = 0;
    unsigned long logport = 0;
    
    #endif
    gw_uint32 port = 0;
    gw_uint32 defaultflag = 0;
    GwdUMnGlobalParameter inetconfig;
    
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
             
        default:
            return gw_cli_arg_help(cli, argc > 1, NULL);
        }
    }

    ret = gwd_onu_tlv_ifconfig_info_get(&inetconfig,&defaultflag);
    if(ret != GW_OK)
    {
        return ret;
    }
    
    if(defaultflag == DEFAULT_IFCONFIG_DISABLE)
    {
        if(call_gwdonu_if_api(LIB_IF_MGTIF_INETCONFIG_GET,2,&inetconfig,&port)!= GW_OK)
        {
            gw_cli_print(cli,"get inet config fail\r\n");
            return ret;
        }
    }

	gw_cli_print(cli,"========================================================================");
	sprintf(str, "%d.%d.%d.%d", (inetconfig.mngIpAddr>>24)&0xff, (inetconfig.mngIpAddr>>16)&0xff,
					(inetconfig.mngIpAddr>>8)&0xff, (inetconfig.mngIpAddr)&0xff);
	gw_cli_print(cli,"%-10s:%-20s","ipaddress", str);
	sprintf(str, "%d.%d.%d.%d", (inetconfig.mngIpMask>>24)&0xff, (inetconfig.mngIpMask>>16)&0xff,
					(inetconfig.mngIpMask>>8)&0xff, (inetconfig.mngIpMask)&0xff);
	gw_cli_print(cli,"%-10s:%-20s","netmask", str);
    gw_cli_print(cli,"%-s:%-20s","gateway","000.000.000.000");
	gw_cli_print(cli,"%-10s:%-8d","cvlan", inetconfig.mngDataCvlan);
    gw_cli_print(cli,"%-10s:0","svlan");
    #if 0
    ret = onu_bitport_phyport_get(port,phyportmember);/*bit位转换为物理地址*/
	if(GW_ERROR == ret)/*不合法的物理端口*/
	{
		return ret;
	}
	
	for(phyport = 0; phyport <  PHY_PORT_MAX; phyport++)
	{
		if(PHY_OK == phyportmember[phyport])
		{
			if(!boards_physical_to_logical(0, phyport, &logport))/*物理地址转换为逻辑地址*/
			{
				continue;
			}
			else
			{
				if(logport > NUM_PORTS_PER_SYSTEM || logport < NUM_PORTS_MINIMUM_SYSYTEM)
				{
					continue;
				}
				else
				{
					gw_cli_print(cli,"%d ",logport);
				}
			}
		}
	}
    #endif
	gw_cli_print(cli,"========================================================================\r\n");
        
    return GW_OK;
}
int cmd_onu_ifconfig_add(struct cli_def *cli, char *command, char *argv[], int argc)
{
    gw_uint32 defaultflag = 0;
    gw_uint32 ret = GW_ERROR;
    gw_int8 gwport = 0x7ffffff;
    GwdUMnGlobalParameter inetconfig;
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
        case 1:
            return gw_cli_arg_help(cli, 0,
                "<vlan>", "1-4092",
                 NULL);
        case 2:
        	return gw_cli_arg_help(cli, 0,
        		"<xxx.xxx.xxx.xxx>","ip address",
        		NULL);
        case 3:
        	return gw_cli_arg_help(cli, 0,
        		"<xxx.xxx.xxx.xxx>","ip netmask",
        		NULL);            
        default:
            return gw_cli_arg_help(cli, argc > 2, NULL);
        }
    }
    
    
   if(3==argc)
    {
        inetconfig.mngDataCvlan = atoi(argv[0]);
        inetconfig.mngIpAddr = inet_addr(argv[1]);
        inetconfig.mngIpMask= inet_addr(argv[2]);

        if((inetconfig.mngDataCvlan < 1) || (inetconfig.mngDataCvlan > 4092))
        {
           gw_cli_print(cli,"input vlannumber error\r\n");
            return ret;            
        }

        if((0 == inetconfig.mngIpAddr) || (0 == inetconfig.mngIpAddr))
        {
            gw_cli_print(cli,"ipaddr and netmask is NULL\r\n");
            return ret;
        }
        if(call_gwdonu_if_api(LIB_IF_MGTIF_INETCONFIG_ADD,2,inetconfig,&gwport)!= GW_OK)
        {
            gw_cli_print(cli,"set inet config fail\r\n");
            return ret;
        }
        else
        {
            defaultflag = DEFAULT_IFCONFIG_ENABLE;
            gwd_onu_tlv_ifconfig_info_set(inetconfig,defaultflag);
            gw_cli_print(cli,"set inet config success\r\n");
        }
    }
    else
    {
        gw_cli_print(cli, "%% Invalid input.");
		return ret;
    }

    return CLI_OK;

}
int cmd_onu_ifconfig_del(struct cli_def *cli, char *command, char *argv[], int argc)
{
    gw_uint32 port = 0;
    gw_uint32 defaultflag = DEFAULT_IFCONFIG_DISABLE;
    gw_uint32 ret = GW_ERROR;
    GwdUMnGlobalParameter inetconfig;
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
             
        default:
            return gw_cli_arg_help(cli, argc > 1, NULL);
        }
    }
    if(call_gwdonu_if_api(LIB_IF_MGTIF_INETCONFIG_DEL,0,NULL)!= GW_OK)
    {
        gw_cli_print(cli,"set inet config fail\r\n");
        return ret;
    }
    else
    {
        memset(&inetconfig,0,sizeof(GwdUMnGlobalParameter));
        gwd_onu_tlv_ifconfig_info_set(inetconfig,defaultflag);
        gw_cli_print(cli,"set inet config success\r\n");
    }
    return CLI_OK;
}
void cli_reg_mgtif_cmd(struct cli_command **cmd_root)
{
   struct cli_command *ifconfig;
    
   ifconfig = gw_cli_register_command(cmd_root, NULL, "ifconfig",cmd_onu_ifconfig_show, PRIVILEGE_PRIVILEGED, MODE_CONFIG, "set onu mgt ip config");
              gw_cli_register_command(cmd_root, ifconfig, "add",cmd_onu_ifconfig_add, PRIVILEGE_PRIVILEGED, MODE_CONFIG, "set onu mgt ip add");
              gw_cli_register_command(cmd_root, ifconfig, "del",cmd_onu_ifconfig_del, PRIVILEGE_PRIVILEGED, MODE_CONFIG, "set onu mgt ip default");
}

