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


unsigned char phy_log_map[NUM_UNITS_PER_SYSTEM][PHY_PORT_MAX+1] = {
    /* PHY_PORT_FE0 ~ FE7, MII, EXPAN, SMP */
   // {1, 2, 3, 4, 5, 6, 7, 8, 10, 11, 9, 0xFF} /* uint 0 */
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 0xFF} /* uint 0 */
};

log_phy_map_t log_phy_map[NUM_PORTS_PER_SYSTEM] =
  {
    { 0, (PHY_PORT_FE0 + 0) },
    { 0, (PHY_PORT_FE0 + 1) },
    { 0, (PHY_PORT_FE0 + 2) },
    { 0, (PHY_PORT_FE0 + 3) },
    { 0, (PHY_PORT_FE0 + 4) },
#if 1
    { 0, (PHY_PORT_FE0 + 5) },
    { 0, (PHY_PORT_FE0 + 6) },
    { 0, (PHY_PORT_FE0 + 7) },
    { 0, (PHY_PORT_FE0 + 8) },
    { 0, (PHY_PORT_FE0 + 9) },
    { 0, (PHY_PORT_FE0 + 10) },
    { 0, (PHY_PORT_FE0 + 11) },
    { 0, (PHY_PORT_FE0 + 12) },
    { 0, (PHY_PORT_FE0 + 13) },
    { 0, (PHY_PORT_FE0 + 14) },
    { 0, (PHY_PORT_FE0 + 15) },
    { 0, (PHY_PORT_FE0 + 16) },
    { 0, (PHY_PORT_FE0 + 17) },
    { 0, (PHY_PORT_FE0 + 18) },
    { 0, (PHY_PORT_FE0 + 19) },
    { 0, (PHY_PORT_FE0 + 20) },
    { 0, (PHY_PORT_FE0 + 21) },
    { 0, (PHY_PORT_FE0 + 22) },
    { 0, (PHY_PORT_FE0 + 23) },
    { 0, (PHY_PORT_FE0 + 24) },
    { 0, (PHY_PORT_FE0 + 25) }    
#endif
  };
//extern unsigned char phy_log_map[NUM_UNITS_PER_SYSTEM][PHY_PORT_MAX+1];
//extern log_phy_map_t log_phy_map[NUM_PORTS_PER_SYSTEM];

int onu_bitport_phyport_get(unsigned int egports,unsigned char phyportmember[NUM_PORTS_PER_SYSTEM - 1]);
int boards_physical_to_logical(unsigned long unit, unsigned long pport, unsigned long *lport);
int boards_logical_to_physical(unsigned long lport, unsigned long *unit, unsigned long *pport);

#endif

