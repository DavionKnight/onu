#include "poe_cpld.h"
#include "gw_log.h"
#include "poe_api.h"
#include "gw_port.h"
#include "gwdonuif_interval.h"

#if(RPU_MODULE_POE == RPU_YES)

unsigned int gulPoeEnabl = 0;
unsigned char gucPoeDisablePerPort[NUM_PORTS_PER_SYSTEM - 1] = {1};



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

int onu_cpld_write_register(unsigned int type,unsigned char val)
{
    unsigned int reg = 0;
    int ret = EPON_RETURN_SUCCESS;
    unsigned int w_val = 0;
    w_val = (unsigned int)val;
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
            gw_log(GW_LOG_LEVEL_MINOR,"cpld write register:0x%02x val:0x%02x\n",reg,w_val);
            if(call_gwdonu_if_api(LIB_IF_CPLD_REGISTER_WRITE,2,ret,w_val) != EPON_RETURN_SUCCESS)
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
    
    gw_log(GW_LOG_LEVEL_MINOR,"intput onu_cpld_exist_get\n");
    if(onu_cpld_read_register(GWD_CPLD_VERSION_REG,&val) != EPON_RETURN_SUCCESS)
    {
        return EPON_RETURN_FAIL;
    }
    else
    {
        gw_log(GW_LOG_LEVEL_MINOR,"read cpld version:%d\n",val);
        if(val != CPLD_ENABLE)
        {
            ret =EPON_RETURN_EXIST_ERROR;
        }
    }

    gw_log(GW_LOG_LEVEL_MINOR,"write enable\n");   
    
    if(onu_cpld_write_register(GWD_CPLD_PROTECT_REG,0x55) != EPON_RETURN_SUCCESS)
    {
        return EPON_RETURN_FAIL;
    }
    
    if(onu_cpld_read_register(GWD_CPLD_PROTECT_REG,&val) != EPON_RETURN_SUCCESS)
    {
        return EPON_RETURN_FAIL;
    }
    else
    {
        if(val != cpld_write_enable)
        {
            ret =EPON_RETURN_EXIST_ERROR;
        }
    }

    if(onu_cpld_write_register(GWD_CPLD_PROTECT_REG,cpld_write_disable) != EPON_RETURN_SUCCESS)
    {
        return EPON_RETURN_FAIL;
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

    if(onu_cpld_write_register( GWD_CPLD_PROTECT_REG,cpld_write_enable) != EPON_RETURN_SUCCESS)
    {
        return EPON_RETURN_FAIL;
    }
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
    if(onu_cpld_write_register(GWD_CPLD_POWER_ALARM_REG,val) != EPON_RETURN_SUCCESS)
    {
       return EPON_RETURN_FAIL;
    }

    if(onu_cpld_write_register( GWD_CPLD_PROTECT_REG,cpld_write_disable) != EPON_RETURN_SUCCESS)
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
    PORT_POWER_STAT(power_stat,port);
    
    if(power_stat)
        *port_power_stat = power_stat;
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



#endif


