/*
 * gw_conf_file.h
 *
 *  Created on: 2013年9月3日
 *      Author: tommy
 */

#ifndef GW_CONF_FILE_H_
#define GW_CONF_FILE_H_

#include "../include/gw_os_common.h"
#include "../apps/gw_log.h"

typedef gw_int32 (*func_conf_save)(gw_int32 * len, gw_uint8 ** pv );
typedef gw_int32 (*func_conf_restore)(gw_int32 len, gw_uint8 *pv);

 enum{
	GW_CONF_TYPE_MIN = 1,
	GW_CONF_TYPE_QOSVLANQUEUE = 1,
	GW_CONF_TYPE_POE_CONFIG,
	GW_CONF_TYPE_MAX
};

gw_int32 gw_register_conf_handlers(gw_int32 type, func_conf_save s, func_conf_restore r);

gw_int32 gw_conf_file_init();

gw_int32 gw_conf_save();
gw_int32 gw_conf_restore();

#endif /* GW_CONF_FILE_H_ */
