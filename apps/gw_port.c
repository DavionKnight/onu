#include "../include/gw_config.h"
#include "../include/gw_types.h"
#include "oam.h"
#include "rcp_gwd.h"
#include "gw_usermac.h"
#include "gwdonuif_interval.h"
#include "gw_port.h"




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




