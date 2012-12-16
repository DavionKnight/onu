
#include "../include/gw_os_api_core.h"
#include "../include/gw_timer.h"
#include "../cli_lib/cli_common.h"
#include "gw_log.h"
#include "oam.h"
#include "gwdonuif_interval.h"


int cmd_oam_port_mode(struct cli_def *cli, char *command, char *argv[], int argc)
{

	int port, mode;
	
	if(CLI_HELP_REQUESTED)
	{
		switch (argc)
		{
			case 1:
				return gw_cli_arg_help(cli, 0,
					"<1-4>", "port id selected");
				break;
			case 2:
				return gw_cli_arg_help(cli, 0, 
					"[0|1|2|3|4|5|6]", "port mode: 0 an;1 10HD; 2 10FD; 3 100HD; 4 100FD; 5 1000HD; 6 1000FD");
				break;
			default:
				return gw_cli_arg_help(cli, argc > 1, NULL  );
				break;
		}
	}

	if(argc >= 1)
	{
		int spd, duplex;
					
		port = atoi(argv[0]);
		if(argc > 1)
			mode = atoi(argv[1]);

		if(argc == 1 )
		{
			if(call_gwdonu_if_api(LIB_IF_PORT_MODE_GET, 3, port, &spd, &duplex) != GW_OK)
				gw_cli_print(cli, "get port mode fail!\r\n");
			else
				gw_cli_print(cli, "get port mode: speed %d, duplex %s\r\n", spd, duplex?"full":"half");
		}
		else
		{
			switch(mode)
			{
				case 1:
					spd = 10;
					duplex = 0;
					break;
				case 2:
					spd = 10;
					duplex = 1;
					break;
				case 3:
					spd = 100;
					duplex = 0;
					break;
				case 4:
					spd = 100;
					duplex = 1;
					break;
				case 5:
					spd = 1000;
					duplex = 0;
					break;
				case 6:
					spd = 1000;
					duplex = 1;
					break;
				default:
					spd = 0;
					duplex = 1;
					break;
			}
			
			if(call_gwdonu_if_api(LIB_IF_PORT_MODE_SET, 3, port, spd, duplex) != GW_OK)
				gw_cli_print(cli, "port %d set mode %d fail!\r\n", port, mode);
		}
	}
	else
		gw_cli_print(cli, "invalid input!\r\n");
	

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
					"<1-4>", "port id selected");
				break;
			case 2:
				return gw_cli_arg_help(cli, 0, 
					"[0|1]", "isolate 1 enable; 0 disable");
				break;
			default:
				return gw_cli_arg_help(cli, argc > 1, NULL  );
				break;
		}
	}

	if(argc >= 1)
	{
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
	}
	else
		gw_cli_print(cli, "invalid input!\r\n");
	

	return CLI_OK;
}

int cmd_oam_atu_learn(struct cli_def *cli, char *command, char *argv[], int argc)
{

	int en = 0;

	if(CLI_HELP_REQUESTED)
	{
		switch (argc)
		{
			case 1:
				return gw_cli_arg_help(cli, 0,
					"[1|0]", "1 enable; 0 disable");
				break;

			default:
				return gw_cli_arg_help(cli, argc > 1, NULL  );
				break;
		}
	}

	if(argc == 1)
	{
		en = atoi(argv[0]);
		if(GW_OK != call_gwdonu_if_api(LIB_IF_ATU_LEARN_SET, 1, en))
			gw_cli_print(cli, "atu learning set %s fail!\r\n", en?"enable":"disable");
	}
	else
	{
		if(GW_OK != call_gwdonu_if_api(LIB_IF_ATU_LEARN_GET, 1, &en))
			gw_cli_print(cli, "get atu learning fail!\r\n");
		else
			gw_cli_print(cli, "atu learning %s\r\n", en?"enable":"disable");
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
					"<0-600>", "l2 age time unit sec, 0: disable aging");
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
	gw_cli_register_command(cmd_root, atu, "age", cmd_oam_atu_learn, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "age set");

	c = gw_cli_register_command(cmd_root, NULL, "vlan", NULL, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "vlan command");
	c = gw_cli_register_command(cmd_root, c, "port", NULL, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "port command");
	gw_cli_register_command(cmd_root, c, "isolate", cmd_oam_port_isolate, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "isolate command");
	
    return;
}
