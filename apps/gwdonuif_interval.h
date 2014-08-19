/*
 * gwdonuif_interval.h
 *
 *  Created on: 2012-11-6
 *      Author: tommy
 */

#ifndef GWDONUIF_INTERVAL_H_
#define GWDONUIF_INTERVAL_H_

#include "../include/gwdonuif.h"
#include "../include/gw_os_api_core.h"

enum{    
	LIB_IF_ONU_LLID_GET,
	LIB_IF_SYSINFO_GET,
	LIB_IF_SYSCONF_SAVE,
	LIB_IF_SYSCONF_RESTORE,
	LIB_IF_SWCONF_SAVE,
	LIB_IF_SWCONF_RESTORE,
	LIB_IF_PORTSEND,
	LIB_IF_OAM_HDR_BUILDER,

	LIB_IF_PORT_ADMIN_GET,
	LIB_IF_PORT_ADMIN_SET,
	LIB_IF_PORT_OPER_STATUS_GET,
	LIB_IF_PORT_MODE_GET,
	LIB_IF_PORT_MODE_SET,	
	LIB_IF_PORT_ISOLATE_GET,
	LIB_IF_PORT_ISOLATE_SET,
	LIB_IF_PORT_STATISTIC_GET,
	LIB_IF_PORT_STATISTIC_CLEAR,
	LIB_IF_PORT_PVID_GET,
	LIB_IF_PORT_MIRROR_STAT_GET,
	LIB_IF_PORT_MIRROR_STAT_SET,
	LIB_IF_PORT_INGRESS_MIRROR_SET,
	LIB_IF_PORT_EGRESS_MIRROR_SET,
	LIB_IF_MIRROR_TO_PORT_SET,

	LIB_IF_VLAN_ENTRY_GETNEXT,
	LIB_IF_VLAN_ENTRY_GET,
	
	LIB_IF_FDB_ENTRY_GET,
	LIB_IF_FDB_ENTRY_GETNEXT,
	LIB_IF_FDB_MGT_MAC_SET,
	LIB_IF_ATU_LEARN_GET,
	LIB_IF_ATU_LEARN_SET,
	LIB_IF_ATU_AGE_GET,
	LIB_IF_ATU_AGE_SET,	

	LIB_IF_ONU_MAC_SET,

	LIB_IF_OPM_GET,
	LIB_IF_LASER_GET,
	LIB_IF_LASER_SET,
	
	LIB_IF_PORT_LOOP_EVENT_POST,
	LIB_IF_SPECIAL_PKT_HANDLER_REGIST,
	LIB_IF_SYSTERM_CURRENT_MS_TIME_GET,
	LIB_IF_BROADCAST_SPEED_LIMIT,
	LIB_IF_LOCALTIME_GET,
	LIB_IF_STATIC_MAC_ADD,
	LIB_IF_STATIC_MAC_DEL,
	LIB_IF_ONU_REGISTER_GET,
#ifndef CYG_LINUX
	LIB_IF_ONU_REBOOT,
#endif
	LIB_IF_ONU_START_LOOP_LED,
	LIB_IF_ONU_STOP_LOOP_LED,
	LIB_IF_OLT_MAC_GET,
	LIB_IF_ONU_VER_GET,
	LIB_IF_ONU_SYSLOG_REGIST,

	LIB_IF_REG_OUT_HWVER_GET,
	LIB_IF_REG_CONSOLE_ENTRY,

	LIB_IF_VFILE_OPEN,
	LIB_IF_VFILE_CLOSE,
	LIB_IF_QOS_VLAN_QUEUE_MAP,
	LIB_IF_CONF_WR_FLASH,
	LIB_IF_VER_BUILD_TIME_GET,
#if(RPU_MODULE_POE == RPU_YES)
    LIB_IF_CPLD_REGISTER_READ,
    LIB_IF_CPLD_REGISTER_WRITE,
    LIB_IF_POE_PORT_OPERATION_SET,
#endif
    LIB_IF_MULTICAST_MODE_SET,
    LIB_IF_MULTICAST_MODE_GET,
    LIB_IF_REAL_PRODUCT_TYPE_GET,
    LIB_IF_ENTRY_SDK_CLI,

    LIB_IF_TVM_STATUS_SET,
    LIB_IF_TVM_STATUS_GET,
    LIB_IF_TVM_RELATION_TABEL_CLEAR,
    LIB_IF_CTC_Mcast_CTRL_TABEL_CLEAR,
    LIB_IF_TVM_RELATION_TABEL_ITEM_ADD,
    LIB_IF_TVM_RELATION_TABEL_IP_DELETE,
    LIB_IF_TVM_RELATION_TABEL_CRC_GET,
    LIB_IF_TVM_RELATION_TABEL_VLAN_DELETE,
    LIB_IF_TVM_RELATION_TABEL_DEBUG_SHOW,
    LIB_IF_TVM_RELATION_TABEL_COUNT,
    
    LIB_IF_MGTIF_INETCONFIG_ADD,
    LIB_IF_MGTIF_INETCONFIG_GET,
    LIB_IF_MGTIF_INETCONFIG_DEL,

    LIB_IF_VLAN_FIELD_CFG_GET,
    LIB_IF_RCP_FIELD_CFG_SET,
    LIB_IF_THREAD_INFO_GET,

};

gw_status init_im_interfaces();

gw_status call_gwdonu_if_api(gw_int32 type, gw_int32 argc, ...);

#endif /* GWDONUIF_INTERVAL_H_ */
