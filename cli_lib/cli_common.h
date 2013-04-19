#ifndef __CLI_COMMON_H__
#define __CLI_COMMON_H__

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
//#include "plat_common.h"
#include "../include/gw_os_api_core.h"

#define CLI_OK              0
#define CLI_ERROR           -1
#define CLI_QUIT            -2
#define CLI_ERROR_ARG       -3

#define MAX_HISTORY             10
#define MAX_WORDS_NUM           64

/*#ifdef HAVE_ZTE_OAM
#define MAX_WORDS_LEN           128
#else
#define MAX_WORDS_LEN           32
#endif*/

#define MAX_WORDS_LEN           128
#define MAX_LINE_LENTH         256
#define MAX_FILTER_NUM           8
#define MAX_PRINT_BUF_LEN     1024
#define GW_MAX_CLI_COMMAND       256

#define PRIVILEGE_UNPRIVILEGED  0
#define PRIVILEGE_PRIVILEGED    15

#define CLI_COMMAND_FREE        0x0
#define CLI_COMMAND_BUSY        0xffffffff
#define MODE_ANY                -1
#define MODE_EXEC               0
#define MODE_CONFIG             1
#define MODE_CONFIG_INTF        10
#define MODE_DEBUG              11
#define MODE_SWITCH				2
#ifdef HAVE_ZTE_OAM
#define MODE_ZTE                12
#endif/* END_HAVE_ZTE_OAM */

#define PRINT_PLAIN             0x00
#define PRINT_FILTERED          0x01
#define PRINT_BUFFERED          0x02

#define CHANNEL_SERIAL          1
#define CHANNEL_TCP             2

#define CHANNEL_PTY				3
#define CHANNEL_OAM				4

#define TELNETD_PORT            23
#define DFT_SESSION_TIMEOUT     300 //in seconds for telnet

#define CLI_BANNER              "iROS ONU CLI"
#ifdef HAVE_ZTE_OAM
#define ZTE_CLI_BANNER          "\n\
************************************************\n\
Welcome to ZXAN product of ZTE Corporation\n\
************************************************\n"
#define ZTE_HOST_NAME               "ZXAN"
#endif/* END_HAVE_ZTE_OAM */

#ifdef CYG_HAL_ARM_IMST_OLT
#define HOST_NAME               "OLT"
#else
#define HOST_NAME               "ONU"
#endif

enum cli_states {
    STATE_LOGIN,
    STATE_PASSWORD,
    STATE_NORMAL,
    STATE_ENABLE_PASSWORD,
    STATE_ENABLE
};

#define GW_CLI_INCOMPLETE_MSG "%% Command incomplete.\r\n"

// macros for ? helper in cmd handler
#define CLI_HELP_REQUESTED  (argc > 0 && argv[argc-1][strlen(argv[argc-1])-1] == '?')
#define CLI_HELP_NO_ARGS    ((argc > 1 || argv[0][1]) ? CLI_OK : gw_cli_arg_help(cli, 1, NULL))

/* match if b is a substr of a */
#define MATCH(a,b) (!strncmp((a), (b), strlen(b)))

/* free and zero (to avoid double-free) */
#define free_z(p) do { if (p) { free(p); (p) = 0; } } while (0)

#define CTRL(c) (c - '@')

//#define CS_CONSOLE_BUF_LEN  8192
#define CS_CONSOLE_BUF_LEN  4096

typedef struct {
    gw_uint8 buf[CS_CONSOLE_BUF_LEN];

    gw_uint32 lock; /* console lock */
    gw_uint32 rd_avail;

    gw_uint16 rd_ptr;
    gw_uint16 empty;
    
    gw_uint16 wr_ptr;
    gw_uint16 full;
} gw_console_t;

typedef union {
    unsigned long int lv;
    struct{
        unsigned long int swport:6;
        unsigned long int swslot:2;
        unsigned long int port:6;
        unsigned long int slot:2;
        unsigned long int reserve:18;
    };
}port_idx_u;

typedef union{
    port_idx_u port_u;
    unsigned char mac[6];
    unsigned long int ip;
    void * ptrval;
}gw_cli_index_u;


extern struct cli_command *gpst_cmd_tree;

struct cli_def {
    int completion_callback;
    struct cli_command *commands;
    int (*auth_callback)(char *, char *, struct cli_def *);
    int (*regular_callback)(struct cli_def *cli);
    int (*enable_callback)(char *);
    char *banner;
    struct unp *users;
    char *enable_password;
    char history[MAX_HISTORY][MAX_LINE_LENTH];
    char showprompt;
    char *promptchar;
    char *hostname;
    char *modestring;
    int privilege;
    int mode;
    int state;
    struct cli_filter *filters;
    void (*print_callback)(struct cli_def *cli, char *string);
#ifdef HAVE_TELNET_CLI
    int sockfd;
    FILE *client;
#endif
    int channel;
    /* internal buffers */
    void *conn;
    void *service;
    char commandname[MAX_LINE_LENTH];  // temporary buffer for cli_command_name() to prevent leak
    char *buffer;
    unsigned buf_size;

#ifdef HAVE_TELNET_CLI
    struct timeval timeout_tm;
#endif
    unsigned int idle_timeout;
    time_t last_action;
    gw_cli_index_u index;
};

struct cli_filter {
    int (*filter)(struct cli_def *cli, char *string, void *data);
    void *data;
    struct cli_filter *next;
};

struct cli_command {
    char *command;
    int (*callback)(struct cli_def *, char *, char **, int);
    unsigned int unique_len;
    char *help;
    int privilege;
    int mode;
    int usage;
    struct cli_command *next;
    struct cli_command *children;
    struct cli_command *parent;
};

typedef void (*USER_CMD_INIT)(struct cli_command **);

int registerUserCmdInitHandler(const char * desc, USER_CMD_INIT handler);
void userCmdInitHandlerInit(void);

struct cli_command *gw_cli_tree_init();
struct cli_def *gw_cli_init(struct cli_command *cmd_root, int channel);
int gw_cli_done(struct cli_def *cli);
struct cli_command *gw_cli_register_command(struct cli_command **cmd_root, struct cli_command *parent, char *command, int (*callback)(struct cli_def *cli, char *, char **, int), int privilege, int mode, char *help);
int gw_cli_unregister_command(struct cli_command **cmd_root, char *command);
int gw_cli_run_command(struct cli_def *cli, char *command);
int gw_cli_loop(struct cli_def *cli);
int gw_cli_file(struct cli_def *cli, FILE *fh, int privilege, int mode);
void gw_cli_set_auth_callback(struct cli_def *cli, int (*auth_callback)(char *, char *, struct cli_def *));
void gw_cli_set_enable_callback(struct cli_def *cli, int (*enable_callback)(char *));
void gw_cli_allow_user(struct cli_def *cli, char *username, char *password);
void gw_cli_allow_enable(struct cli_def *cli, char *password);
void gw_cli_deny_user(struct cli_def *cli, char *username);
void gw_cli_set_banner(struct cli_def *cli, char *banner);
void gw_cli_set_hostname(struct cli_def *cli, char *hostname);
void gw_cli_set_promptchar(struct cli_def *cli, char *promptchar);
void gw_cli_set_modestring(struct cli_def *cli, char *modestring);
int gw_cli_set_privilege(struct cli_def *cli, int privilege);
int gw_cli_set_configmode(struct cli_def *cli, int mode, char *config_desc);
int gw_cli_set_switch_onuport_mode_enter(struct cli_def *cli, int mode, char *config_desc);
void gw_cli_reprompt(struct cli_def *cli);
void gw_cli_regular(struct cli_def *cli, int (*callback)(struct cli_def *cli));
void gw_cli_regular_interval(struct cli_def *cli, int seconds);
void gw_cli_print(struct cli_def *cli, char *format, ...) __attribute__((format (printf, 2, 3)));
void gw_cli_bufprint(struct cli_def *cli, char *format, ...) __attribute__((format (printf, 2, 3)));
void gw_cli_vabufprint(struct cli_def *cli, char *format, va_list ap);
void gw_cli_error(struct cli_def *cli, char *format, ...) __attribute__((format (printf, 2, 3)));
void gw_cli_print_callback(struct cli_def *cli, void (*callback)(struct cli_def *, char *));
void gw_cli_free_history(struct cli_def *cli);
void gw_cli_set_idle_timeout(struct cli_def *cli, unsigned int seconds);
int gw_cli_arg_help(struct cli_def *cli, int cr_ok, char *entry, ...);

void gw_console_put_char(char c);

extern struct cli_command gw_cli_command[GW_MAX_CLI_COMMAND];


#endif
