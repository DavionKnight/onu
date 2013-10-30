#ifndef __GW_PORT__
#define __GW_PORT__

#define ONU_BITPORT_TO_PHYPORT(str) (0x1 << str)

#define NUM_PORTS_PER_SYSTEM 26
#define NUM_PORTS_MINIMUM_SYSYTEM 1
#define NUM_UNITS_PER_SYSTEM    5


#define PHY_PORT_MAX 	26		
#define PHY_PORT_FE0 0

typedef struct log_phy_map_s {
    unsigned char unit;
    unsigned char physical_port;
} log_phy_map_t;


#define BEGIN_PARSE_PORT_LIST_TO_PORT_NO_CHECK(portlist, ifindex,devonuport_num) \
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

#define END_PARSE_PORT_LIST_TO_PORT_NO_CHECK() \
        }\
        free(_pulIfArray);\
    }\
}

extern unsigned char phy_log_map[NUM_UNITS_PER_SYSTEM][PHY_PORT_MAX+1];
extern log_phy_map_t log_phy_map[NUM_PORTS_PER_SYSTEM];

int onu_bitport_phyport_get(unsigned int egports,unsigned char phyportmember[NUM_PORTS_PER_SYSTEM - 1]);
int boards_physical_to_logical(unsigned long unit, unsigned long pport, unsigned long *lport);
int boards_logical_to_physical(unsigned long lport, unsigned long *unit, unsigned long *pport);

#endif

