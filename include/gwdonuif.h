/*
 * gwdonuif.h
 *
 *  Created on: 2012-11-5
 *      Author: tommy
 */

#ifndef GWDONUIF_H_
#define GWDONUIF_H_

#include "../include/gw_types.h"

typedef enum{
	PORT_ADMIN_DOWN,
	PORT_ADMIN_UP
}gwd_port_admin_t;

typedef enum{
	PORT_OPER_STATUS_DOWN,
	PORT_OPER_STATUS_UP
}gwd_port_oper_status_t;

typedef gw_int32 (*libgwdonu_special_frame_handler_t)(gw_int8 *pkt, const gw_int32 len, gw_int32 portid);

typedef gw_status (*libgwdonu_port_send_t)(gw_int32 portid, gw_uint8 *buf, gw_uint32 len);
typedef gw_uint32 (*libgwdonu_oam_std_hdr_builer_t)(gw_uint8*, gw_uint32);
typedef gw_status (*libgwdonu_onu_llid_get_t)(gw_uint32 *llid);
typedef gw_status (*libgwdonu_sys_info_get_t)(gw_uint8 * sysmac, gw_uint32 *uniportnum);
typedef gw_uint32 (*libgwdonu_sys_conf_save_t)(gw_uint8 * info, gw_uint32 len);
typedef gw_uint32 (*libgwdonu_sys_conf_restore_t)(gw_uint8 *info, gw_uint32 len);

typedef gw_status (*libgwdonu_port_admin_status_get_t)(gw_int32 portid, gwd_port_admin_t *status);
typedef gw_status (*libgwdonu_port_admin_status_set_t)(gw_int32 portid, gwd_port_admin_t status);
typedef gw_status (*libgwdonu_port_oper_status_get_t)(gw_int32 portid, gwd_port_oper_status_t *status);
typedef gw_status (*libgwdonu_port_mode_get_t)(gw_int32 portid, gw_int32 *spd, gw_int32 *duplex);
typedef gw_status (*libgwdonu_port_mode_set_t)(gw_int32 portid, gw_int32 spd, gw_int32 duplex);
typedef gw_status (*libgwdonu_port_isolate_get_t)(gw_int32 portid, gw_int32 *en);
typedef gw_status (*libgwdonu_port_isolate_set_t)(gw_int32 portid, gw_int32 en);

typedef gw_status (*libgwdonu_vlan_entry_getnext_t)(gw_uint32 index, gw_uint16 *vlanid, gw_uint32 *tag_portlist, gw_uint32 *untag_portlist);
typedef gw_status (*libgwdonu_vlan_entry_get_t)(gw_uint32 vlanid, gw_uint32 *tag_portlist, gw_uint32 *untag_portlist);
typedef gw_status (*libgwdonu_fdb_entry_get_t)(gw_uint32 vid, gw_uint8 * macaddr, gw_uint32 *eg_portlist);
typedef gw_status (*libgwdonu_fdb_entry_getnext_t)(gw_uint32 vid, gw_uint8 * macaddr, gw_uint32 *nextvid, gw_uint8 *nextmac, gw_uint32 * eg_portlist);
typedef gw_status (*libgwdonu_fdb_mgt_mac_set_t)(gw_uint8 * mac);
typedef gw_status (*libgwdonu_atu_learn_get_t)(gw_int32 *en);
typedef gw_status (*libgwdonu_atu_learn_set_t)(gw_int32 en);
typedef gw_status (*libgwdonu_atu_age_get_t)(gw_int32 *age);
typedef gw_status (*libgwdonu_atu_age_set_t)(gw_int32 age);

typedef gw_status (*libgwdonu_opm_get_t)(gw_uint16 *temp,gw_uint16 *vcc,gw_uint16 *bias,gw_uint16 *txpow,gw_uint16 *rxpow);

typedef gw_status (*libgwdonu_port_loop_event_post_t)(gw_uint32 status);
typedef gw_status (*libgwdonu_onu_register_special_frame_hanler_t)(libgwdonu_special_frame_handler_t handler);

typedef struct gwdonu_im_if_s{

	libgwdonu_onu_llid_get_t onullidget;
	libgwdonu_sys_info_get_t sysinfoget;
	libgwdonu_sys_conf_save_t sysconfsave;
	libgwdonu_sys_conf_restore_t sysconfrestore;
	libgwdonu_port_send_t portsend;
	libgwdonu_oam_std_hdr_builer_t oamhdrbuilder;

	libgwdonu_port_admin_status_get_t portadminget;
	libgwdonu_port_admin_status_set_t portadminset;
	libgwdonu_port_oper_status_get_t    portoperstatusget;
	libgwdonu_port_mode_get_t		portmodeget;
	libgwdonu_port_mode_set_t		portmodeset;
	libgwdonu_port_isolate_get_t		portisolateget;
	libgwdonu_port_isolate_set_t		portisolateset;

	libgwdonu_vlan_entry_getnext_t		vlanentrygetnext;
	libgwdonu_vlan_entry_get_t		vlanentryget;
	libgwdonu_fdb_entry_get_t		fdbentryget;
	libgwdonu_fdb_entry_getnext_t	fdbentrygetnext;
	libgwdonu_fdb_mgt_mac_set_t	fdbmgtmacset;
	libgwdonu_atu_learn_get_t		atulearnget;
	libgwdonu_atu_learn_set_t		atulearnset;
	libgwdonu_atu_age_get_t			atuageget;
	libgwdonu_atu_age_set_t			atuageset;

	libgwdonu_opm_get_t			opmget;

	libgwdonu_port_loop_event_post_t portloopnotify;

	libgwdonu_onu_register_special_frame_hanler_t specialpkthandler;

}gwdonu_im_if_t;

gw_status reg_gwdonu_im_interfaces(gwdonu_im_if_t * ifs);

#endif /* GWDONUIF_H_ */
