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

typedef gw_status (*libgwdonu_port_send_t)(gw_int32 portid, gw_uint8 *buf, gw_uint32 len);
typedef gw_uint32 (*libgwdonu_oam_std_hdr_builer_t)(gw_uint8*, gw_uint32);
typedef gw_status (*libgwdonu_sys_info_get_t)(gw_uint8 * sysmac, gw_uint32 *uniportnum);
typedef gw_uint32 (*libgwdonu_sys_conf_save_t)(gw_uint8 * info, gw_uint32 len);
typedef gw_uint32 (*libgwdonu_sys_conf_restore_t)(gw_uint8 *info, gw_int32 len);

typedef gw_status (*libgwdonu_port_admin_status_get_t)(gw_int32 portid, gwd_port_admin_t *status);
typedef gw_status (*libgwdonu_port_admin_status_set_t)(gw_int32 portid, gwd_port_admin_t status);

typedef struct gwdonu_im_if_s{

	libgwdonu_sys_info_get_t sysinfoget;
	libgwdonu_sys_conf_save_t sysconfsave;
	libgwdonu_sys_conf_restore_t sysconfrestore;
	libgwdonu_port_send_t portsend;
	libgwdonu_oam_std_hdr_builer_t oamhdrbuilder;

	libgwdonu_port_admin_status_get_t portadminget;
	libgwdonu_port_admin_status_set_t portadminset;

}gwdonu_im_if_t;

gw_status reg_gwdonu_im_interfaces(gwdonu_im_if_t * ifs);

#endif /* GWDONUIF_H_ */
