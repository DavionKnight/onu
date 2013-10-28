#include "poe_cpld.h"
#include "gw_log.h"
#include "poe_api.h"
#include "gw_port.h"
#include "gwdonuif_interval.h"
#include "../cli_lib/cli_common.h"
#include <stdlib.h>
#include "gw_conf_file.h"

#if(RPU_MODULE_POE == RPU_YES)

unsigned int gulPoeEnabl = 0;
unsigned char gucPoeDisablePerPort[NUM_PORTS_PER_SYSTEM - 1] = {0};
unsigned char gucPoedefaultconfig[NUM_PORTS_PER_SYSTEM - 1] = {0};

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
    unsigned int writeenableaddr = 0x07;
    unsigned int writedisabeaddr = 0x07;
    
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
            
            if(call_gwdonu_if_api(LIB_IF_CPLD_REGISTER_WRITE,2,writeenableaddr,cpld_write_enable) != EPON_RETURN_SUCCESS)
            {
                gw_log(GW_LOG_LEVEL_MINOR,"write cpld register(0x07) fail\n");
            }
            if(call_gwdonu_if_api(LIB_IF_CPLD_REGISTER_WRITE,2,reg,val) != EPON_RETURN_SUCCESS)
            {
                gw_log(GW_LOG_LEVEL_MINOR,"write cpld register(0x%02x) fail\n",reg);
            }
            if(call_gwdonu_if_api(LIB_IF_CPLD_REGISTER_WRITE,2,writedisabeaddr,cpld_write_disable) != EPON_RETURN_SUCCESS)
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

epon_return_code_t onu_poe_exist_stat_set(unsigned int poe_stat)
{
    gulPoeEnabl = poe_stat;
    return EPON_RETURN_SUCCESS;
}

epon_return_code_t onu_poe_exist_stat_get(unsigned int *poe_stat)
{
    *poe_stat = gulPoeEnabl;
    return EPON_RETURN_SUCCESS;
}

epon_return_code_t onu_cpld_exist_get(unsigned int *stat)
{
    unsigned char val = 0;
    int ret = 0;
    ret = EPON_RETURN_EXIST_OK;
    if(onu_cpld_read_register(GWD_CPLD_VERSION_REG,&val) != EPON_RETURN_SUCCESS)
    {
        return EPON_RETURN_FAIL;
    }
    else
    {
        if(val != CPLD_ENABLE)
        {
            ret =EPON_RETURN_EXIST_ERROR;
        }
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

epon_return_code_t onu_poe_cpld_init()
{
    unsigned char val = 0;


    if(onu_cpld_write_register(GWD_CPLD_POWER3_REG,val) != EPON_RETURN_SUCCESS)
    {
        return EPON_RETURN_FAIL;
    }

    if(onu_cpld_write_register(GWD_CPLD_RESET_REG,val) != EPON_RETURN_SUCCESS)
    {
        return EPON_RETURN_FAIL;
    }

    if(onu_cpld_write_register(GWD_CPLD_POWER2_REG,val) != EPON_RETURN_SUCCESS)
    {
        return EPON_RETURN_FAIL;
    }
    
    ALARM_VAL_INIT(val);
    if(onu_cpld_write_register(GWD_CPLD_POWER_ALARM_REG,val) != EPON_RETURN_SUCCESS)
    {
       return EPON_RETURN_FAIL;
    }
    
    val = 0;
    if(onu_cpld_write_register(GWD_CPLD_POWER1_REG,val) != EPON_RETURN_SUCCESS)
    {
       return EPON_RETURN_FAIL;
    }
    return EPON_RETURN_SUCCESS;
}

epon_return_code_t onu_port_power_detect_get(unsigned int port,unsigned int* port_power_stat)
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

epon_return_code_t onu_port_poe_control_state_get(unsigned int port, unsigned int* poe_ctl_state)
{
    if(poe_ctl_state == NULL)
        return EPON_RETURN_FAIL;
    
    if((port >= sizeof(gucPoeDisablePerPort)) || port < 1)
        return EPON_RETURN_FAIL;

    *poe_ctl_state = gucPoeDisablePerPort[port - 1];

    return EPON_RETURN_SUCCESS;
}

epon_return_code_t onu_port_poe_control_state_set(unsigned int port, unsigned int poe_ctl_state)
{    
    if((port >= sizeof(gucPoeDisablePerPort)) || port < 1)
        return EPON_RETURN_FAIL;

   gucPoeDisablePerPort[port - 1] = poe_ctl_state;

    return EPON_RETURN_SUCCESS;
}

epon_return_code_t onu_port_poe_operation_state_set(int lport,int state)
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
    int i = 0;
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
int cmd_onu_cpld_reg_cfg_set(struct cli_def *cli, char *command, char *argv[], int argc)
{
    unsigned int poeexiststat = 0;
    unsigned int poe_stat = 0;

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
   else
   {
       if(!poe_stat)
       {
            gw_cli_print(cli,"POE function is not enable\r\n");
            return CLI_ERROR;
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
    }else if(argc = 3)
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


