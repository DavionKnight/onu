#ifndef __POE_API__
#define __POE_API__

#define POE_PORT_ENABLE 1
#define POE_POWER_DISABLE 0
#define POE_POWER_ENABLE 1
typedef enum {
    EPON_RETURN_SUCCESS = 0,
    EPON_RETURN_FAIL,
    EPON_RETURN_EXIST_OK,
    EPON_RETURN_EXIST_ERROR
} epon_return_code_t;

typedef enum __bool { 
    false = 0,
    true = 1 
 } bool;

extern epon_return_code_t onu_poe_cpld_init();
extern epon_return_code_t onu_cpld_exist_get(unsigned int *stat);
extern epon_return_code_t onu_poe_exist_stat_get(unsigned int *poe_stat);
extern epon_return_code_t onu_poe_exist_stat_set(unsigned int poe_stat);
extern epon_return_code_t onu_port_power_detect_get(unsigned int port,unsigned int* port_power_stat);
extern epon_return_code_t onu_port_poe_control_state_get(unsigned int port, unsigned int* poe_ctl_state);
extern epon_return_code_t onu_port_poe_control_state_set(unsigned int port, unsigned int poe_ctl_state);
extern gw_uint8 gw_onu_read_port_num();

typedef struct
{
    epon_return_code_t (*cpld_init)();
    epon_return_code_t (*poe_cpld_exist_get)(unsigned int *cpld_stat);
    epon_return_code_t (*poe_exist_stat_get)(unsigned int *stat_val);
    epon_return_code_t (*poe_exist_stat_set)(unsigned int stat_val);
    epon_return_code_t (*poe_onu_port_power_detect_get)(unsigned int port,unsigned int *power_stat);
    epon_return_code_t (*poe_control_state_get)(unsigned int port ,unsigned int*control_state);
    epon_return_code_t (*poe_control_state_set)(unsigned int port ,unsigned int control_state);
}poe_cpld_function_set_t;

#endif

