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
	LIB_IF_ONU_LLID_GET,
	LIB_IF_SYSINFO_GET,
	LIB_IF_SYSCONF_SAVE,
	LIB_IF_SYSCONF_RESTORE,
	LIB_IF_PORTSEND,
	LIB_IF_OAM_HDR_BUILDER,

	LIB_IF_PORT_ADMIN_GET,
	LIB_IF_PORT_ADMIN_SET,
	LIB_IF_PORT_OPER_STATUS_GET,

	LIB_IF_VLAN_ENTRY_GETNEXT,
	LIB_IF_VLAN_ENTRY_GET,
	LIB_IF_FDB_ENTRY_GET,
	LIB_IF_PORT_LOOP_EVENT_POST,
};

gw_status init_im_interfaces();

gw_status call_gwdonu_if_api(gw_int32 type, gw_int32 argc, ...);

#endif /* GWDONUIF_INTERVAL_H_ */
