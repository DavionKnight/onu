/*
 * gw_cli_commands.c
 *
 *  Created on: 2012-12-25
 *      Author: tommy
 */


#include "../include/gw_os_api_core.h"
#include "../include/gw_timer.h"
#include "../cli_lib/cli_common.h"
#include "gw_log.h"
#include "gwdonuif_interval.h"

int cmd_stat_port_show(struct cli_def *cli, char *command, char *argv[], int argc)
{

	int portid = 0;
	char data[256]="";

	if(CLI_HELP_REQUESTED)
	{
		switch (argc)
		{
			case 1:
				return gw_cli_arg_help(cli, 0,
					"<port_list>", "Input one fe port number", NULL );
				break;

			default:
				return gw_cli_arg_help(cli, argc > 1, NULL  );
				break;
		}
	}

	if(argc == 1)
	{
		int len = 256;
		portid = atoi(argv[0]);


		if(GW_OK != call_gwdonu_if_api(LIB_IF_PORT_STATISTIC_GET, 3,portid, data, &len))
			gw_cli_print(cli, "get port %d statistic fail!\r\n", portid);
		else
		{

		}

	}
	else
	{
		gw_cli_print(cli, GW_CLI_INCOMPLETE_MSG);
		return CLI_ERROR;
	}



	return CLI_OK;
}

void gw_cli_reg_native_cmd(struct cli_command **cmd_root)
{
	struct cli_command * stat = NULL;


	stat = gw_cli_register_command(cmd_root, NULL, "stat", NULL,  PRIVILEGE_UNPRIVILEGED, MODE_ANY, "stat command");
	gw_cli_register_command(cmd_root, stat, "port_show", cmd_stat_port_show, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "port statistic show");


    return;
}
