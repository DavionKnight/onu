
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

void gw_cli_reg_oam_cmd(struct cli_command **cmd_root)
{
	struct cli_command * portcmd = NULL;

	portcmd = gw_cli_register_command(cmd_root, NULL, "port", NULL,  PRIVILEGE_UNPRIVILEGED, MODE_ANY, "port config or get");
	gw_cli_register_command(cmd_root, portcmd, "mode", cmd_oam_port_mode, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "mode config");
	
    return;
}
