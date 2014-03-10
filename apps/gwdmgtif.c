#include "gwdonuif_interval.h"
#include "oam.h"
#include "../cli_lib/cli_common.h"
#include "../include/gwdmgtif.h"
#include "../include/gwdonuif.h"



CtcUMnGlobalParameter inetconfig;

int cmd_onu_ifconfig_add(struct cli_def *cli, char *command, char *argv[], int argc)
{
    gw_uint32 port = 0;
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
        		"<portlist>","1-24",
        		NULL);
        case 3:
        	return gw_cli_arg_help(cli, 0,
        		"<xxx.xxx.xxx.xxx>","ip address",
        		NULL);
        case 4:
        	return gw_cli_arg_help(cli, 0,
        		"<xxx.xxx.xxx.xxx>","ip netmask",
        		NULL);            
        default:
            return gw_cli_arg_help(cli, argc > 3, NULL);
        }
    }
    
    inetconfig.mngDataCvlan = atoi(argv[0]);
    port = atoi(argv[1]);
    inetconfig.mngIpAddr = inet_addr(argv[2]);
    inetconfig.mngIpMask= inet_addr(argv[3]);
    
    if(0==argc)
    {

    }else if(4==argc)
    {
        call_gwdonu_if_api(LIB_IF_MGTIF_INETCONFIG_ADD,2,inetconfig,port);
    }
    else
    {
        gw_cli_print(cli, "%% Invalid input.");
		return CLI_ERROR;
    }

    return CLI_OK;

}
int cmd_onu_ifconfig_del(struct cli_def *cli, char *command, char *argv[], int argc)
{
    return CLI_OK;
}
void cli_reg_mgtif_cmd(struct cli_command **cmd_root)
{
   struct cli_command *ifconfig;
    
   ifconfig = gw_cli_register_command(cmd_root, NULL, "ifconfig",NULL, PRIVILEGE_PRIVILEGED, MODE_CONFIG, "set onu mgt ip config");
              gw_cli_register_command(cmd_root, ifconfig, "add",cmd_onu_ifconfig_add, PRIVILEGE_PRIVILEGED, MODE_CONFIG, "set onu mgt ip add");
              gw_cli_register_command(cmd_root, ifconfig, "del",cmd_onu_ifconfig_del, PRIVILEGE_PRIVILEGED, MODE_CONFIG, "set onu mgt ip del");
}