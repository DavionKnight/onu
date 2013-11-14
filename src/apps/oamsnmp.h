/*
 * oamsnmp.h
 *
 *  Created on: 2013-3-25
 *      Author: tommy
 */

#ifndef OAMSNMP_H_
#define OAMSNMP_H_

#include "../include/gw_os_api_core.h"
#include "oam.h"


void init_oamsnmp();

gw_int32 get_oamsnmp_socket();

gw_uint32 get_oamsnmp_send_no();
gw_uint32 set_oamsnmp_send_no(gw_uint32 no);
void set_oamsnmp_session_id(gw_int8 * sid, gw_int32 len);
gw_int8* get_oamsnmp_session_id();

gw_status gwd_oamsnmp_handle(GWTT_OAM_MESSAGE_NODE * msg);

#endif /* OAMSNMP_H_ */
