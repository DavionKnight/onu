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

static void show_port_statistic(struct cli_def * cli, int portid)
{

#define DUMP_PORT_STAT_FMT "  %-24s:  %-22llu  %-24s:  %-22llu"
#define DUMP_PORT_STAT_FMT32 "  %-24s:  %-22u  %-24s:  %-22u"
	if (portid == 0xff) {
		int i = 0;
		for (i = 1; i <= gw_onu_read_port_num(); i++)
			show_port_statistic(cli, i);
	} else {
		int len = sizeof(gw_onu_port_counter_t);
		char *data = malloc(len);

		if (data) {
			memset(data, 0, len);
			if (GW_OK
					!= call_gwdonu_if_api(LIB_IF_PORT_STATISTIC_GET, 3, portid,
							data, &len))
				gw_cli_print(cli, "get port %d statistic fail!\r\n", portid);
			else {
				gw_onu_port_counter_t * pd = (gw_onu_port_counter_t*) data;
				gw_cli_print(cli, DUMP_PORT_STAT_FMT32,
						"In bit rate", pd->rxrate, "Out bit rate", pd->txrate);
				gw_cli_print(cli, DUMP_PORT_STAT_FMT,
						"In bytes", pd->counter.RxOctetsOKLsb, "Out bytes", pd->counter.TxOctetsOk);
				gw_cli_print(cli, DUMP_PORT_STAT_FMT,
						"In total pkts", pd->counter.RxFramesOk, "Out bytes", pd->counter.TxFramesOk);
				gw_cli_print(cli, DUMP_PORT_STAT_FMT,
						"In unicast pkts",  pd->counter.RxUnicasts, "Out bytes", pd->counter.TxUnicasts);
				gw_cli_print(cli, DUMP_PORT_STAT_FMT,
						"In multicast pkts",  pd->counter.RxMulticasts, "Out multicast pkts", pd->counter.TxMulticasts);
				gw_cli_print(cli, DUMP_PORT_STAT_FMT,
						"In broadcast pkts", 	pd->counter.RxBroadcasts, "Out broadcast pkts",pd->counter.TxBroadcasts);
				gw_cli_print(cli, DUMP_PORT_STAT_FMT, "In pause pkts", pd->counter.RxPause, "Out pause pkts", pd->counter.TxPause);
				gw_cli_print(cli, DUMP_PORT_STAT_FMT,
						"In crc error pkts", pd->counter.RxError, "Out crc error pkts", pd->counter.TxError);
				gw_cli_print(cli, DUMP_PORT_STAT_FMT,	"In jumbo pkts", pd->counter.RxOversize, "Out jumbo pkts", pd->counter.TxTooLongFrames);
				gw_cli_print(cli, DUMP_PORT_STAT_FMT, "In undersize pkts", pd->counter.RxUndersize, "Out undersize pkts", (long long unsigned int)0);

			}
			free(data);
		}
	}
}

int cmd_stat_port_show(struct cli_def *cli, char *command, char *argv[], int argc)
{

	int portid = 0;

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
		portid = atoi(argv[0]);
	}
	else
	{
		portid = 0xff;
	}

	show_port_statistic(cli, portid);

	return CLI_OK;
}

int cmd_bsctrl_policy(struct cli_def *cli, char *command, char *argv[], int argc)
{

	int portid = 0;

	if(CLI_HELP_REQUESTED)
	{
		switch (argc)
		{
			case 1:
				return gw_cli_arg_help(cli, 0,
					"[enable|disable]", "enable: port down; disable: only limit rate", NULL );
				break;

			default:
				return gw_cli_arg_help(cli, argc > 1, NULL  );
				break;
		}
	}


	return CLI_OK;
}

int cmd_bsctrl_threshold(struct cli_def *cli, char *command, char *argv[], int argc)
{

	int portid = 0;

	if(CLI_HELP_REQUESTED)
	{
		switch (argc)
		{
			case 1:
				return gw_cli_arg_help(cli, 0,
					"<10-2000000>", "unit: packets per second", NULL );
				break;

			default:
				return gw_cli_arg_help(cli, argc > 1, NULL  );
				break;
		}
	}


	return CLI_OK;
}

void gw_cli_reg_native_cmd(struct cli_command **cmd_root)
{

	struct cli_command * stat = NULL, *cp = NULL;


	stat = gw_cli_register_command(cmd_root, NULL, "stat", NULL,  PRIVILEGE_UNPRIVILEGED, MODE_ANY, "stat command");
	gw_cli_register_command(cmd_root, stat, "port_show", cmd_stat_port_show, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "port statistic show");

	 // portdown {[enable|disable]}*1
	cp = gw_cli_register_command(cmd_root, NULL, "broadcast", NULL, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Broadcast config");
	cp = gw_cli_register_command(cmd_root, cp, "storm", NULL, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Broadcast storm config");
	gw_cli_register_command(cmd_root, cp, "portdown", cmd_bsctrl_policy, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "port down config");
	gw_cli_register_command(cmd_root, cp, "threshold", cmd_bsctrl_threshold, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "threshold config");


    return;
}
