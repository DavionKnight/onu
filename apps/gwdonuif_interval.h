/*
 * gwdonuif_interval.h
 *
 *  Created on: 2012-11-6
 *      Author: tommy
 */

#ifndef GWDONUIF_INTERVAL_H_
#define GWDONUIF_INTERVAL_H_

#include "../include/gwdonuif.h"

enum{
	LIB_IF_SYSINFO_GET,
	LIB_IF_SYSCONF_SAVE,
	LIB_IF_SYSCONF_RESTORE,
	LIB_IF_PORTSEND,
	LIB_IF_OAM_HDR_BUILDER
};

gw_status init_im_interfaces();

gw_status call_gwdonu_if_api(gw_int32 type, gw_int32 argc, ...);

#endif /* GWDONUIF_INTERVAL_H_ */
