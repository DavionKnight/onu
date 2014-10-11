#include "poe_cpld.h"
#include "gw_log.h"
#include "gwdonuif_interval.h"
#include "../cli_lib/cli_common.h"
#include <stdlib.h>
#include "../include/gw_os_api_core.h"

#if(RPU_MODULE_POE == RPU_YES)
extern int cmd_onu_cpld_reg_cfg_set(struct cli_def *cli, char *command, char *argv[], int argc);
int onu_cpld_read_register(unsigned int type,unsigned char* val)
{
    unsigned int reg = 0;
    unsigned int ret = EPON_RETURN_SUCCESS;

    if(val == NULL)
    {
        return EPON_RETURN_FAIL;
    }
    
    switch(type)
    {
        case GWD_CPLD_VERSION_REG:
        case GWD_CPLD_RESERVED_REG:
        case GWD_CPLD_POWER3_REG:
        case GWD_CPLD_RESET_REG:
        case GWD_CPLD_POWER2_REG:
        case GWD_CPLD_POWER_ALARM_REG:
        case GWD_CPLD_POWER1_REG:
        case GWD_CPLD_PROTECT_REG:
            reg = type;
            if(call_gwdonu_if_api(LIB_IF_CPLD_REGISTER_READ,2,reg,val) != EPON_RETURN_SUCCESS)
            {
                gw_log(GW_LOG_LEVEL_MINOR,"read cpld register(0x%02x) fail\n",reg);
            }
            
            break;
           default:
              ret = EPON_RETURN_FAIL;
                break;
    }
    if(ret)
    {
        gw_log(GW_LOG_LEVEL_MINOR,"read cpld register type error\n");
        return EPON_RETURN_FAIL;
    }
    return EPON_RETURN_SUCCESS;
}

int onu_cpld_write_register(unsigned int type,unsigned int val)
{
    unsigned int reg = 0;
    int ret = EPON_RETURN_SUCCESS;

    switch(type)
    {
        case GWD_CPLD_VERSION_REG:
        case GWD_CPLD_RESERVED_REG:
        case GWD_CPLD_POWER3_REG:
        case GWD_CPLD_RESET_REG:
        case GWD_CPLD_POWER2_REG:
        case GWD_CPLD_POWER_ALARM_REG:
        case GWD_CPLD_POWER1_REG:
            reg = type;
            
            if(call_gwdonu_if_api(LIB_IF_CPLD_REGISTER_WRITE,2,GWD_CPLD_PROTECT_REG,cpld_write_enable) != EPON_RETURN_SUCCESS)
            {
                gw_log(GW_LOG_LEVEL_MINOR,"write cpld register(0x07) fail\n");
            }
            if(call_gwdonu_if_api(LIB_IF_CPLD_REGISTER_WRITE,2,reg,val) != EPON_RETURN_SUCCESS)
            {
                gw_log(GW_LOG_LEVEL_MINOR,"write cpld register(0x%02x) fail\n",reg);
            }
            if(call_gwdonu_if_api(LIB_IF_CPLD_REGISTER_WRITE,2,GWD_CPLD_PROTECT_REG,cpld_write_disable) != EPON_RETURN_SUCCESS)
            {
                gw_log(GW_LOG_LEVEL_MINOR,"write cpld register(0x07) fail\n");
            }
            break;
           default:
              ret = EPON_RETURN_FAIL;
                break;
    }
    if(ret)
    {
        gw_log(GW_LOG_LEVEL_MINOR,"write cpld register type error\n");
        return EPON_RETURN_FAIL;
    }
    return EPON_RETURN_SUCCESS;
}


int cmd_onu_cpld_reg_cfg_set(struct cli_def *cli, char *command, char *argv[], int argc)
{
    unsigned int regaddr = 0;
    unsigned char readval = 0;
    unsigned char writeval = 0;

    char 	*pcTmp = NULL;
    
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
        case 1:
            return gw_cli_arg_help(cli, 0,
                "<read/write>", "read cpld register/write cpld register\n",
                 NULL);
        case 2:
            return gw_cli_arg_help(cli,0,
                "{<address>}*1","cpld register {<address>}*1 \n",
                NULL);
        case 3:
            return gw_cli_arg_help(cli,0,
                "{<value>}*1","cpld register {<value>}*1 \n",
                NULL);
        default:
            return gw_cli_arg_help(cli, argc > 2, NULL);
        }
    }
      
    if(argc == 2)
    {
       if(strcmp(argv[0],"read") == 0)
       {    
            regaddr = (unsigned int)strtoul(argv[1], &pcTmp, 0);
            onu_cpld_read_register(regaddr,&readval);
            gw_cli_print(cli,"readregaddr:0x%02x val:0x%02x\n",regaddr,readval);
            
       }
       else
       {
            gw_cli_print(cli,"input command %s not found\n",argv[0]);
       }
    }    
   else if(argc == 3)
    {
        if(strcmp(argv[0],"write") == 0)
       {
            regaddr = (unsigned int)strtoul(argv[1], &pcTmp, 0);
            writeval = (unsigned int)strtoul(argv[2], &pcTmp, 0);        
            onu_cpld_write_register(regaddr,writeval);
            gw_cli_print(cli,"write regaddr:0x%02x val:0x%02x\n",regaddr,readval);
       }
       else
       {
           gw_cli_print(cli,"input command %s not found\n",argv[0]);
       }
    }
   else
   {
        gw_cli_print(cli,"Incomplete command\n");
   }
   
    return CLI_OK;
}

#endif


