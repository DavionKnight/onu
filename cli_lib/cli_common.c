//#include "plat_common.h"
#include "cli_common.h"
#include "../apps/gw_log.h"

#define MAX_USER_CMD_INIT_HANDLER 16
#define USER_CMD_INIT_HANDLE_DESC_LEN 32

typedef struct{
	char desc[USER_CMD_INIT_HANDLE_DESC_LEN];
	USER_CMD_INIT func;
}user_init_handler_t;

user_init_handler_t g_user_cmd_init_handlers[MAX_USER_CMD_INIT_HANDLER];

void userCmdInitHandlerInit()
{
	memset(g_user_cmd_init_handlers, 0, sizeof(g_user_cmd_init_handlers));
}

int registerUserCmdInitHandler(const char * desc, USER_CMD_INIT handler)
{
	int i, ret = GW_ERROR, fblank = -1;

	if(desc && handler)
	{
		for(i=0; i<MAX_USER_CMD_INIT_HANDLER; i++)
		{
			user_init_handler_t * phandle = &g_user_cmd_init_handlers[i];
			if(phandle->func == handler)
			{
				ret = GW_OK;
				break;
			}
			else if(phandle->func == NULL)
			{
				if(fblank == -1)
					fblank = i;
				continue;
			}
			else
				continue;
		}

		if(ret == GW_ERROR && fblank != -1)
		{
			int len = strlen(desc) >= (USER_CMD_INIT_HANDLE_DESC_LEN-1)? (USER_CMD_INIT_HANDLE_DESC_LEN-1):strlen(desc);
			strncpy(g_user_cmd_init_handlers[fblank].desc, desc, len);
			g_user_cmd_init_handlers[fblank].func = handler;
			ret = GW_OK;
		}
	}

	return ret;
}

//#ifdef HAVE_TELNET_CLI
struct cli_command *gw_cmd_tree = NULL;
struct cli_command gw_cli_command[GW_MAX_CLI_COMMAND];


//extern void cli_reg_usr_cmd(struct cli_command **cmd_root);
extern void gw_user_register_command_entry(struct cli_command **cmd_root);
extern int gw_cli_show_help(struct cli_def *cli, struct cli_command *c);

int gw_cli_int_help(struct cli_def *cli, char *command, char *argv[], int argc)
{
    if (CLI_HELP_REQUESTED)
        return CLI_HELP_NO_ARGS;

    gw_cli_error(cli, "\nCommands available:");
    gw_cli_show_help(cli, cli->commands);
    return CLI_OK;
}

int gw_cli_int_history(struct cli_def *cli, char *command, char *argv[], int argc)
{
    int i;

    if (CLI_HELP_REQUESTED)
        return CLI_HELP_NO_ARGS;

    gw_cli_error(cli, "\nCommand history:");
    for (i = 0; i < MAX_HISTORY; i++)
    {
        if (strlen(&cli->history[i][0]))
            gw_cli_error(cli, "%3d. %s", i, cli->history[i]);
    }

    return CLI_OK;
}

int gw_cli_int_enable(struct cli_def *cli, char *command, char *argv[], int argc)
{
    if (CLI_HELP_REQUESTED)
        return CLI_HELP_NO_ARGS;

    if (cli->privilege == PRIVILEGE_PRIVILEGED)
        return CLI_OK;

    if (!cli->enable_password && !cli->enable_callback)
    {
        /* no password required, set privilege immediately */
        gw_cli_set_privilege(cli, PRIVILEGE_PRIVILEGED);
        gw_cli_set_configmode(cli, MODE_EXEC, NULL);
    }
    else
    {
        /* require password entry */
        cli->state = STATE_ENABLE_PASSWORD;
    }

    return CLI_OK;
}

int gw_cli_int_disable(struct cli_def *cli, char *command, char *argv[], int argc)
{
    if (CLI_HELP_REQUESTED)
        return CLI_HELP_NO_ARGS;

    gw_cli_set_privilege(cli, PRIVILEGE_UNPRIVILEGED);
    gw_cli_set_configmode(cli, MODE_EXEC, NULL);
    return CLI_OK;
}

int gw_cli_int_quit(struct cli_def *cli, char *command, char *argv[], int argc)
{
    if (CLI_HELP_REQUESTED)
        return CLI_HELP_NO_ARGS;

    if(argc > 0)
    {
        gw_cli_print(cli, "%% Invalid input.");
        return CLI_OK;
    }

    gw_cli_set_privilege(cli, PRIVILEGE_UNPRIVILEGED);
    gw_cli_set_configmode(cli, MODE_EXEC, NULL);
    return CLI_QUIT;
}


int gw_cli_int_exit(struct cli_def *cli, char *command, char *argv[], int argc)
{
    if (CLI_HELP_REQUESTED)
        return CLI_HELP_NO_ARGS;

    if(argc > 0)
    {
        gw_cli_print(cli, "%% Invalid input.");
        return CLI_OK;
    }

    if (cli->mode == MODE_EXEC)
        return gw_cli_int_quit(cli, command, argv, argc);

    if (cli->mode > MODE_CONFIG)
        gw_cli_set_configmode(cli, MODE_CONFIG, NULL);
    else
        gw_cli_set_configmode(cli, MODE_EXEC, NULL);

    cli->service = NULL;
    return CLI_OK;
}


int gw_cli_set_configmode(struct cli_def *cli, int mode, char *config_desc)
{
     static char string[64];
    int old = cli->mode;
    cli->mode = mode;

    if (mode != old)
    {
        if (!cli->mode)
        {
            // Not config mode
            gw_cli_set_modestring(cli, NULL);
        }
        else if (config_desc && *config_desc)
        {
            snprintf(string, sizeof(string), "(config-%s)", config_desc);
            gw_cli_set_modestring(cli, string);
        }
        else
        {
	   snprintf(string, sizeof(string), "(config)");
            gw_cli_set_modestring(cli, string);
        }

    }

    return old;
}

int gw_cli_interface_debug_terminal(struct cli_def *cli, char *command, char *argv[], int argc)
{
	
    if (CLI_HELP_REQUESTED)
        return CLI_HELP_NO_ARGS;

    if(argc > 0)
    {
        gw_cli_print(cli, "%% Invalid input.");
        return CLI_OK;
    }

    gw_cli_set_configmode(cli, MODE_DEBUG,"advdebug");
    gw_cli_set_privilege(cli, PRIVILEGE_PRIVILEGED);
    return CLI_OK;
}

struct cli_command *gw_cli_tree_init()
{
    struct cli_command *cmd_root = NULL;
    
    memset(&gw_cli_command[0],0,sizeof(struct cli_command)*GW_MAX_CLI_COMMAND);
    // build-in standard commands
    gw_cli_register_command(&cmd_root, 0, "help",    gw_cli_int_help,       PRIVILEGE_UNPRIVILEGED, MODE_ANY,   "Show available commands");
    gw_cli_register_command(&cmd_root, 0, "history", gw_cli_int_history,    PRIVILEGE_UNPRIVILEGED, MODE_ANY,   "Show a list of previously run commands");
    gw_cli_register_command(&cmd_root, 0, "t_enable",  gw_cli_int_enable,     PRIVILEGE_UNPRIVILEGED, MODE_EXEC,  "Turn on privileged commands");
    gw_cli_register_command(&cmd_root, 0, "t_disable", gw_cli_int_disable,    PRIVILEGE_PRIVILEGED,   MODE_EXEC,  "Turn off privileged commands");
    gw_cli_register_command(&cmd_root, 0, "quit",    gw_cli_int_quit,       PRIVILEGE_UNPRIVILEGED, MODE_ANY,   "Disconnect");
    gw_cli_register_command(&cmd_root, 0, "logout",  gw_cli_int_quit,       PRIVILEGE_UNPRIVILEGED, MODE_ANY,   "Disconnect");
    gw_cli_register_command(&cmd_root, 0, "exit",    gw_cli_int_exit,       PRIVILEGE_UNPRIVILEGED, MODE_ANY,   "Exit from current mode");
    gw_cli_register_command(cmd_root, NULL, "advdebug",gw_cli_interface_debug_terminal, PRIVILEGE_PRIVILEGED,   MODE_CONFIG,    "Enter debug mode");

    // reg demo coammnds
//    cli_reg_usr_cmd(&cmd_root);

//     gw_user_register_command_entry(&cmd_root);
     {
    	 int i;
    	 for(i=0; i<MAX_USER_CMD_INIT_HANDLER; i++)
    	 {
    		 if(g_user_cmd_init_handlers[i].func != NULL)
    		 {
    			 (*g_user_cmd_init_handlers[i].func)(&cmd_root);
    			 gw_printf("%s cmds register ok!\r\n", g_user_cmd_init_handlers[i].desc);
    		 }
    		 else
    		 {
    			 if(g_user_cmd_init_handlers[i].desc[0])
    				 gw_printf("%s cmds register fail!\r\n", g_user_cmd_init_handlers[i].desc);
    		 }
    	 }
     }

    return cmd_root;
}

extern int g_pty_slave;
void cli_start()
{
    struct cli_def *cli = NULL;

    // init command tree
    if(gw_cmd_tree == NULL)
    {
		gw_cmd_tree = gw_cli_tree_init();
		if(!gw_cmd_tree)
		{
			gw_printf("--------root cmd init fail--------!\r\n");
				return;
		}
    }

    // init console session
    cli = gw_cli_init(gw_cmd_tree, CHANNEL_PTY);
    if(NULL == cli)
    {
    	gw_printf("--------cli_init fail--------\r\n");
        	return;
    }

    // configure session
    cli->sockfd = g_pty_slave;
    cli->channel = CHANNEL_PTY;
    gw_cli_set_banner(cli, CLI_BANNER);
    gw_cli_set_hostname(cli, HOST_NAME);

#if 1
    gw_cli_loop(cli);

    gw_cli_done(cli);
#endif

    return;
}

void cli_console_start(gw_int32 type, gw_int32 fd)
{
    struct cli_def *cli = NULL;

    // init command tree
    if(gw_cmd_tree == NULL)
    {
        gw_cmd_tree = gw_cli_tree_init();
        if(!gw_cmd_tree)
        {
            gw_printf("--------root cmd init fail--------!\r\n");
                return;
        }
    }

    // init console session
    cli = gw_cli_init(gw_cmd_tree, type);
    if(NULL == cli)
    {
        gw_printf("--------cli_init fail--------\r\n");
            return;
    }

    // configure session
    cli->sockfd = fd;
    cli->channel = type;
    gw_cli_set_banner(cli, CLI_BANNER);
    gw_cli_set_hostname(cli, HOST_NAME);

#if 1
    gw_cli_loop(cli);

    gw_cli_done(cli);
#endif

    return;
}


void oam_cli_start()
{
    struct cli_def *cli = NULL;

    // init command tree
    if(gw_cmd_tree == NULL)
    {
		gw_cmd_tree = gw_cli_tree_init();
		if(!gw_cmd_tree)
		{
			gw_printf("--------root cmd init fail--------!\r\n");
				return;
		}
    }

    // init console session
    cli = gw_cli_init(gw_cmd_tree, CHANNEL_OAM);
    if(NULL == cli)
    {
    	gw_printf("--------cli_init fail--------\r\n");
        	return;
    }

    // configure session
    //cli->sockfd = g_pty_slave;
    cli->channel = CHANNEL_OAM;
    gw_cli_set_banner(cli, CLI_BANNER);
    gw_cli_set_hostname(cli, HOST_NAME);
}

//#endif

