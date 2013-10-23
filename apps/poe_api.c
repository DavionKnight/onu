#include "gw_log.h"
#include "gwdonuif_interval.h"
#include "poe_api.h"
#include "poe_cpld.h"
#include "../cli_lib/cli_common.h"

#if(RPU_MODULE_POE == RPU_YES)
poe_cpld_function_set_t poe_func_set ={
    onu_poe_cpld_init,
    onu_cpld_exist_get,
    onu_poe_exist_stat_get,
    onu_poe_exist_stat_set,
    onu_port_power_detect_get,
    onu_port_poe_control_state_get,
    onu_port_poe_control_state_set
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
    gw_log(GW_LOG_LEVEL_MINOR,"intput gwd_onu_poe_cpld_cheak\n");
   while(true)
    {
        gw_log(GW_LOG_LEVEL_MINOR,"intput while\n");
        if(Gwd_onu_cpld_exist_get(&cpld_stat) != EPON_RETURN_SUCCESS)
        {
            gw_log(GW_LOG_LEVEL_MINOR,"get cpld exist fial\n");
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

        gw_thread_delay(10000);
    }
}


int cmd_onu_poe_control_set(struct cli_def *cli, char *command, char *argv[], int argc)
{
    unsigned int poeexiststat = 0;
    unsigned int lport = 0;
    unsigned int poe_ctl_val = 0;
    
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
        case 1:
            return gw_cli_arg_help(cli, 0,
                "<enable/disable>", "Manufacture serial number(length<16)",
                 NULL);
        case 2:
            return gw_cli_arg_help(cli,0,
                "<port_list>","port list 1-3",
                NULL);
        default:
            return gw_cli_arg_help(cli, argc > 1, NULL);
        }
    }
    
    Gwd_onu_poe_exist_stat_get(&poeexiststat);

    if(!poeexiststat)
    {
        return CLI_ERROR;
    }
    
    if(argc == 1)
    {
       if(strcmp(argv[0],"enable") == 0)
       {
            
       }

       if(strcmp(argv[0],"disable") == 0)
       {

       }
       
    }else if(argc == 2)
    {
        if(strcmp(argv[0],"enable") == 0)
        {
            lport = atoi(argv[1]);
            poe_ctl_val = 1;
            Gwd_onu_port_poe_controle_stat_set(lport,poe_ctl_val);
        }
            
        if(strcmp(argv[0],"disable") == 0)
        {
            lport = atoi(argv[1]);
            poe_ctl_val = 0;
            Gwd_onu_port_poe_controle_stat_set(lport,poe_ctl_val);
        } 
    }
    else
    {

    }
    
    return CLI_OK;
}

void cli_reg_gwd_poe_cmd(struct cli_command **cmd_root)
{
    struct cli_command *set;

    set = gw_cli_register_command(cmd_root, NULL, "poe", NULL, PRIVILEGE_UNPRIVILEGED, MODE_CONFIG, "onu poe control");
          gw_cli_register_command(cmd_root, set, "set",cmd_onu_poe_control_set, PRIVILEGE_UNPRIVILEGED, MODE_CONFIG, "onu poe control");

    return;
}
#endif


