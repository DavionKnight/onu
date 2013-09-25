#include "../include/gw_os_api_core.h"
#include "../include/gw_types.h"
#include "oam.h"
#include "rcp_gwd.h"
#include "gw_usermac.h"
#include "gwdonuif_interval.h"
#include "gw_aux.h"
#include "gw_log.h"

int onu_mulimac_cheak(unsigned char *fdbmac)
{
#if 0
	GWD_ONU_IF_MULIMAC_U mulimac;
	mulimac.mulimac_byte = fdbmac[0];
	gw_printf("mulimac.mulimac_byte:%02x\n",mulimac.mulimac_byte);
	if(1 == mulimac.if_mulimac_t.multag)
	{
		return GW_OK;
	}
#else
    unsigned char mac = 0;
	if(fdbmac == NULL)
	{
		func_pointer_error_syslog("function NULL pointer error (%s %d)\n",__func__,__LINE__);
		return GW_OK;
	}
    mac = fdbmac[0];
	if(1 == ((0x01 << 7) & mac))
	{
		return GW_OK;
	}
#endif
	return GW_ERROR;
}

