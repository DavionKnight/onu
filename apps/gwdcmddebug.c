#include "../cli_lib/cli_common.h"
#include "oam.h"


extern int gwd_debug_tvm_flag;
int cmd_gwd_debug(struct cli_def *cli, char *command, char *argv[], int argc)
{
    
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
        case 1:
            gw_cli_arg_help(cli, 0,
                "<tvm>", "tvm debug infomation",
                 NULL);
            return gw_cli_arg_help(cli, 0,
                "<cr>", " ",
                 NULL);
        case 2:
            gw_cli_arg_help(cli, 0,
                "<on>", " ",
                 NULL);
            return gw_cli_arg_help(cli, 0,
                "<off>", " ",
                 NULL);
        default:
            return gw_cli_arg_help(cli, argc > 1, NULL);
        }
    }

    if(2 == argc)
    {
        if(strcmp(argv[0],"tvm") == 0)
        {
            if(strcmp(argv[1],"on") == 0)
            {
                gwd_debug_tvm_flag = 1;
            }
            else if(strcmp(argv[1],"off") == 0)
            {
                gwd_debug_tvm_flag = 0;
            }
            else
            {
                return GWD_RETURN_ERR;
            }
        }
    }
    else
    {
        return GWD_RETURN_ERR;
    }

    return GWD_RETURN_OK;
}

void gw_cli_debug_cmd(struct cli_command **cmd_root)
{
    gw_cli_register_command(cmd_root, NULL, "gwdebug", cmd_gwd_debug, PRIVILEGE_PRIVILEGED, MODE_CONFIG, "gwd platform debug command line");
      
}

