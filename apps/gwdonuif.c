/*
 * gwdonuif.c
 *
 *  Created on: 2012-11-5
 *      Author: tommy
 */

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "gw_log.h"
#include "gwdonuif_interval.h"

static gwdonu_im_if_t * g_im_ifs = NULL;


gw_status init_im_interfaces()
{
	if(g_im_ifs == NULL)
	{
		g_im_ifs = malloc(sizeof(gwdonu_im_if_t));
		if(g_im_ifs)
		{
			memset(g_im_ifs, 0 ,sizeof(gwdonu_im_if_t));
			return GW_OK;
		}
		else
			return GW_ERROR;
	}

	return GW_OK;
}

gw_status reg_gwdonu_im_interfaces(gwdonu_im_if_t * ifs)
{
	gw_status ret = GW_E_ERROR;
	if(ifs)
	{
		if(g_im_ifs)
		{
			memcpy(g_im_ifs, ifs, sizeof(gwdonu_im_if_t));
			ret = GW_E_OK;
		}
	}

	return ret;
}

gw_status call_gwdonu_if_api(gw_int32 type, gw_int32 argc, ...)
{
	va_list ap;
	gw_status ret = GW_ERROR;


	if(!g_im_ifs)
	{
		gw_log(GW_LOG_LEVEL_DEBUG, ("onu import api ifs not init!\r\n"));
		return GW_ERROR;
	}

	va_start(ap, argc);

	switch (type) {
		case LIB_IF_PORTSEND:
			if(g_im_ifs->portsend)
				ret = (*g_im_ifs->portsend)(va_arg(ap, gw_uint32), va_arg(ap, gw_uint8 *), va_arg(ap, gw_uint32));
			else
				gw_log(GW_LOG_LEVEL_DEBUG,("port send if is null!\r\n"));
			break;
		case LIB_IF_OAM_HDR_BUILDER:
			if(g_im_ifs->oamhdrbuilder)
				ret = (*g_im_ifs->oamhdrbuilder)(va_arg(ap, gw_uint8 *), va_arg(ap, gw_uint32));
			else
				gw_log(GW_LOG_LEVEL_DEBUG,("port send if is null!\r\n"));
			break;
		default:
			break;
	}

	va_end(ap);

	return ret;
}
