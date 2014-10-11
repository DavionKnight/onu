#ifndef __POE_API__
#define __POE_API__

#define POE_PORT_ENABLE 1
#define POE_POWER_DISABLE 0
#define POE_POWER_ENABLE 1


#define CPLD_VERSION_1 1


extern int onu_cpld_read_register(unsigned int type,unsigned char* val);
extern int onu_cpld_write_register(unsigned int type,unsigned int val);

extern gw_int32 gw_onu_poe_api_register_check();

extern gw_uint8 gw_onu_read_port_num();


typedef enum __bool { 
    false = 0,
    true = 1 
 } bool;

#endif

