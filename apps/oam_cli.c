
#include "../include/gw_os_api_core.h"
#include "../include/gw_timer.h"
#include "../cli_lib/cli_common.h"
#include "gw_log.h"
#include "oam.h"
#include "gwdonuif_interval.h"


int cmd_oam_port_mode(struct cli_def *cli, char *command, char *argv[], int argc)
{
	gw_cli_print(cli, "\r\nport mode exected!\r\n");
	return CLI_OK;
}

int cmd_oam_atu_learn(struct cli_def *cli, char *command, char *argv[], int argc)
{
	gw_cli_print(cli, "\r\natu learning exected!\r\n");
	return CLI_OK;
}

int cmd_oam_atu_age(struct cli_def *cli, char *command, char *argv[], int argc)
{
	gw_cli_print(cli, "\r\natu age exected!\r\n");
	return CLI_OK;
}

int cmd_oam_port_isolate(struct cli_def *cli, char *command, char *argv[], int argc)
{
	gw_cli_print(cli, "\r\nport isolate exected!\r\n");
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
