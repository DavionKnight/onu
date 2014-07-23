/*
 * qos.h
 *
 *  Created on:
 *      Author: tommy
 */

#ifndef QOS_H_
#define QOS_H_

#include "../include/gw_os_common.h"
#include "../apps/gw_log.h"
#include "../apps/gw_conf_file.h"
#include "../apps/gwdonuif_interval.h"

#define GWD_ONU_QOS_VLAN_MAX 4092
#define GWD_ONU_QOS_VLAN_LEAST 2
#define GWD_ONU_OQS_QUEUE_MAX 7
#define GWD_ONU_QOS_QUEUE_LEAST 0


void gw_qos_init();
gw_int32 gw_qos_vlan_queue_rules_apply();

#endif /* QOS_H_ */
