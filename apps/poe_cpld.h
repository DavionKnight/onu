#ifndef __POE_CPLD__
#define __POE_CPLD__

#define cpld_write_enable 0x55
#define cpld_write_disable 0
#define ALARM_VAL_INIT(val) val |= 1 << 7
#define PORTECT_REG_NUM(port,reg_num) reg_num = (port - 1)/8 
#define PORT_TO_CPLDPORT(port)  port = ((port-1)%8)
#define PORT_POWER_STAT(power_stat,port) power_stat &=(1 << port)

typedef enum {
    EPON_RETURN_SUCCESS = 0,
    EPON_RETURN_FAIL,
    EPON_RETURN_EXIST_OK,
    EPON_RETURN_EXIST_ERROR
} epon_return_code_t;

typedef enum{
    GWD_CPLD_VERSION_REG = 0x00,
    GWD_CPLD_RESERVED_REG = 0x01,
    GWD_CPLD_POWER3_REG = 0x02,
    GWD_CPLD_RESET_REG = 0x03,
    GWD_CPLD_POWER2_REG = 0x04,
    GWD_CPLD_POWER_ALARM_REG = 0x05,
    GWD_CPLD_POWER1_REG = 0x06,
    GWD_CPLD_PROTECT_REG = 0x07
}cpld_register_t;

typedef enum{
    reg1,
    reg2,
    reg3
}cpld_power_reg_t;

extern epon_return_code_t Gwd_onu_cpld_exist_get(unsigned int *stat);
extern epon_return_code_t Gwd_onu_poe_exist_stat_get(unsigned int *poe_stat);

#endif

