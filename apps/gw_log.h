/*
 * gw_log.h
 *
 *  Created on: 2012-11-2
 *      Author: tommy
 */

#ifndef GW_LOG_H_
#define GW_LOG_H_

#include "../include/gw_os_api_core.h"

enum{
	GW_LOG_LEVEL_DEBUG,
	GW_LOG_LEVEL_INFO,
	GW_LOG_LEVEL_MINOR,
	GW_LOG_LEVEL_MAJOR,
	GW_LOG_LEVEL_CRI,
};

gw_int32 getGwlogLevel();
gw_int32 setGwLogLevel(gw_int32 lv);

#define gw_log(lv, log...) if(lv >= getGwlogLevel()) gw_printf (log)

#endif /* GW_LOG_H_ */
