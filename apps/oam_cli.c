
#include "../include/gw_os_api_core.h"
#include "../include/gw_timer.h"
#include "../cli_lib/cli_common.h"
#include "gw_log.h"
#include "oam.h"
#include "gwdonuif_interval.h"


#define GW_VLAN_MAX 4094
#define GW_VLAN_LAS 1

#define GW_ONUPORT_MAX 24
#define GW_ONUPORT_LAS 1

#define GW_PORT_PRI_MAX 7
#define GW_PORT_PRI_LAS 0


#define BEGIN_PARSE_PORT_LIST_TO_PORT_NO_CHECK_CLI(portlist, ifindex,devonuport_num) \
{\
    gw_uint32 * _pulIfArray;\
    gw_uint32 _i = 0;\
    _pulIfArray = (gw_uint32*)ETH_ParsePortList(portlist,devonuport_num);\
    if(!_pulIfArray)\
    	{\
    		ifindex = 0;\
    	}\
    if(_pulIfArray != NULL)\
    {\
        for(_i=0;_pulIfArray[_i]!=0;_i++)\
        {\
            ifindex = _pulIfArray[_i];\

#define END_PARSE_PORT_LIST_TO_PORT_NO_CHECK_CLI() \
        }\
        free(_pulIfArray);\
    }\
}

extern int cmd_show_fdb(struct cli_def *, char *, char *[], int );
int cmd_oam_port_kill_thread_test(struct cli_def *cli, char *command, char *argv[], int argc)
{
    int ret;
    char buf[10]={0};

    memset(&ret,0,10000);
    
    return ret;
}
int cmd_oam_port_mode(struct cli_def *cli, char *command, char *argv[], int argc)
{

	int port, mode;
	
	if(CLI_HELP_REQUESTED)
	{
		switch (argc)
		{
			case 1:
				return gw_cli_arg_help(cli, 0,
					"<1-%d>", gw_onu_read_port_num(), "port id selected", NULL);
				break;
			case 2:
				return gw_cli_arg_help(cli, 0, 
					"[0|8|9|10|11|12]", "port mode: 0 an;8 100FD; 9 100HD; 10 10FD; 11 10FD; 12 1000FD", NULL);
				break;
			default:
				return gw_cli_arg_help(cli, argc > 1, NULL  );
				break;
		}
	}

	if(argc >= 1)
	{
		int spd, duplex, en;
					
		port = atoi(argv[0]);

		if(port < 1 || port > gw_onu_read_port_num())
			return CLI_ERROR_ARG;

		if(argc > 1)
			mode = atoi(argv[1]);

		if(argc == 1 )
		{
			if(call_gwdonu_if_api(LIB_IF_PORT_MODE_GET, 4, port, &en, &spd, &duplex) != GW_OK)
				gw_cli_print(cli, "get port mode fail!\r\n");
			else
			{
				char str_spd[16] = "";
				char str_duplex[16] = "";
				
				gw_cli_print(cli, "  port %d auto neg: %s\r\n", port, en?"enabled":"disabled");

				if(spd == GWD_PORT_SPD_10)
					strcpy(str_spd, "10M");
				else if( spd == GWD_PORT_SPD_100 )
					strcpy(str_spd, "100M");
				else if( spd == GWD_PORT_SPD_1000 )
					strcpy(str_spd, "1000M");
				else
					strcpy(str_spd, "UNKNOWN");

				if(duplex == GWD_PORT_DUPLEX_FULL)
					strcpy(str_duplex, "FULL");
				else if( duplex == GWD_PORT_DUPLEX_HALF )
					strcpy(str_duplex, "HALF");
				else
					strcpy(str_duplex, "UNKNOWN");

				gw_cli_print(cli, "  current speed %s, duplex %s\r\n", str_spd, str_duplex);
			}
		}
		else
		{
			switch(mode)
			{
				case 11:
					spd = GWD_PORT_SPD_10;
					duplex = GWD_PORT_DUPLEX_HALF;
					break;
				case 10:
					spd = GWD_PORT_SPD_10;
					duplex = GWD_PORT_DUPLEX_FULL;
					break;
				case 8:
					spd = GWD_PORT_SPD_100;
					duplex = GWD_PORT_DUPLEX_FULL;
					break;
				case 9:
					spd = GWD_PORT_SPD_100;
					duplex = GWD_PORT_DUPLEX_HALF;
					break;
				case 6:
					spd = GWD_PORT_SPD_1000;
					duplex = GWD_PORT_DUPLEX_FULL;
					break;
				default:
					spd = GWD_PORT_SPD_AUNEG;
					duplex = GWD_PORT_DUPLEX_AUNEG;
					break;
			}
			if(call_gwdonu_if_api(LIB_IF_PORT_MODE_SET, 3, port, spd, duplex) != GW_OK)
				gw_cli_print(cli, "port %d set mode %d fail!\r\n", port, mode);
		}
	}
	else
		gw_cli_print(cli, "  Invalid input!\r\n");
	

	return CLI_OK;
}
#if 1
int cmd_oam_port_mirror_to(struct cli_def *cli, char *command, char *argv[], int argc)
{
    int mode = OFF;
    int UNIT = 0;
    unsigned int unit = 0;
    unsigned int phyport = 0;
    unsigned int ulPort=0;
    unsigned long maxportnumber = 0;
    unsigned int portmap = 0;

    maxportnumber = gw_onu_read_port_num();
    
    if(CLI_HELP_REQUESTED)
	{
		switch (argc)
		{

			case 1:
				    gw_cli_arg_help(cli, 0,
					"<port_list>", "Specify interface's port list(e.g.: 1; 1,2; 1-3)", NULL);

				return gw_cli_arg_help(cli, 0,
									"<0>", " Delete mirror to port", NULL);
				break;
			default:
				return gw_cli_arg_help(cli, argc > 1, NULL);
				break;
		}
	}

    if(argc == 1)
    {
        BEGIN_PARSE_PORT_LIST_TO_PORT_NO_CHECK_CLI(argv[0], ulPort,maxportnumber)
        {
			if(ulPort < 0)
			{
				gw_cli_print(cli, "%% valid input error.");
				return CLI_OK;
			}
			if(ulPort > maxportnumber)
			{
				gw_cli_print(cli,   "  Input port number %ld error!\r\n", ulPort);
				return CLI_OK;
			}
            
            boards_logical_to_physical(ulPort,&unit,&phyport);
            portmap |= 1<< phyport;
        }
        END_PARSE_PORT_LIST_TO_PORT_NO_CHECK_CLI();
        
        if(call_gwdonu_if_api(LIB_IF_PORT_MIRROR_STAT_GET,2,UNIT,&mode) != GW_OK)
        {
            gw_cli_print(cli,"get port mirror stat fail\n");
            return CLI_ERROR;
        }
        else
        {
            gw_cli_print(cli,"mirror stat :%s",mode?"ON":"OFF");
        }
            
        if(mode == OFF)
        {
            mode = ON;
            if(call_gwdonu_if_api(LIB_IF_PORT_MIRROR_STAT_SET,2,UNIT,mode)!= GW_OK)
            {
                gw_cli_print(cli,"set port mirror stat fail\n");
                return CLI_ERROR;
            }
            else
            {
                gw_cli_print(cli,"set port mirror stat %s success\n",mode?"ON":"OFF");
            }   
        }

        if(call_gwdonu_if_api(LIB_IF_MIRROR_TO_PORT_SET,2,0,portmap) != GW_OK)
        {
            gw_cli_print(cli,"mirror to port fail\n");
        }
        else
        {
            gw_cli_print(cli,"mirror to port 1/%d\r\n",ulPort);            
        }

    }
    else
    {
         gw_cli_print(cli,"  Invalid input!\r\n");
    }

    return GW_OK;
}
int cmd_oam_port_mirror_from(struct cli_def *cli, char *command, char *argv[], int argc)
{
    int mode = OFF;
    int UNIT = 0;
    unsigned long unit = 0;
    unsigned int ulPort=0;
    unsigned int phyport = 0;
    unsigned long maxportnumber = 0;
    unsigned int portmap = 0;
    int stat = 0;
    
    maxportnumber = gw_onu_read_port_num();
    
	if(CLI_HELP_REQUESTED)
	{
		switch (argc)
		{
			case 1:
				return gw_cli_arg_help(cli, 0,
					"[i|e|a]", "i: ingress; e: egress; a:All direction", NULL);
				break;
			case 2:
				return gw_cli_arg_help(cli, 0,
					"<port_list>", "Specify interface's port list(e.g.: 1; 1,2; 1-3)", NULL);
				break;
			case 3:
				return gw_cli_arg_help(cli, 0,
									"[0|1]", "1: select as source port; 0: not source port", NULL);
				break;
			default:
				return gw_cli_arg_help(cli, argc > 1, NULL);
				break;
		}
	}

    
    if(argc = 3)
    {
        if(call_gwdonu_if_api(LIB_IF_PORT_MIRROR_STAT_GET,2,UNIT,&mode) != GW_OK)
        {
            gw_cli_print(cli,"get port mirror stat fail\n");
            return CLI_ERROR;
        }
        else
        {
            gw_cli_print(cli,"mirror stat :%s",mode?"ON":"OFF");
        }
            
        if(mode == OFF)
        {
            mode = ON;
            if(call_gwdonu_if_api(LIB_IF_PORT_MIRROR_STAT_SET,2,UNIT,mode)!= GW_OK)
            {
                gw_cli_print(cli,"set port mirror stat fail\n");
                return CLI_ERROR;
            }
            else
            {
                gw_cli_print(cli,"set port mirror stat %s success\n",mode?"ON":"OFF");
            }   
        }
        
        BEGIN_PARSE_PORT_LIST_TO_PORT_NO_CHECK_CLI(argv[1], ulPort,maxportnumber)
        {
			if(ulPort < 0)
			{
				gw_cli_print(cli, "%% valid input error.");
				return CLI_OK;
			}
			if(ulPort > maxportnumber)
			{
				gw_cli_print(cli,   "  Input port number %ld error!\r\n", ulPort);
				return CLI_OK;
			}
            
            stat = atoi(argv[2]);
            
            boards_logical_to_physical(ulPort,&unit,&phyport);
            
            if(strcmp(argv[0],"i") == 0)
            {           
                if(call_gwdonu_if_api(LIB_IF_PORT_INGRESS_MIRROR_SET,3,0,phyport,stat) != GW_OK)
                {
                    gw_cli_print(cli,"set ingress mirror port fail\n");
                }
                else
                {
                    gw_cli_print(cli,"mirror ingress port 1/%d\r\n",ulPort);
                }
                
            }
            else if(strcmp(argv[0],"e") == 0)
            {
                if(call_gwdonu_if_api(LIB_IF_PORT_EGRESS_MIRROR_SET,3,0,phyport,stat) != GW_OK)
                {
                    gw_cli_print(cli,"set egress mirror port fail\n");
                }
                else
                {
                    gw_cli_print(cli,"mirror egress port 1/%d\r\n",ulPort);
                }
            }
            else if(strcmp(argv[0],"a") == 0)
            {
                if(call_gwdonu_if_api(LIB_IF_PORT_INGRESS_MIRROR_SET,3,0,phyport,stat) != GW_OK)
                {
                    gw_cli_print(cli,"set ingress mirror port fail\n");
                }

                if(call_gwdonu_if_api(LIB_IF_PORT_EGRESS_MIRROR_SET,3,0,phyport,stat) != GW_OK)
                {
                    gw_cli_print(cli,"set egress mirror port fail\n");
                }

            }

        }
        END_PARSE_PORT_LIST_TO_PORT_NO_CHECK_CLI();
        

    }
    else
    {
        gw_cli_print(cli,"  Invalid input!\r\n");
    }

	return CLI_OK;
}
#endif

int cmd_oam_event_show(struct cli_def *cli, char *command, char *argv[], int argc)
{

	int slot;
	char * pbuf = NULL;

	if(CLI_HELP_REQUESTED)
	{
		switch (argc)
		{
			case 1:
				return gw_cli_arg_help(cli, 0,
					"<1-1024>", "msg slot number", NULL);
				break;
			default:
				return gw_cli_arg_help(cli, argc > 1, NULL  );
				break;
		}
	}

	slot = 1025;

	if(argc >= 1)
	{
		if(argc == 1)
			slot = atoi(argv[0]);

		pbuf = gw_log_get_record(slot-1);

		if(pbuf)
			gw_cli_print(cli, "%s", pbuf);
		else
			gw_cli_print(cli, "empty slot!\r\n");

	}
	else
	{
		int nextslot = 0, start = 0;
		int flag = 0;
		start = slot =  gw_log_get_current_msg_slot();
#if 0
		do{

			pbuf = gw_log_getnext_record(slot, &nextslot);
		if(!pbuf)
			gw_cli_print(cli, "empty slot!\r\n");
		else
			gw_cli_print(cli, "%d %s\r\n", nextslot, pbuf);

			slot = nextslot;
		}
		while(nextslot != start);
#else
		for(start = 0; start < GW_MAX_EVENT_LOG_NUM; start++)
		{
			pbuf = gw_log_get_record(start);

			if(pbuf)
			{
				flag = 1;
				gw_cli_print(cli, "%s", pbuf);
                gw_thread_delay(100);                                                
			}
		}
		if(!flag)
			gw_cli_print(cli, "empty slot!\r\n");
#endif
	}


	return CLI_OK;
}

int cmd_oam_port_isolate(struct cli_def *cli, char *command, char *argv[], int argc)
{

	int port, en;
	
	if(CLI_HELP_REQUESTED)
	{
		switch (argc)
		{
			case 1:
				return gw_cli_arg_help(cli, 0, 
					"{[0|1]}*1", "isolate 1 enable; 0 disable", NULL);
			default:
				return gw_cli_arg_help(cli, argc > 1, NULL  );
		}
	}

	port = 0xff;

	if(argc >= 1)
	{
		if(argc == 1)
			en = atoi(argv[0]);


		if(call_gwdonu_if_api(LIB_IF_PORT_ISOLATE_SET, 2, port, en) != GW_OK)
			gw_cli_print(cli, "port %d set isolate %s fail!\r\n", port, en?"enabled":"disabled");
		else
			{
				if(en)
					gw_cli_print(cli,"set all port isolate enable success\n");
				else
					gw_cli_print(cli,"set all port isolate disable success\n");
			}
	}
	else
	{	
		if(call_gwdonu_if_api(LIB_IF_PORT_ISOLATE_GET, 2, port, &en) != GW_OK)
			gw_cli_print(cli, "get port isolate fail!\r\n");
		else
			gw_cli_print(cli, "Port isolate is %s\r\n", en?"enabled":"disabled");

	}
	

	return CLI_OK;
}

int cmd_oam_atu_learn(struct cli_def *cli, char *command, char *argv[], int argc)
{

	int en = 0, portid = 0;

	if(CLI_HELP_REQUESTED)
	{
		switch (argc)
		{
			case 1:
				return gw_cli_arg_help(cli, 0, 
					"<portlist>", "Input one fe port number", NULL );
				break;
			case 2:
				return gw_cli_arg_help(cli, 0,
					"{[1|0]}*1", "1 enable; 0 disable", NULL);
				break;

			default:
				return gw_cli_arg_help(cli, argc > 1, NULL  );
				break;
		}
	}
 
	if(argc >= 1)
	{
		portid = atoi(argv[0]);

		if(argc == 2)
			en = atoi(argv[1]);

		if(argc == 2 )
		{
			if(GW_OK != call_gwdonu_if_api(LIB_IF_ATU_LEARN_SET, 2, portid,  en))
				gw_cli_print(cli, "atu learning set %s fail!\r\n", en?"enable":"disable");
		}
		else
		{
			if(GW_OK != call_gwdonu_if_api(LIB_IF_ATU_LEARN_GET, 2,portid, &en))
				gw_cli_print(cli, "get port %d atu learning fail!\r\n", portid);
			else
				gw_cli_print(cli,"Port %d source mac address learn is %s\r\n",portid,en?"enable":"disable");
		}
	}
	else
	{
		gw_cli_print(cli, GW_CLI_INCOMPLETE_MSG);
		return CLI_ERROR;
	}

	

	return CLI_OK;
}

int cmd_oam_atu_age(struct cli_def *cli, char *command, char *argv[], int argc)
{

	int age = 0;

	if(CLI_HELP_REQUESTED)
	{
		switch (argc)
		{
			case 1:
				return gw_cli_arg_help(cli, 0,
					"<0-600>", "l2 age time unit sec, 0: disable aging", NULL);

			default:
				return gw_cli_arg_help(cli, argc > 1, NULL  );
		}
	}

	if(argc == 1)
	{
		age = atoi(argv[0]);
		if(age < 0 ||age > 600)
			{
				gw_cli_print(cli,"set aging time error \n");
				return CLI_ERROR;
			}
		if(GW_OK != call_gwdonu_if_api(LIB_IF_ATU_AGE_SET, 1, age))
			gw_cli_print(cli, "atu age set %d fail!\r\n", age);
		else
			gw_cli_print(cli,"atu aging time set %d sucess",age);
	}
	else
	{
		if(GW_OK != call_gwdonu_if_api(LIB_IF_ATU_AGE_GET, 1, &age))
			gw_cli_print(cli, "get atu aging time fail!\r\n");
		else
			gw_cli_print(cli, "Mac table aging time is %d seconds (PAS & BCM).\r\n", age);
	}
	

	return CLI_OK;
}

int cmd_gw_laser(struct cli_def *cli, char *command, char *argv[], int argc)
{
	unsigned int laser_mode = 0;

	// deal with help
	if (CLI_HELP_REQUESTED) {
		switch (argc) {
		case 1:
			return gw_cli_arg_help(cli, 0, "[1|0]", "1: always on 0: normal",
					NULL );
		default:
			return gw_cli_arg_help(cli, argc > 1, NULL );
		}
	}

	if (1 == argc) {
		if (atoi(argv[0]) == 1) {
			laser_mode = GwEponTxLaserAlwaysOn;
		} else if (atoi(argv[0]) == 0) {
			laser_mode = GwEponTxLaserNormal;
		} else {
			gw_cli_print(cli, "%% Invalid input.");
			return CLI_ERROR;
		}

		if(call_gwdonu_if_api(LIB_IF_LASER_SET, 1, laser_mode)!=GW_OK)
			gw_cli_print(cli, "set lase mode fail!\r\n");

	} else {
		if(call_gwdonu_if_api(LIB_IF_LASER_GET, 1, &laser_mode) != GW_OK)
				gw_cli_print(cli, "get laser status fail!\r\n");
		else
		{
		    char ststr[32]="";
		    switch(laser_mode)
		    {
		    case GwEponTxLaserNormal:
		        strcpy(ststr, "normal");
		        break;
		    case GwEponTxLaserAlwaysOn:
		        strcpy(ststr, "alwways on");
		        break;
		    case GwEponTxLaserDisable:
		        strcpy(ststr, "disable");
		        break;
		    default:
		        strcpy(ststr, "unknow");
		        break;
		    }
			gw_cli_print(cli, "laser status is %s", laser_mode?"always on":"normal");
		}
	}

	return CLI_OK;
}

int cmd_gw_conf(struct cli_def *cli, char *command, char *argv[], int argc)
{
	int ret = CLI_OK, code = 1;

	if (CLI_HELP_REQUESTED) {
		switch (argc) {
		case 1:
			return gw_cli_arg_help(cli, 0, "[save|clear]", "save or clear config file",
					NULL );
		default:
			return gw_cli_arg_help(cli, argc > 1, NULL );
		}
	}

	if(argc == 1)
	{
		if(strcmp(argv[0], "clear") == 0 )
			code = 0;
	}
	else
		ret = CLI_ERROR_ARG;

	
	if(gw_conf_save(code) != GW_OK)
		ret = CLI_ERROR;

	return ret;
}

int cmd_static_mac_add_fdb(struct cli_def *cli, char *command, char *argv[], int argc)
{
	gw_uint32 gw_port;
	gw_uint16 gw_vlan;
	gw_uint32 gw_pri;
	if (CLI_HELP_REQUESTED) {
		switch (argc) {
		case 1:
			return gw_cli_arg_help(cli, 0, "xxxx.xxxx.xxxx", "Please input the mac address",
					NULL );
		case 2:
		    return gw_cli_arg_help(cli, 0, "<port_list>", "Please input the port_list",
					NULL );
		case 3:
			return gw_cli_arg_help(cli, 0, "<1-4094>", "Please input vlan id",
					NULL );
		case 4:
			return gw_cli_arg_help(cli, 0, "<0-7>", "MAC address's priority",
					NULL );			
		default:
			return gw_cli_arg_help(cli, argc > 3, NULL );

		}
	}

	if(argc == 4)
		{
			gw_port = strtol(argv[1], NULL, 10);
			if(gw_port > GW_ONUPORT_MAX || gw_port < GW_ONUPORT_LAS)
				{
					gw_cli_print(cli,"port error\n");
					return -1;
				}
			gw_vlan = strtol(argv[2], NULL, 10);
			if(gw_vlan >GW_VLAN_MAX ||gw_vlan < GW_VLAN_LAS)
				{
					gw_cli_print(cli,"vlan error\n");
					return -1;
				}	
			gw_pri = strtol(argv[3], NULL, 10);
			if(gw_pri < GW_PORT_PRI_LAS || gw_pri > GW_PORT_PRI_MAX)
				{
					gw_cli_print(cli,"pri error\n");
				}
			if(call_gwdonu_if_api(LIB_IF_STATIC_MAC_ADD, 3,argv[0],gw_port,gw_vlan) != GW_OK)
				gw_cli_print(cli,"add static mac fail\n");
			else
				gw_cli_print(cli,"add static mac success\n");
		}
	else
		{
			gw_cli_print(cli,"%%input error\n");
		}
	return CLI_OK;
}
int cmd_static_mac_del_fdb(struct cli_def *cli, char *command, char *argv[], int argc)
{
		gw_uint16 gw_vlan;
		if (CLI_HELP_REQUESTED) {
		switch (argc) {
		case 1:
			return gw_cli_arg_help(cli, 0, "xxxx.xxxx.xxxx", "Please input the mac address",
					NULL );
		case 2:
			return gw_cli_arg_help(cli, 0, "<1-4094>", "Please input vlan id",
					NULL );
		default:
			return gw_cli_arg_help(cli, argc > 1, NULL );
			}

		}
		if(argc == 2)
			{
				gw_vlan = strtol(argv[1], NULL, 10);
				if(gw_vlan >GW_VLAN_MAX ||gw_vlan < GW_VLAN_LAS)
					{
						gw_cli_print(cli,"vlan error\n");
						return -1;
					}	
				if(call_gwdonu_if_api(LIB_IF_STATIC_MAC_DEL, 2,argv[0],gw_vlan) != GW_OK)
					gw_cli_print(cli,"del static mac fail\n");
				else
					gw_cli_print(cli,"del static mac success\n");
			}
		else
			{
				gw_cli_print(cli,"%%input error\n");
			}		
	return CLI_OK;
}

#ifdef CYG_LINUX
extern int gwd_onu_reboot(int a);
#endif

int cmd_onu_reboot(struct cli_def *cli, char *command, char *argv[], int argc)
{
	int a = 10;
#ifndef CYG_LINUX
	int enable;
	if (CLI_HELP_REQUESTED) {
	switch (argc) {
	case 1:
		return gw_cli_arg_help(cli, 0, "[0|1]", "reboot enable",
				NULL );
	default:
		return gw_cli_arg_help(cli, argc > 1, NULL );
		}

	}

    if(argc == 1)
    {
    	enable = atoi(argv[0]);
		if(enable)
			call_gwdonu_if_api(LIB_IF_ONU_REBOOT, 1,a);
		else
			gw_printf("reboot error\n");

    }
	else
	{
		gw_cli_print(cli, "%% Invalid input.");
        return CLI_OK;
	}
	#else
	gwd_onu_reboot(a);
	#endif
	return CLI_OK;
}
void gw_cli_reg_oam_cmd(struct cli_command **cmd_root)
{
	struct cli_command * portcmd = NULL, *atu = NULL , *c = NULL,*reboot= NULL;

	portcmd = gw_cli_register_command(cmd_root, NULL, "port", NULL,  PRIVILEGE_UNPRIVILEGED, MODE_ANY, "port config or get");
	gw_cli_register_command(cmd_root, portcmd, "mode", cmd_oam_port_mode, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "mode config");
    gw_cli_register_command(cmd_root, portcmd, "mirror_from", cmd_oam_port_mirror_from, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "port mirror from set");
    gw_cli_register_command(cmd_root, portcmd, "mirror_to", cmd_oam_port_mirror_to, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "port mirror to set");
    
	//gw_cli_register_command(cmd_root, portcmd, "kill_thread", cmd_oam_port_kill_thread_test, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "mode config");
  
	atu = gw_cli_register_command(cmd_root, NULL, "atu", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "atu command");
	gw_cli_register_command(cmd_root, atu, "learning", cmd_oam_atu_learn, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "learning enable");
	gw_cli_register_command(cmd_root, atu, "aging", cmd_oam_atu_age, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "age set");
	gw_cli_register_command(cmd_root, atu, "show", cmd_show_fdb, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "show information");
	gw_cli_register_command(cmd_root, atu, "static_add", cmd_static_mac_add_fdb, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "static fdb mac add");
	gw_cli_register_command(cmd_root, atu, "static_del", cmd_static_mac_del_fdb, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "static fdb mac del");

	c = gw_cli_register_command(cmd_root, NULL, "vlan", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "vlan command");
	gw_cli_register_command(cmd_root, c, "port_isolate", cmd_oam_port_isolate, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "isolate command");
	
	c = gw_cli_register_command(cmd_root, NULL, "mgt", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "ONU device management");
	gw_cli_register_command(cmd_root, c, "laser", cmd_gw_laser, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Laser status");
	gw_cli_register_command(cmd_root, c, "config", cmd_gw_conf, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "config save");

	c = gw_cli_register_command(cmd_root, NULL, "event", NULL, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "event show");
	gw_cli_register_command(cmd_root, c, "show", cmd_oam_event_show, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "show");
	reboot = gw_cli_register_command(cmd_root, NULL, "ONU",NULL, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "reboot onu");
	gw_cli_register_command(cmd_root,reboot, "reboot", cmd_onu_reboot, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "reboot onu");


    return;
}
