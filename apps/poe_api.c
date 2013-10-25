#include "gw_log.h"
#include "gwdonuif_interval.h"
#include "poe_api.h"
#include "poe_cpld.h"
#include "../cli_lib/cli_common.h"



#if(RPU_MODULE_POE == RPU_YES)

extern int cmd_onu_cpld_reg_cfg_set(struct cli_def *cli, char *command, char *argv[], int argc);

#define BEGIN_PARSE_PORT_LIST_TO_PORT_NO_CHECK_FOR_POE(portlist, ifindex,devonuport_num) \
{\
    gw_uint32 * _pulIfArray;\
    gw_uint32 _i = 0;\
    _pulIfArray = (gw_uint32*)ETH_ParsePortList(portlist,devonuport_num);\
    if(!_pulIfArray)\
    	{\
    		ifindex = 0;\
    	}\
    if(_pulIfArray != NULL)\
    {\
        for(_i=0;_pulIfArray[_i]!=0;_i++)\
        {\
            ifindex = _pulIfArray[_i];\

#define END_PARSE_PORT_LIST_TO_PORT_NO_CHECK_FOR_POE() \
        }\
        free(_pulIfArray);\
    }\
}

poe_cpld_function_set_t poe_func_set ={
    onu_poe_cpld_init,
    onu_cpld_exist_get,
    onu_poe_exist_stat_get,
    onu_poe_exist_stat_set,
    onu_port_power_detect_get,
    onu_port_poe_control_state_get,
    onu_port_poe_control_state_set,
};


epon_return_code_t Gwd_onu_poe_cpld_init()
{
    if(poe_func_set.cpld_init)
    {
        return poe_func_set.cpld_init();
    }
    else
    {
        return EPON_RETURN_FAIL;
    }
}

epon_return_code_t Gwd_onu_cpld_exist_get(unsigned int *stat)
{
    if(poe_func_set.poe_cpld_exist_get)
    {
        return poe_func_set.poe_cpld_exist_get(stat);
    }
    else
    {
        return EPON_RETURN_FAIL;
    }
}

epon_return_code_t Gwd_onu_poe_exist_stat_set(unsigned int stat_val)
{
    if(poe_func_set.poe_exist_stat_set)
        return poe_func_set.poe_exist_stat_set(stat_val);
    else
        return EPON_RETURN_FAIL;
}

epon_return_code_t Gwd_onu_poe_exist_stat_get(unsigned int* stat_val)
{
    if(poe_func_set.poe_exist_stat_get)
        return poe_func_set.poe_exist_stat_get(stat_val);
    else
        return EPON_RETURN_FAIL;
}

epon_return_code_t Gwd_onu_port_power_detect_get(unsigned int port,unsigned int* power_stat)
{
    if(poe_func_set.poe_onu_port_power_detect_get)
        return poe_func_set.poe_onu_port_power_detect_get(port,power_stat);
    else
        return EPON_RETURN_FAIL;
}

epon_return_code_t Gwd_onu_port_poe_controle_stat_get(unsigned int port,unsigned int*ctl_state)
{
    if(poe_func_set.poe_control_state_get)
        return poe_func_set.poe_control_state_get(port,ctl_state);
    else
        return EPON_RETURN_FAIL;
}

epon_return_code_t Gwd_onu_port_poe_controle_stat_set(unsigned int port,unsigned int ctl_state)
{
    if(poe_func_set.poe_control_state_set)
        return poe_func_set.poe_control_state_set(port,ctl_state);
    else
        return EPON_RETURN_FAIL;
}

void gwd_onu_poe_cpld_cheak()
{
    unsigned int cpld_stat = 0;
    unsigned int ret = 0;
    unsigned int poe_cpld_enable_error_count = 0;
   while(true)
    {
        if(Gwd_onu_cpld_exist_get(&cpld_stat) != EPON_RETURN_SUCCESS)
        {
            poe_cpld_enable_error_count++;

            if(poe_cpld_enable_error_count == 3)
            {
                break;
                ret = EPON_RETURN_FAIL;
            }
        }
        else
        {
            gw_log(GW_LOG_LEVEL_MINOR,"cheak cpld ok\n");
            if(cpld_stat)
            {
                Gwd_onu_poe_exist_stat_set(cpld_stat);
            }
            
            ret = EPON_RETURN_SUCCESS;
            break;
                
        }
    }
}



void gwd_onu_poe_thread_hander()
{
    unsigned int poeexiststat =0;
    unsigned int uni_port_num = 0;
    unsigned int ulport =0;
    unsigned int port_stat = 0;
    unsigned int Pstate = 0;
    unsigned int ctl_state = 0;
    
    gw_log(GW_LOG_LEVEL_MINOR,"intput gwd_onu_poe_thread_hander\n");
    
    Gwd_onu_poe_exist_stat_get(&poeexiststat);
    if(poeexiststat)
    {
        gw_log(GW_LOG_LEVEL_MINOR,"poe exist\n");
        
        Gwd_onu_poe_cpld_init();

        
        gw_log(GW_LOG_LEVEL_MINOR,"init cpld ok\n");
    }
    else
    {
        gw_log(GW_LOG_LEVEL_MINOR,"poe thread exit\n");
        return ;
    }

    uni_port_num = gw_onu_read_port_num();
    
    gw_log(GW_LOG_LEVEL_MINOR,"uni_port_num:%d\n",uni_port_num);
    
    while(true)
    {
        for(ulport = 1;ulport < uni_port_num; ulport++)
        {
            if(call_gwdonu_if_api(LIB_IF_PORT_ADMIN_GET, 2, ulport, &port_stat) != EPON_RETURN_SUCCESS)
            {
                continue;
            }

            if(port_stat == PORT_ADMIN_UP)
            {
                gw_log(GW_LOG_LEVEL_MINOR,"port:%d  admin up:\n",ulport);
                if(Gwd_onu_port_poe_controle_stat_get(ulport,&ctl_state) != EPON_RETURN_SUCCESS)
                {
                    gw_log(GW_LOG_LEVEL_MINOR,"get port:%d  admin fail:\n",ulport);
                    continue;
                }
                else
                {
                    gw_log(GW_LOG_LEVEL_MINOR,"get port:%d  admin up success:\n",ulport);
                    if(ctl_state == POE_PORT_ENABLE)
                    {
                        gw_log(GW_LOG_LEVEL_MINOR,"port:%d ctl enable\n",ulport);
                        if(Gwd_onu_port_power_detect_get(ulport,&Pstate) != EPON_RETURN_SUCCESS)
                        {
                            gw_log(GW_LOG_LEVEL_MINOR,"get port:%d  power fail:\n",ulport);
                            continue;
                        }

                        if(Pstate == POE_POWER_DISABLE)
                        {
                            gw_log(GW_LOG_LEVEL_MINOR,"port:%d power not found\n",ulport);
                            call_gwdonu_if_api(LIB_IF_PORT_ADMIN_SET, 2, ulport,PORT_ADMIN_DOWN);
                        }
                    }
                }
                                    
            }
            else
            {
                gw_log(GW_LOG_LEVEL_MINOR,"port:%d admin down\n",ulport);
                if(Gwd_onu_port_poe_controle_stat_get(ulport,&ctl_state) != EPON_RETURN_SUCCESS)
                {
                    continue;
                }
                else
                {
                    if(ctl_state == POE_PORT_ENABLE)
                    {
                        gw_log(GW_LOG_LEVEL_MINOR,"port:%d ctl enable\n",ulport);
                        if(Gwd_onu_port_power_detect_get(ulport,&Pstate) != EPON_RETURN_SUCCESS)
                        {
                            continue;
                        }

                        if(Pstate == POE_POWER_ENABLE)
                        {
                            gw_log(GW_LOG_LEVEL_MINOR,"port:%d power found\n",ulport);
                            call_gwdonu_if_api(LIB_IF_PORT_ADMIN_SET, 2, ulport,PORT_ADMIN_UP);
                        }
                    }
                    else
                    {
                        gw_log(GW_LOG_LEVEL_MINOR,"port:%d ctl disable\n",ulport);
                        call_gwdonu_if_api(LIB_IF_PORT_ADMIN_SET, 2, ulport,PORT_ADMIN_UP);
                    }
                    
                }
            }
        }

        gw_thread_delay(50000);
    }
}


int cmd_onu_poe_cfg_set(struct cli_def *cli, char *command, char *argv[], int argc)
{
    unsigned int poeexiststat = 0;
    unsigned int lport = 0;
    unsigned int poe_ctl_val = 0;
    unsigned int uni_port_num = 0;
    unsigned int poe_stat = 0;
    
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
        case 1:
            return gw_cli_arg_help(cli, 0,
                "<enable/disable>", "enable poe,disable poe\n",
                 NULL);
        case 2:
            return gw_cli_arg_help(cli,0,
                "<port_list>","Specify interface's port list(e.g.: 1; 1,2; 1-8)\n",
                NULL);
        default:
            return gw_cli_arg_help(cli, argc > 1, NULL);
        }
    }
    
    Gwd_onu_poe_exist_stat_get(&poeexiststat);

    if(!poeexiststat)
    {
        gw_cli_print(cli,"This dev does not support poe function\r\n");
        return CLI_ERROR;
    }

    if(Gwd_onu_poe_exist_stat_get(&poe_stat) != EPON_RETURN_SUCCESS)
    {
         return CLI_ERROR;
    }
    if(!poe_stat)
    {
         gw_cli_print(cli,"POE function is not enable\r\n");
         return CLI_ERROR;
    }
   
    uni_port_num = gw_onu_read_port_num();
    
    if(argc == 1)
    {

       if(strcmp(argv[0],"enable") == 0)
       {
            poe_ctl_val = 1;
       }

       if(strcmp(argv[0],"disable") == 0)
       {
            poe_ctl_val = 0;
       }

       for(lport = 1; lport < uni_port_num; lport++)
       {
            if(Gwd_onu_port_poe_controle_stat_set(lport,poe_ctl_val) != EPON_RETURN_SUCCESS)
            {
                gw_cli_print(cli,"poe set port %d control enable fail\r\n",lport);
                return CLI_ERROR;
            }
       }
  
    }else if(argc == 2)
    {
        if(strcmp(argv[0],"enable") == 0)
        {
            poe_ctl_val = 1;
            BEGIN_PARSE_PORT_LIST_TO_PORT_NO_CHECK_FOR_POE(argv[1],lport,uni_port_num)
            {
                if(Gwd_onu_port_poe_controle_stat_set(lport,poe_ctl_val) != EPON_RETURN_SUCCESS)
                {
                    gw_cli_print(cli,"poe set port(%d) control enable fial\r\n",lport);
                    continue;
                }
            }
            END_PARSE_PORT_LIST_TO_PORT_NO_CHECK_FOR_POE();
        }
            
        if(strcmp(argv[0],"disable") == 0)
        {
            poe_ctl_val = 0;
            BEGIN_PARSE_PORT_LIST_TO_PORT_NO_CHECK_FOR_POE(argv[1],lport,uni_port_num)
            {
                if(Gwd_onu_port_poe_controle_stat_set(lport,poe_ctl_val) != EPON_RETURN_SUCCESS)
                {
                    gw_cli_print(cli,"poe set port(%d) control disable fail\r\n",lport);
                    continue;
                }
            }
            END_PARSE_PORT_LIST_TO_PORT_NO_CHECK_FOR_POE();
        } 


    }
    else
    {
            for(lport = 1; lport < uni_port_num; lport++)
            {
                if(Gwd_onu_port_poe_controle_stat_get(lport,&poe_ctl_val) != EPON_RETURN_SUCCESS)
                {
                    gw_cli_print(cli,"UNI Port %d : get control stat fail\r\n",lport);
                    continue;
                }
                else
                {
                    gw_cli_print(cli,"UNI Port %d : %s\r\n",lport,poe_ctl_val?"POE CONTROL ENABLE":"POE CONTROL DISABLE");
                }
            }
    }
    
    return CLI_OK;
}

int gw_cli_int_debug_terminal(struct cli_def *cli, char *command, char *argv[], int argc)
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

void cli_reg_gwd_poe_cmd(struct cli_command **cmd_root)
{
    struct cli_command *reg;

    gw_cli_register_command(cmd_root, NULL, "poe", cmd_onu_poe_cfg_set, PRIVILEGE_UNPRIVILEGED, MODE_CONFIG, "onu poe control");

    gw_cli_register_command(cmd_root, NULL, "advdebug",gw_cli_int_debug_terminal, PRIVILEGE_PRIVILEGED,   MODE_CONFIG,    "Enter debug mode");
    reg = gw_cli_register_command(cmd_root, NULL, "cpld",NULL, PRIVILEGE_PRIVILEGED,MODE_DEBUG, "read/write onu cpld register");
          gw_cli_register_command(cmd_root, reg, "register",cmd_onu_cpld_reg_cfg_set, PRIVILEGE_PRIVILEGED, MODE_DEBUG, "read/write onu cpld register");

    return;
}


#endif


