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
#include "../include/gwdonuif.h"
extern broadcast_storm_s broad_storm;
char   g_cSetTime[20]= {0};
#define PORT_DOWN_ENABLE 1
#define PORT_DOWN_DISABLE 0

#define BC_STORM_THRESHOLD_MAX 2000000
#define BC_STORM_THRESHOLD_LAS 10
static void show_port_statistic(struct cli_def * cli, int portid)
{

#define DUMP_PORT_STAT_FMT "  %-24s:  %-22llu  %-24s:  %-22llu"
#define DUMP_PORT_STAT_FMT32 "  %-24s:  %-22u  %-24s:  %-22u"
#if 0
	if (portid == 0xff) {
		int i = 0;
		for (i = 1; i <= 4; i++)
			{
				gw_cli_print(cli,"===========================port %d stat===========================",i);
				show_port_statistic(cli, i);
			}
	} else {
#endif
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
						"In bytes rate", pd->rxrate, "Out bytes rate", pd->txrate);
				gw_cli_print(cli, DUMP_PORT_STAT_FMT,
						"In bytes", pd->counter.RxOctetsOKLsb, "Out bytes", pd->counter.TxOctetsOk);
				gw_cli_print(cli, DUMP_PORT_STAT_FMT,
						"In total pkts", pd->counter.RxFramesOk, "Out total pkts", pd->counter.TxFramesOk);
				gw_cli_print(cli, DUMP_PORT_STAT_FMT,
						"In unicast pkts",  pd->counter.RxUnicasts, "Out unicast pkts", pd->counter.TxUnicasts);
				gw_cli_print(cli, DUMP_PORT_STAT_FMT,
						"In multicast pkts",  pd->counter.RxMulticasts, "Out multicast pkts", pd->counter.TxMulticasts);
				gw_cli_print(cli, DUMP_PORT_STAT_FMT,
						"In broadcast pkts", 	pd->counter.RxBroadcasts, "Out broadcast pkts",pd->counter.TxBroadcasts);
				gw_cli_print(cli, DUMP_PORT_STAT_FMT, "In pause pkts", pd->counter.RxPause, "Out pause pkts", pd->counter.TxPause);
				gw_cli_print(cli, DUMP_PORT_STAT_FMT,
						"In crc error pkts", pd->counter.RxError, "Out crc error pkts", pd->counter.TxError);
				gw_cli_print(cli, DUMP_PORT_STAT_FMT,	"In jumbo pkts", pd->counter.RxJumboOctets, "Out jumbo pkts", pd->counter.TxJumboOctets);
				gw_cli_print(cli, DUMP_PORT_STAT_FMT, "In undersize pkts", pd->counter.RxUndersize, "Out undersize pkts", (long long unsigned int)0);

			}
			free(data);
		}
}

int cmd_stat_port_show(struct cli_def *cli, char *command, char *argv[], int argc)
{

	int portid = 0;
	int i = 0;
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
		show_port_statistic(cli, portid);
	}
	else
	{		
		for (i = 1; i <= gw_onu_read_port_num(); i++)
			{
				gw_cli_print(cli,"===========================port %d stat===========================",i);
				show_port_statistic(cli, i);
				gw_thread_delay(100);
			}

	}

	

	return CLI_OK;
}

int cmd_bsctrl_policy(struct cli_def *cli, char *command, char *argv[], int argc)
{

	//int portid = 0;
	int len;
	int storm_stat;
	
	if(CLI_HELP_REQUESTED)
	{
		switch (argc)
		{
			case 1:
				return gw_cli_arg_help(cli, 0,
					"[enable|disable]", "enable: port down; disable: only limit rate", NULL );

			default:
				return gw_cli_arg_help(cli, argc > 1, NULL  );
		}
	}
	len = strlen(argv[0]);
	if(argc == 0)
		{
			storm_stat = broad_storm.gulBcStormStat? 1:0;
			if(storm_stat)
				gw_cli_print(cli,"port will be link down when broadcast storm happened.");
			else
				gw_cli_print(cli,"Port will be rate limit when broadcast storm happened.");
		}
	if(argc == 1)
		{
			if((strncmp("enable",argv[0],len)) && (strncmp("disable",argv[0],len)))
				{
					gw_cli_print(cli,"%% Invalid input.\n");
					return CLI_ERROR;
				}
			else
				{
					if(!strncmp("enable",argv[0],len))
						{
							broad_storm.gulBcStormStat = PORT_DOWN_ENABLE;
						}
					if(!strncmp("disable",argv[0],len))
						{
							broad_storm.gulBcStormStat = PORT_DOWN_DISABLE;
						}
					gw_cli_print(cli,"set portdown stat success\n");
				}
		}
	if(argc > 1)
		{
			gw_cli_print(cli,"%% Invalid input.\n");
			return CLI_ERROR;
		}
	return CLI_OK;
}

int cmd_bsctrl_threshold(struct cli_def *cli, char *command, char *argv[], int argc)
{

    gw_uint32 gw_threshold = 0;

    if (CLI_HELP_REQUESTED)
    {

        switch (argc)
        {
        case 1:
            return gw_cli_arg_help(cli, 0, "<10-2000000>", "unit: packets per second", NULL );
        default:
            return gw_cli_arg_help(cli, argc > 1, NULL );
        }

    }

    if (argc == 1)
    {

        gw_threshold = atoi(argv[0]);
        if (gw_threshold > BC_STORM_THRESHOLD_MAX || gw_threshold < BC_STORM_THRESHOLD_LAS)
        {
            gw_cli_print(cli, "set broadcast storm threshold fail\n");
            return CLI_ERROR;
        }
        broad_storm.gulBcStormThreshold = gw_threshold;
        gw_cli_print(cli, "set broadcast storm threshold success\n");
    }
    else
    {
        gw_cli_print(cli, "%% Invalid input.\n");
    }

    return CLI_OK;
}

int cmd_bsctrl_threshold_get(struct cli_def *cli, char *command, char *argv[], int argc)
{
	gw_uint64 threshold;
	
	threshold = broad_storm.gulBcStormThreshold;

	gw_cli_print(cli,"broadcast storm threshold %lld",threshold);
	return CLI_OK;
}
typedef struct gw_timers{
	int gw_year;
	char gw_a;
	short gw_mon;
	char gw_b;
	short gw_day;
	char gw_c;
	short gw_hour;
	char gw_d;
	short gw_min;
	char gw_e;
	short gw_sec;
}rec_cur_time;
extern localtime_tm w_gw_tim;
int curr_time_cheak_and_get(localtime_tm olt_time,localtime_tm *onu_time)
{
	localtime_tm local_time;
	int gw_t;
	int gw_m;
	local_time.tm_sec = onu_time->tm_sec + olt_time.tm_sec;
	local_time.tm_min = onu_time->tm_min + olt_time.tm_min;
	local_time.tm_hour = onu_time->tm_hour + olt_time.tm_hour;
	local_time.tm_mday = onu_time->tm_mday + olt_time.tm_mday - 1;
	local_time.tm_mon = onu_time->tm_mon + olt_time.tm_mon - 1;
	local_time.tm_year = onu_time->tm_year + olt_time.tm_year - 70;
	if(local_time.tm_sec > 60 )
		{
			gw_t = local_time.tm_sec / 60;
			gw_m = local_time.tm_sec - gw_t*60;
			local_time.tm_min +=gw_t;
			local_time.tm_sec =gw_m;
		}
	
	if(local_time.tm_min > 60)
		{
			gw_t = local_time.tm_min / 60;
			gw_m = local_time.tm_min - gw_t*60;
			local_time.tm_hour +=gw_t;
			local_time.tm_min = gw_m;
		}
	
	if(local_time.tm_hour > 24)
		{
			gw_t = local_time.tm_hour / 24;
			gw_m = local_time.tm_hour - gw_t*24;
			local_time.tm_mday +=gw_t;
			local_time.tm_hour =gw_m;
		}
	
	if(local_time.tm_mday > 31)
		{
			gw_t = local_time.tm_mday / 31;
			gw_m = local_time.tm_mday - gw_t*31;
			local_time.tm_mon +=gw_t;
			local_time.tm_mday =gw_m;
		}
	
	if(local_time.tm_mon > 12)
		{
			gw_t = local_time.tm_mon / 12;
			gw_m = local_time.tm_mon - gw_t*12;
			local_time.tm_year +=gw_t;
			local_time.tm_mon =gw_m;
		}
	
	memcpy(onu_time,&local_time,sizeof(localtime_tm));
	return 1;
		
}
int cmd_timer_show(struct cli_def *cli, char *command, char *argv[], int argc)
{
	
	
	gw_cli_print(cli,"======================================================================\n");
	localtime_tm tm;
	memcpy(&tm,&w_gw_tim,sizeof(localtime_tm));
	gw_cli_print(cli, "one:%.4d-%.2d-%.2d %.2d:%.2d:%.2d \n",
    tm.tm_year, tm.tm_mon+1, tm.tm_mday,tm.tm_hour, tm.tm_min, tm.tm_sec);
	call_gwdonu_if_api(LIB_IF_LOCALTIME_GET, 1, &tm);
	curr_time_cheak_and_get(w_gw_tim,&tm);
    gw_cli_print(cli, "two:%.4d-%.2d-%.2d %.2d:%.2d:%.2d \n",
    tm.tm_year, tm.tm_mon+1, tm.tm_mday,tm.tm_hour, tm.tm_min, tm.tm_sec);
		
	gw_cli_print(cli,"======================================================================\n");

    return 1;
	
}
int gw_time_get(localtime_tm *tm)
{
	memcpy(&tm,&w_gw_tim,sizeof(localtime_tm));
    call_gwdonu_if_api(LIB_IF_LOCALTIME_GET, 1, tm);
	curr_time_cheak_and_get(w_gw_tim,tm);
	
	return 0;
}


void gw_cli_reg_native_cmd(struct cli_command **cmd_root)
{

	struct cli_command * stat = NULL, *cp = NULL,*time_get = NULL;


	stat = gw_cli_register_command(cmd_root, NULL, "stat", NULL,  PRIVILEGE_UNPRIVILEGED, MODE_ANY, "stat command");
	gw_cli_register_command(cmd_root, stat, "port_show", cmd_stat_port_show, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "port statistic show");

	 // portdown {[enable|disable]}*1
	cp = gw_cli_register_command(cmd_root, NULL, "broadcast", NULL, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Broadcast config");
	cp = gw_cli_register_command(cmd_root, cp, "storm", NULL, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Broadcast storm config");
	gw_cli_register_command(cmd_root, cp, "portdown", cmd_bsctrl_policy, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "port down config");
	gw_cli_register_command(cmd_root, cp, "threshold", cmd_bsctrl_threshold, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "threshold config");
	gw_cli_register_command(cmd_root, cp, "threshold_get", cmd_bsctrl_threshold_get, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "threshold config get");
//	time_get = gw_cli_register_command(cmd_root, NULL, "gwd", NULL, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "show port statistics");
//	gw_cli_register_command(cmd_root, time_get, "time_get", cmd_timer_show, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "show port statistics");

    return;
}
