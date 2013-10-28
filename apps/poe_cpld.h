#ifndef __POE_CPLD__
#define __POE_CPLD__

#define CPLD_ENABLE 1


#define cpld_write_enable 0x55
#define cpld_write_disable 0
#define ALARM_VAL_INIT(val) val |= 1 << 7
#define PORTECT_REG_NUM(port,reg_num) reg_num = (port - 1)/8 
#define PORT_TO_CPLDPORT(port)  port = ((port-1)%8)
#define PORT_POWER_STAT(power_stat,port) power_stat &=(1 << port)

typedef enum{
    GWD_CPLD_VERSION_REG,
    GWD_CPLD_RESERVED_REG,
    GWD_CPLD_POWER3_REG,
    GWD_CPLD_RESET_REG,
    GWD_CPLD_POWER2_REG,
    GWD_CPLD_POWER_ALARM_REG,
    GWD_CPLD_POWER1_REG,
    GWD_CPLD_PROTECT_REG
}cpld_register_t;

typedef enum{
    reg1,
    reg2,
    reg3
}cpld_power_reg_t;

#endif

