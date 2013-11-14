#include "../include/gw_config.h"
#include "../include/gw_types.h"
#include "oam.h"
#include "rcp_gwd.h"
#include "gw_usermac.h"
#include "gwdonuif_interval.h"
#include "gw_port.h"


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

int onu_bitport_phyport_get(unsigned int egports,unsigned char phyportmember[PHY_PORT_MAX])
{
	int numOfUniPorts = 0,phyport=0;
	int phynum = 0;
	
	if(phyportmember == NULL)
	{
		return GW_ERROR;
	}
	
	for ( phyport = 0; phyport < PHY_PORT_MAX; phyport++)
	{
		if (egports & ONU_BITPORT_TO_PHYPORT(phyport))/*转换为物理地址*/
		{
			phyportmember[phyport] = PHY_OK;
			phynum++;
		}
		else
		{
			phyportmember[phyport] = PHY_ERROR;
		}
	}
	if(0 == phynum)
	{
		return GW_ERROR;
	}
	return GW_OK;
}

int boards_logical_to_physical(unsigned long lport, unsigned long *unit, unsigned long *pport)
{
    int lport_0 = (lport-1);

    /* check whether logical port number is legal or not */
    if ((lport_0 < 0) || (lport_0 >= NUM_PORTS_PER_SYSTEM)) {
        return 0;
    }
    *unit = log_phy_map[lport_0].unit;
    *pport = log_phy_map[lport_0].physical_port;

    return 1;
}

int boards_physical_to_logical(unsigned long unit, unsigned long pport, unsigned long *lport)
{
    /* check whether logical port number is legal or not */
    if ((unit >= NUM_UNITS_PER_SYSTEM) || (pport > PHY_PORT_MAX)) {
        return 0;
    }

    *lport = phy_log_map[unit][pport];
    if ( *lport == 0xFF ) {
        return 0;
    }

    return 1;
}




