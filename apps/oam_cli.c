
#include "../include/gw_os_api_core.h"
#include "../include/gw_timer.h"
#include "../cli_lib/cli_common.h"
#include "gw_log.h"
#include "oam.h"
#include "gwdonuif_interval.h"

extern int cmd_show_fdb(struct cli_def *, char *, char *[], int );

int cmd_oam_port_mode(struct cli_def *cli, char *command, char *argv[], int argc)
{

	int port, mode;
	
	if(CLI_HELP_REQUESTED)
	{
		switch (argc)
		{
			case 1:
				return gw_cli_arg_help(cli, 0,
					"<1-4>", "port id selected", NULL);
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
		gw_cli_print(cli, "  nvalid input!\r\n");
	

	return CLI_OK;
}

int cmd_oam_port_isolate(struct cli_def *cli, char *command, char *argv[], int argc)
{

	int port, en;
	
	if(CLI_HELP_REQUESTED)
	{
		switch (argc)
		{
/*			
			case 1:
				return gw_cli_arg_help(cli, 0,
					"<1-4>", "port id selected", NULL);
				break;

			case 2:
*/
			case 1:
				return gw_cli_arg_help(cli, 0, 
					"[0|1]", "isolate 1 enable; 0 disable", NULL);
				break;
			default:
				return gw_cli_arg_help(cli, argc > 1, NULL  );
				break;
		}
	}

	port = 0xff;

	if(argc >= 1)
	{
		#if 0
		port = atoi(argv[0]);
		if(argc > 1)
			en = atoi(argv[1]);

		if(argc == 1 )
		{
			if(call_gwdonu_if_api(LIB_IF_PORT_ISOLATE_GET, 2, port, &en) != GW_OK)
				gw_cli_print(cli, "get port isolate fail!\r\n");
			else
				gw_cli_print(cli, "get port isolate: %s\r\n", en?"enable":"disable");
		}
		else
		{		
			if(call_gwdonu_if_api(LIB_IF_PORT_ISOLATE_SET, 2, port, en) != GW_OK)
				gw_cli_print(cli, "port %d set isolate %d fail!\r\n", port, en?"enable":"disable");
		}
		#else
		if(argc == 1)
			en = atoi(argv[1]);


		if(call_gwdonu_if_api(LIB_IF_PORT_ISOLATE_SET, 2, port, en) != GW_OK)
			gw_cli_print(cli, "port %d set isolate %s fail!\r\n", port, en?"enabled":"disabled");
	
		#endif
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
					"<port_list>", "Input one fe port number", NULL );
				break;
			case 2:
				return gw_cli_arg_help(cli, 0,
					"[1|0]", "1 enable; 0 disable", NULL);
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
				gw_cli_print(cli, "port %d atu learning %s\r\n", portid, en?"enable":"disable");
		}
	}
	else
	{
		gw_cli_print(cli, "input invalid!\r\n");
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
				break;

			default:
				return gw_cli_arg_help(cli, argc > 1, NULL  );
				break;
		}
	}

	if(argc == 1)
	{
		age = atoi(argv[0]);
		if(GW_OK != call_gwdonu_if_api(LIB_IF_ATU_AGE_SET, 1, age))
			gw_cli_print(cli, "atu age set %d fail!\r\n", age);
	}
	else
	{
		if(GW_OK != call_gwdonu_if_api(LIB_IF_ATU_AGE_GET, 1, &age))
			gw_cli_print(cli, "get atu age fail!\r\n");
		else
			gw_cli_print(cli, "atu age %d\r\n", age);
	}
	

	return CLI_OK;
}

void gw_cli_reg_oam_cmd(struct cli_command **cmd_root)
{
	struct cli_command * portcmd = NULL, *atu = NULL , *c = NULL;

	portcmd = gw_cli_register_command(cmd_root, NULL, "port", NULL,  PRIVILEGE_UNPRIVILEGED, MODE_ANY, "port config or get");
	gw_cli_register_command(cmd_root, portcmd, "mode", cmd_oam_port_mode, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "mode config");

	atu = gw_cli_register_command(cmd_root, NULL, "atu", NULL, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "atu command");
	gw_cli_register_command(cmd_root, atu, "learning", cmd_oam_atu_learn, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "learning enable");
	gw_cli_register_command(cmd_root, atu, "aging", cmd_oam_atu_age, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "age set");
	gw_cli_register_command(cmd_root, atu, "show", cmd_show_fdb, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "show information");

	c = gw_cli_register_command(cmd_root, NULL, "vlan", NULL, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "vlan command");
	gw_cli_register_command(cmd_root, c, "port_isolate", cmd_oam_port_isolate, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "isolate command");
	
    return;
}
