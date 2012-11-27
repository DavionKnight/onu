/*
 * gwd_eth_loop_detect.h
 *
 *  Created on: 2012-7-18
 *      Author: tommy
 */

#ifndef GWD_ETH_LOOP_DETECT_H_
#define GWD_ETH_LOOP_DETECT_H_

#if 0
#include "immenstar.h"
#endif

typedef enum{
	GWD_ETH_PORT_LOOP_ALARM=1,
	GWD_ETH_PORT_LOOP_ALARM_CLEAR,
}GWD_ETH_LOOP_STATUS_T;

typedef struct{
	unsigned long int msgtype;
#if 0
	unsigned long int portid;
	unsigned long int vid;
#endif
	unsigned long int loopstatus;
}gwd_ethloop_msg_t;

#if 0
int gwdEthPortLoopMsgBuildAndSend(unsigned long int portid, unsigned long int vid, unsigned long int status);
#else
int gwdEthPortLoopMsgBuildAndSend(unsigned long int status);
int gwdEthPortLoopLedAction();
#endif

#endif /* GWD_ETH_LOOP_DETECT_H_ */
