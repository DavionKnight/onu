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
#include "../qos/qos.h"

extern broadcast_storm_s broad_storm;
char   g_cSetTime[20]= {0};
#define PORT_DOWN_ENABLE 1
#define PORT_DOWN_DISABLE 0

#define BC_STORM_THRESHOLD_MAX 2000000
#define BC_STORM_THRESHOLD_LAS 10
int cli_printf_delay = 100;
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
                gw_thread_delay(cli_printf_delay);                
				gw_cli_print(cli, DUMP_PORT_STAT_FMT,
						"In bytes", pd->counter.RxOctetsOKLsb, "Out bytes", pd->counter.TxOctetsOk);
                gw_thread_delay(cli_printf_delay);                
                
				gw_cli_print(cli, DUMP_PORT_STAT_FMT,
						"In total pkts", pd->counter.RxFramesOk, "Out total pkts", pd->counter.TxFramesOk);
                gw_thread_delay(cli_printf_delay);                
                
				gw_cli_print(cli, DUMP_PORT_STAT_FMT,
						"In unicast pkts",  pd->counter.RxUnicasts, "Out unicast pkts", pd->counter.TxUnicasts);
                gw_thread_delay(cli_printf_delay);                
                
				gw_cli_print(cli, DUMP_PORT_STAT_FMT,
						"In multicast pkts",  pd->counter.RxMulticasts, "Out multicast pkts", pd->counter.TxMulticasts);
                gw_thread_delay(cli_printf_delay);                
                
				gw_cli_print(cli, DUMP_PORT_STAT_FMT,
						"In broadcast pkts", 	pd->counter.RxBroadcasts, "Out broadcast pkts",pd->counter.TxBroadcasts);
                gw_thread_delay(cli_printf_delay);                
                
				gw_cli_print(cli, DUMP_PORT_STAT_FMT, "In pause pkts", pd->counter.RxPause, "Out pause pkts", pd->counter.TxPause);
                gw_thread_delay(cli_printf_delay);                
                
				gw_cli_print(cli, DUMP_PORT_STAT_FMT,
						"In crc error pkts", pd->counter.RxFCSErrors, "Out crc error pkts", pd->counter.TxError);
                gw_thread_delay(cli_printf_delay);                
                
				gw_cli_print(cli, DUMP_PORT_STAT_FMT,	"In jumbo pkts", pd->counter.RxJumboOctets, "Out jumbo pkts", pd->counter.TxJumboOctets);
                gw_thread_delay(cli_printf_delay);                
                
				gw_cli_print(cli, DUMP_PORT_STAT_FMT, "In undersize pkts", pd->counter.RxUndersize, "Out undersize pkts", (long long unsigned int)0);
                gw_thread_delay(cli_printf_delay);                                

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
					"<1-%d>", gw_onu_read_port_num(), "Input one fe port number", NULL );
				break;

			default:
				return gw_cli_arg_help(cli, argc > 1, NULL  );
				break;
		}
	}

	if(argc == 1)
	{
		portid = atoi(argv[0]);
		if(portid > 0 && portid <= gw_onu_read_port_num())
		{
			gw_cli_print(cli,"\n===========================port %d stat===========================",portid);
			show_port_statistic(cli, portid);
		}
		else
			return CLI_ERROR_ARG;
	}
	else
	{		
		for (i = 1; i <= gw_onu_read_port_num(); i++)
			{
				gw_cli_print(cli,"\n===========================port %d stat===========================",i);
                gw_thread_delay(cli_printf_delay);                                
				show_port_statistic(cli, i);                
			}
	}

	

	return CLI_OK;
}

int cmd_bsctrl_policy(struct cli_def *cli, char *command, char *argv[], int argc)
{

    //int portid = 0;
    int len;
    int storm_stat;

    if (CLI_HELP_REQUESTED)
    {
        switch (argc)
        {
        case 1:
            return gw_cli_arg_help(cli, 0, "[enable|disable]", "enable: port down; disable: only limit rate", NULL );

        default:
            return gw_cli_arg_help(cli, argc > 1, NULL );
        }
    }

    if (argc == 0)
    {
        storm_stat = broad_storm.gulBcStormStat ? 1 : 0;
        if (storm_stat)
            gw_cli_print(cli, "port will be link down when broadcast storm happened.");
        else
            gw_cli_print(cli, "Port will be rate limit when broadcast storm happened.");
    }
    if (argc == 1)
    {

        len = strlen(argv[0]);
        if ((strncmp("enable", argv[0], len)) && (strncmp("disable", argv[0], len)))
        {
            gw_cli_print(cli, "%% Invalid input.\n");
            return CLI_ERROR;
        }
        else
        {
            if (!strncmp("enable", argv[0], len))
            {
                broad_storm.gulBcStormStat = PORT_DOWN_ENABLE;
            }
            if (!strncmp("disable", argv[0], len))
            {
                broad_storm.gulBcStormStat = PORT_DOWN_DISABLE;
            }
            gw_cli_print(cli, "set portdown stat success\n");
        }
    }
    if (argc > 1)
    {
        gw_cli_print(cli, "%% Invalid input.\n");
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
            return gw_cli_arg_help(cli, 0, "{<10-2000000>}*1", "unit: packets per second", NULL );
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
        gw_uint64 threshold;

        threshold = broad_storm.gulBcStormThreshold;

        gw_cli_print(cli,"broadcast storm threshold %lld",threshold);
    }

    return CLI_OK;
}

/*
int cmd_bsctrl_threshold_get(struct cli_def *cli, char *command, char *argv[], int argc)
{
	gw_uint64 threshold;
	
	threshold = broad_storm.gulBcStormThreshold;

	gw_cli_print(cli,"broadcast storm threshold %lld",threshold);
	return CLI_OK;
}
*/

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

#if (RPU_MODULE_NOT_USE == RPU_YES)
int cmd_timer_show(struct cli_def *cli, char *command, char *argv[], int argc)
{
	
	if (CLI_HELP_REQUESTED)
    {

        switch (argc)
        {

            default:
                return gw_cli_arg_help(cli, argc > 1, NULL );
        }

    }
	gw_cli_print(cli,"======================================================================\n");

    gw_thread_delay(1000);
	localtime_tm tm;
    gw_thread_delay(1000);
    gw_cli_print(cli,"time show \r\n");
	memcpy(&tm,&w_gw_tim,sizeof(localtime_tm));

    gw_thread_delay(1000);
	gw_cli_print(cli, "one:%.4d-%.2d-%.2d %.2d:%.2d:%.2d \n",
    tm.tm_year, tm.tm_mon+1, tm.tm_mday,tm.tm_hour, tm.tm_min, tm.tm_sec);
	call_gwdonu_if_api(LIB_IF_LOCALTIME_GET, 1, &tm);
	curr_time_cheak_and_get(w_gw_tim,&tm);
    gw_cli_print(cli, "two:%.4d-%.2d-%.2d %.2d:%.2d:%.2d \n",
    tm.tm_year, tm.tm_mon+1, tm.tm_mday,tm.tm_hour, tm.tm_min, tm.tm_sec);
		
	gw_cli_print(cli,"======================================================================\n");

    return 1;
	
}
#endif

int cmd_qos_vlan_queue_map(struct cli_def *cli, char *command, char *argv[], int argc)
{

    int ret = CLI_ERROR;

    if (CLI_HELP_REQUESTED)
    {

        switch (argc)
        {
        case 1:
            return gw_cli_arg_help(cli, 0, "<1-%d>", gw_onu_read_port_num()+2, "Input one port number", NULL );
        case 2:
        	return gw_cli_arg_help(cli, 0, "<2-%d>", 4092, "Input vlan id", NULL);
        case 3:
        	return gw_cli_arg_help(cli, 0, "<0-7>", "Input a queue value", NULL);
        default:
            return gw_cli_arg_help(cli, argc > 1, NULL );
        }

    }

    if(argc >= 3)
    {
    	gw_uint8 port = atoi(argv[0]);
    	gw_uint32 vlan = atoi(argv[1]);
    	gw_uint32 queue = atoi(argv[2]);

    	if(gw_qos_vlan_queue_add(port, vlan, queue) == GW_OK)
    		ret = CLI_OK;
    	else
    		gw_cli_print(cli, "add qos vlan id queue map fail!");
    }

    return ret;
}

int cmd_qos_vlan_queue_map_del(struct cli_def *cli, char *command, char *argv[], int argc)
{

    int ret = CLI_ERROR;

    if (CLI_HELP_REQUESTED)
    {

        switch (argc)
        {
        case 1:
            return gw_cli_arg_help(cli, 0, "<1-%d>", gw_onu_read_port_num()+2, "Input one port number", NULL );
        case 2:
        	return gw_cli_arg_help(cli, 0, "<2-%d>", 4092, "Input vlan id", NULL);
        default:
            return gw_cli_arg_help(cli, argc > 1, NULL );
        }

    }

    if(argc >= 2)
    {
    	gw_uint8 port = atoi(argv[0]);
    	gw_uint32 vlan = atoi(argv[1]);

    	if(gw_qos_vlan_queue_remove(port, vlan) == GW_OK)
    		ret = CLI_OK;
    	else
    		gw_cli_print(cli, "del qos vlan id queue map fail!");
    }

    return ret;
}

int cmd_qos_vlan_queue_map_apply(struct cli_def *cli, char *command, char *argv[], int argc)
{

    int ret = CLI_ERROR;
    int reset = 0;

    if (CLI_HELP_REQUESTED)
    {

    	switch (argc)
		{

    	case 1:
    		return gw_cli_arg_help(cli, 0, "reset", "reset qos vid-queue map config", NULL );
    	default:
        	return gw_cli_arg_help(cli, argc > 1, NULL );
		}

    }

    reset = (argc == 1)?1:0;

    if(gw_qos_vlan_queue_rules_apply(reset) != GW_OK)
    	gw_cli_print(cli, "qos vlan queue map apply fail!");
    else
    	ret = CLI_OK;

    return ret;
}

int cmd_qos_vlan_queue_map_show(struct cli_def *cli, char *command, char *argv[], int argc)
{

    int ret = CLI_ERROR, c = 0;
    gw_uint8 port = 0;
    gw_qos_vlan_queue_data_t *pd = NULL;

    if (CLI_HELP_REQUESTED)
    {

        switch (argc)
        {
        case 1:
            return gw_cli_arg_help(cli, 0, "<1-%d>", gw_onu_read_port_num()+2, "Input one port number", NULL );
        default:
            return gw_cli_arg_help(cli, argc > 1, NULL );
        }

    }

    if(argc >= 1)
    	port = atoi(argv[0]);
    else
    	port = 0xff;

    if((c = gw_qos_vlan_queue_entry_get_by_port(port, &pd)))
    {

    	gw_int32 i = 0;

    	gw_cli_print(cli, "%-8s%-8s%-8s%-8s", "id", "port", "vlan", "queue");

    	while(c > 0)
    	{
    		gw_cli_print(cli, "%-8d%-8d%-8d%-8d", i+1, pd[i].port, pd[i].vid, pd[i].queue);
		i++;
		c--;
    	}

    	ret = CLI_OK;
    }
    
    if(pd)
    {
       free(pd);
    }
    return ret;
}

int gw_time_get(localtime_tm *tm)
{
	memcpy(&tm,&w_gw_tim,sizeof(localtime_tm));
    call_gwdonu_if_api(LIB_IF_LOCALTIME_GET, 1, tm);
	curr_time_cheak_and_get(w_gw_tim,tm);
	
	return 0;
}

int igmp_mode_show(struct cli_def *cli,mc_mode_t mc_mode)
{
	int ret = 0;
	char mode[15] = {0};

	switch(mc_mode)
	{
        case MC_SNOOPING:
            strncpy(mode, "snooping", sizeof("snooping"));
            break;

        case MC_MANUAL:
            strncpy(mode, "manual", sizeof("manual"));
            break;

		case MC_PROXY:
            strncpy(mode, "proxy", sizeof("proxy"));
            break;

		case MC_DISABLE:
            strncpy(mode, "transparent", sizeof("transparent"));
            break;

        default:
            strncpy(mode, "unknown", sizeof("unknown"));
            break;
	}
	gw_cli_print(cli,"igmp %s mode\n", mode);

	return ret;
}

int cmd_igmp_mode(struct cli_def *cli, char *command, char *argv[], int argc)
{

	unsigned char en;

	if(CLI_HELP_REQUESTED)
	{
		switch(argc)
		{
			case 1:
				gw_cli_arg_help(cli, 0,
				"<cr>", "just enter to execuse command. (show igmp mode)",
				NULL);
				gw_cli_arg_help(cli, 0,
				"0", "set igmp snooping mode",
				NULL);
				gw_cli_arg_help(cli, 0,
				"1", "set igmp auth mode",
				NULL);
				gw_cli_arg_help(cli, 0,
				"2", "set igmp proxy mode",
				NULL);
				gw_cli_arg_help(cli, 0,
				"3", "set igmp disable mode",
				NULL);
				return CLI_OK;
			case 2:
				return gw_cli_arg_help(cli, 0,
				"<cr>", "just enter to execuse command. (show igmp mode)",
				NULL);
			default:
				return gw_cli_arg_help(cli, argc > 1, NULL);
		}
	}
	if(argc > 1)
	{
		gw_cli_print(cli, "%% Invalid input.");
		return CLI_OK;
	}
	else
	{
		//do nothing
	}

	if(0 == argc)
	{
		mc_mode_t mc_mode;

		if(call_gwdonu_if_api(LIB_IF_MULTICAST_MODE_GET, 1, &mc_mode) != GW_OK)
			gw_cli_print(cli, "Get multicast mode fail!\r\n");
		else
			igmp_mode_show(cli,mc_mode);
	}
	else if(1 == argc)
	{
		unsigned char ret = 0;
		int mode = 0;
		mc_mode_t mc_mode;
		mode = atoi(argv[0]);
		switch(mode)
		{
			case 0:
				mc_mode = MC_SNOOPING;
				break;
			case 1:
				mc_mode = MC_MANUAL;
				break;
			case 2:
				mc_mode = MC_PROXY;
				break;
			case 3:
				mc_mode = MC_DISABLE;
				break;
			default:
				mc_mode = MC_SNOOPING;
				break;
		}

		if(call_gwdonu_if_api(LIB_IF_MULTICAST_MODE_SET, 1,mc_mode) != GW_OK)
			gw_cli_print(cli, "mc_mode_set failed\n");
		else
		{
			gw_cli_print(cli,"mode :0x%x\n", mode);
			gw_cli_print(cli,"mc_mode_set success\n");
		}
	}
	else
	{
		//do nothing
	}

	return CLI_OK;

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
	cp = gw_cli_register_command(cmd_root, NULL, "qos", NULL, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "QoS command");
	gw_cli_register_command(cmd_root, cp, "add-vid-queue-map", cmd_qos_vlan_queue_map, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "vlan queue map add cmd");
	gw_cli_register_command(cmd_root, cp, "del-vid-queue-map", cmd_qos_vlan_queue_map_del, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "vlan queue map del cmd");
	gw_cli_register_command(cmd_root, cp, "show-vid-queue-map", cmd_qos_vlan_queue_map_show, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "show vlan queue map cmd");
	gw_cli_register_command(cmd_root, cp, "apply-vid-queue-map", cmd_qos_vlan_queue_map_apply, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "apply vlan queue map cmd");
	//	gw_cli_register_command(cmd_root, cp, "threshold_get", cmd_bsctrl_threshold_get, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "threshold config get");
	#if (RPU_MODULE_NOT_USE == RPU_YES)
	time_get = gw_cli_register_command(cmd_root, NULL, "gwd", NULL, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "show port statistics");
	gw_cli_register_command(cmd_root, time_get, "time_get", cmd_timer_show, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "show port statistics");
    #endif


    return;
}

