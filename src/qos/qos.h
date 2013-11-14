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


void gw_qos_init();
gw_int32 gw_qos_vlan_queue_rules_apply();

#endif /* QOS_H_ */
