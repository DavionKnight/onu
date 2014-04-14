#include "gw_log.h"
#include "gwdonuif_interval.h"
#include "poe_api.h"
#include "poe_cpld.h"
#include "gw_port.h"
#include "../cli_lib/cli_common.h"
#include "../include/gw_os_api_core.h"
#include "gw_conf_file.h"

#if(RPU_MODULE_POE == RPU_YES)

unsigned int gulPoeEnable = 0;
unsigned char gucPoeDisablePerPort[NUM_PORTS_PER_SYSTEM - 1] = {0};
unsigned char gucPoedefaultconfig[NUM_PORTS_PER_SYSTEM - 1] = {0};

extern int cmd_onu_cpld_reg_cfg_set(struct cli_def *cli, char *command, char *argv[], int argc);
extern int gw_cli_interface_debug_terminal(struct cli_def *cli, char *command, char *argv[], int argc);

epon_return_code_t Gwd_onu_poe_exist_stat_set(unsigned int poe_stat)
{
    gulPoeEnable = poe_stat;
    return EPON_RETURN_SUCCESS;
}

epon_return_code_t Gwd_onu_poe_exist_stat_get(unsigned int *poe_stat)
{
    *poe_stat = gulPoeEnable;
    return EPON_RETURN_SUCCESS;
}

epon_return_code_t Gwd_onu_cpld_exist_get(unsigned int *stat)
{
    unsigned char val = 0;
    int ret = 0;
    ret = EPON_RETURN_EXIST_OK;
    if(onu_cpld_read_register(GWD_CPLD_VERSION_REG,&val) != EPON_RETURN_SUCCESS)
    {
        return EPON_RETURN_FAIL;
    }

    if(val < CPLD_VERSION_1)
    {
        ret =EPON_RETURN_EXIST_ERROR;
    }

    
    if(ret == EPON_RETURN_EXIST_OK)
    {
        *stat = 1;
    }
    else
    {
        *stat = 0;
    }
    return EPON_RETURN_SUCCESS;
}

epon_return_code_t Gwd_onu_poe_cpld_init()
{
    unsigned char val = 0;


    if(onu_cpld_write_register(GWD_CPLD_RESET_REG,val) != EPON_RETURN_SUCCESS)
    {
        return EPON_RETURN_FAIL;
    }

    ALARM_VAL_INIT(val);
    if(onu_cpld_write_register(GWD_CPLD_POWER_ALARM_REG,val) != EPON_RETURN_SUCCESS)
    {
       return EPON_RETURN_FAIL;
    }
    
    return EPON_RETURN_SUCCESS;
}

epon_return_code_t Gwd_onu_port_power_detect_get(unsigned int port,unsigned int* port_power_stat)
{
    unsigned int reg_num = 0;
    unsigned int ret = 0;
    unsigned char power_stat = 0;
    unsigned int uni_port_num = 0;

    uni_port_num = gw_onu_read_port_num();
    
    if(port_power_stat == NULL)
        return EPON_RETURN_FAIL;

    if((port < 1) || port > uni_port_num)
        return EPON_RETURN_FAIL;

    
    PORTECT_REG_NUM(port,reg_num); 
    switch(reg_num)
    {
        case reg1:
                if(onu_cpld_read_register(GWD_CPLD_POWER1_REG,&power_stat) != EPON_RETURN_SUCCESS)
                {
                    ret = EPON_RETURN_FAIL;
                }
                break;
        case reg2:
                if(onu_cpld_read_register(GWD_CPLD_POWER2_REG,&power_stat) != EPON_RETURN_SUCCESS)
                {
                    ret = EPON_RETURN_FAIL;
                }
                break;
        case reg3:
                if(onu_cpld_read_register(GWD_CPLD_POWER3_REG,&power_stat) != EPON_RETURN_SUCCESS)
                {
                    ret = EPON_RETURN_FAIL;
                }
                break;
                default:
                    break;
    }
    PORT_TO_CPLDPORT(port);
    PORT_POWER_STAT(power_stat,port);
    if(power_stat)
        *port_power_stat = 1;
    else
        *port_power_stat = 0;

    return EPON_RETURN_SUCCESS;
}

epon_return_code_t Gwd_onu_port_poe_controle_stat_get(unsigned int port, unsigned int* poe_ctl_state)
{
    if(poe_ctl_state == NULL)
        return EPON_RETURN_FAIL;
    
    if((port >= sizeof(gucPoeDisablePerPort)) || port < 1)
        return EPON_RETURN_FAIL;

    *poe_ctl_state = gucPoeDisablePerPort[port - 1];

    return EPON_RETURN_SUCCESS;
}

epon_return_code_t Gwd_onu_port_poe_controle_stat_set(unsigned int port, unsigned int poe_ctl_state)
{    
    if((port >= sizeof(gucPoeDisablePerPort)) || port < 1)
        return EPON_RETURN_FAIL;

   gucPoeDisablePerPort[port - 1] = poe_ctl_state;

    return EPON_RETURN_SUCCESS;
}

epon_return_code_t Gwd_onu_port_poe_operation_stat_set(int lport,int state)
{
    unsigned long unit = 0;
    unsigned long pport = 0;
    
    boards_logical_to_physical(lport,&unit, &pport);
    
    if(call_gwdonu_if_api(LIB_IF_POE_PORT_OPERATION_SET,3,pport,state) != EPON_RETURN_SUCCESS)
    {
        gw_log(GW_LOG_LEVEL_MINOR,"set port poe operation fail\n");
        return EPON_RETURN_FAIL;
    }

    return EPON_RETURN_SUCCESS;
}
gw_int32 gw_poe_config_showrun(gw_int32* len,gw_uint8**pv)
{
   	gw_int32 ret = GW_ERROR;
	if(len && pv)
	{
		gw_uint8 * p = NULL;
		*len = sizeof(gucPoeDisablePerPort);

       
		p = malloc(*len);

		if(p)
		{
            if(memcmp(gucPoeDisablePerPort,gucPoedefaultconfig,*len) != 0)
            {
                 memcpy(p,gucPoeDisablePerPort,*len);
    		    *pv = (gw_uint8*)p;
    			ret = GW_OK;
            }
		}
	}

	return ret; 
}

gw_int32 gw_poe_config_restore(gw_int32 len, gw_uint8 * pv)
{
    memcpy(gucPoeDisablePerPort,pv,len);
#if 0
    gw_log(GW_LOG_LEVEL_MINOR,"\r\n-----------------------------------------------------------------\r\n");
    gw_log(GW_LOG_LEVEL_MINOR,"len %d [1]\n",len);
    for(i = 0;i < len;i++)
    {
        gw_log(GW_LOG_LEVEL_MINOR,"%d ",gucPoeDisablePerPort[i]);
    }
    gw_log(GW_LOG_LEVEL_MINOR,"\r\n-----------------------------------------------------------------\r\n");
    gw_log(GW_LOG_LEVEL_MINOR,"\r\n");
#endif
	return GW_OK;
}

int gw_poe_config_init()
{
    memset(gucPoeDisablePerPort,1,(NUM_PORTS_PER_SYSTEM-1));
    memset(gucPoedefaultconfig,1,(NUM_PORTS_PER_SYSTEM-1));

    gw_register_conf_handlers(GW_CONF_TYPE_POE_CONFIG, gw_poe_config_showrun, gw_poe_config_restore);

    return GW_OK;
}
void gwd_onu_poe_cpld_check()
{
    unsigned int cpld_stat = 0;
    unsigned int ret = 0;
    unsigned int i = 0;


    if(gw_onu_poe_api_register_check() != EPON_RETURN_SUCCESS)
    {
        Gwd_onu_poe_exist_stat_set(cpld_stat);
        return;
    }
    for(i = 0; i < 3; i++)
    {
        if(Gwd_onu_cpld_exist_get(&cpld_stat) == EPON_RETURN_SUCCESS)
        { 
            gw_log(GW_LOG_LEVEL_MINOR,"cheak cpld ok\n");
            if(cpld_stat)
            {
                Gwd_onu_poe_exist_stat_set(cpld_stat);
            }
            ret = EPON_RETURN_SUCCESS;
            return;        
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
        
    Gwd_onu_poe_exist_stat_get(&poeexiststat);
    if(poeexiststat)
    {        
        Gwd_onu_poe_cpld_init();
    }
    else
    {
        gw_log(GW_LOG_LEVEL_MINOR,"poe thread exit\n");
        return ;
    }

    uni_port_num = gw_onu_read_port_num();
        
    while(TRUE)
    {
        for(ulport = 1;ulport <= uni_port_num; ulport++)
        {
            if(call_gwdonu_if_api(LIB_IF_PORT_ADMIN_GET, 2, ulport, &port_stat) != EPON_RETURN_SUCCESS)
            {
                continue;
            }

            if(port_stat == PORT_ADMIN_UP)
            {
                if(Gwd_onu_port_poe_controle_stat_get(ulport,&ctl_state) != EPON_RETURN_SUCCESS)
                {
                    gw_log(GW_LOG_LEVEL_MINOR,"get port:%d  admin fail:\n",ulport);
                    continue;
                }
                else
                {
                    if(ctl_state == POE_PORT_ENABLE)
                    {
                        if(Gwd_onu_port_power_detect_get(ulport,&Pstate) != EPON_RETURN_SUCCESS)
                        {
                            gw_log(GW_LOG_LEVEL_MINOR,"get port:%d  power fail:\n",ulport);
                            continue;
                        }

                        if(Pstate == POE_POWER_DISABLE)
                        {
                            Gwd_onu_port_poe_operation_stat_set(ulport,PORT_ADMIN_DOWN);
                        }
                    }
                }
                                    
            }
            else
            {
                if(Gwd_onu_port_poe_controle_stat_get(ulport,&ctl_state) != EPON_RETURN_SUCCESS)
                {
                    continue;
                }
                else
                {
                    if(ctl_state == POE_PORT_ENABLE)
                    {
                        if(Gwd_onu_port_power_detect_get(ulport,&Pstate) != EPON_RETURN_SUCCESS)
                        {
                            gw_log(GW_LOG_LEVEL_MINOR,"get port:%d power fail\n",ulport);
                            continue;
                        }
                        if(Pstate == POE_POWER_ENABLE)
                        {
                             Gwd_onu_port_poe_operation_stat_set(ulport,PORT_ADMIN_UP);
                        }
                    }
                    else
                    {
                        Gwd_onu_port_poe_operation_stat_set(ulport,PORT_ADMIN_UP);
                    }
                    
                }
            }
        }

        gw_thread_delay(100);
    }
}


int cmd_onu_poe_cfg_set(struct cli_def *cli, char *command, char *argv[], int argc)
{
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

       for(lport = 1; lport <= uni_port_num; lport++)
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
            BEGIN_PARSE_PORT_LIST_TO_PORT_NO_CHECK(argv[1],lport,uni_port_num)
            {
                if(Gwd_onu_port_poe_controle_stat_set(lport,poe_ctl_val) != EPON_RETURN_SUCCESS)
                {
                    gw_cli_print(cli,"poe set port(%d) control enable fial\r\n",lport);
                    continue;
                }
            }
            END_PARSE_PORT_LIST_TO_PORT_NO_CHECK();
        }
            
        if(strcmp(argv[0],"disable") == 0)
        {
            poe_ctl_val = 0;
            BEGIN_PARSE_PORT_LIST_TO_PORT_NO_CHECK(argv[1],lport,uni_port_num)
            {
                if(Gwd_onu_port_poe_controle_stat_set(lport,poe_ctl_val) != EPON_RETURN_SUCCESS)
                {
                    gw_cli_print(cli,"poe set port(%d) control disable fail\r\n",lport);
                    continue;
                }
            }
            END_PARSE_PORT_LIST_TO_PORT_NO_CHECK();
        } 


    }
    else
    {
            for(lport = 1; lport <= uni_port_num; lport++)
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

void cli_reg_gwd_poe_cmd(struct cli_command **cmd_root)
{
    struct cli_command *reg;

    gw_cli_register_command(cmd_root, NULL, "poe", cmd_onu_poe_cfg_set, PRIVILEGE_UNPRIVILEGED, MODE_CONFIG, "onu poe control");
   
    reg = gw_cli_register_command(cmd_root, NULL, "cpld",NULL, PRIVILEGE_PRIVILEGED,MODE_DEBUG, "read/write onu cpld register");
          gw_cli_register_command(cmd_root, reg, "register",cmd_onu_cpld_reg_cfg_set, PRIVILEGE_PRIVILEGED, MODE_DEBUG, "read/write onu cpld register");

    return;
}


#endif


