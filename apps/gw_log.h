/*
 * gw_log.h
 *
 *  Created on: 2012-11-2
 *      Author: tommy
 */

#ifndef GW_LOG_H_
#define GW_LOG_H_

#include "../include/gw_os_api_core.h"

#define GW_MAX_EVENT_LOG_NUM	1024	 //maximum of record event msg
#define GW_EVENT_LOG_LENGTH		128	//length of record msg

enum{
	GW_LOG_LEVEL_DEBUG,
	GW_LOG_LEVEL_INFO,
	GW_LOG_LEVEL_MINOR,
	GW_LOG_LEVEL_MAJOR,
	GW_LOG_LEVEL_CRI,
};

gw_int32 getGwlogLevel();
gw_int32 setGwLogLevel(gw_int32 lv);
gw_int32 gw_syslog(gw_int32 level, const gw_int8 *String, ...);

#if 0
#define gw_log(lv, log...) if(lv >= getGwlogLevel()) gw_printf (log)
#else
#define gw_log(lv, log...) gw_syslog(lv, log)
#endif

#endif /* GW_LOG_H_ */
