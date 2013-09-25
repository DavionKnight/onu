/*
 * oam_core.h
 *
 *  Created on: 2012-11-8
 *      Author: tommy
 */

#ifndef OAM_CORE_H_
#define OAM_CORE_H_

#include "../include/gw_os_api_core.h"
#include "oam.h"


#define OAM_CLI_OUT_BUF_LENGTH 2048

void init_gw_oam_async();
void init_oam_pty();

///out put cli result to olt
gw_status gwd_oam_async_trans(GWTT_OAM_MESSAGE_NODE *);
gw_status gwd_oam_pty_trans(GWTT_OAM_MESSAGE_NODE * msg);
gw_status gwd_oam_cli_trans_send_out();

void start_oamPtyCliThread();

#endif /* OAM_CORE_H_ */
