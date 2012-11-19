/*
 * gw_log.c
 *
 *  Created on: 2012-11-2
 *      Author: tommy
 */

#include "../include/gw_os_api_core.h"
#include "gw_log.h"

static gw_int32 log_level = GW_LOG_LEVEL_DEBUG;

gw_int32 setGwLogLevel(gw_int32 lv)
{
	log_level = lv;
	return log_level;
}

gw_int32 getGwlogLevel()
{
	return log_level;
}
