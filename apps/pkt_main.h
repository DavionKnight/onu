/*
 * pkt_main.h
 *
 *  Created on: 2012-11-1
 *      Author: tommy
 */

#ifndef PKT_MAIN_H_
#define PKT_MAIN_H_

typedef enum{
	GW_PKT_OAM,
	GW_PKT_LPB,
	GW_PKT_RCP,
	GW_PKT_DHCP,
	GW_PKT_MAX
}GW_PKT_TYPE;

typedef struct{
	GW_PKT_TYPE type;
	gw_uint32 pri;
}GW_PKT_PRI_MAP;

typedef gw_int32 (*PKT_PARSE)(gw_int8 * pkt, const gw_int32 len);
typedef gw_status (*PKT_HANDLER)(gw_int8 * pkt, const gw_int32 len, gw_int32 portid);

typedef struct gw_pkt_cb_func_s{
	PKT_PARSE parser;
	PKT_HANDLER handler;
}gw_pkt_cb_func_t;

gw_status gw_reg_pkt_parse(GW_PKT_TYPE type, PKT_PARSE parser);
gw_status gw_reg_pkt_handler(GW_PKT_TYPE type, PKT_HANDLER handler);
GW_PKT_TYPE gw_pkt_parser_call(gw_int8 *pkt, const gw_int32 len);
gw_status gw_pkt_handler_call(GW_PKT_TYPE type, gw_int8 *pkt, const gw_int32 len, gw_int32 portid);

gw_status init_pkt_proc(void);
gw_int32 gwlib_sendPktToQueue(gw_int8 *pkt, const gw_int32 len, gw_int32 portid);

void gw_dump_pkt(gw_uint8 *pkt, gw_int16 len, gw_uint8 width);

#endif /* PKT_MAIN_H_ */
