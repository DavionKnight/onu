/*
 * qos.h
 *
 *  Created on: 2013年9月5日
 *      Author: tommy
 */

#ifndef QOS_H_
#define QOS_H_

#include "../include/gw_os_common.h"
#include "../apps/gw_log.h"
#include "../apps/gw_conf_file.h"
#include "../apps/gwdonuif_interval.h"

#define gwd_onu_qos_vlan_max 4092
#define gwd_onu_qos_vlan_least 2
#define gwd_onu_qos_queue_max 7
#define gwd_onu_qos_queue_least 0


void gw_qos_init();
gw_int32 gw_qos_vlan_queue_rules_apply();

#endif /* QOS_H_ */
