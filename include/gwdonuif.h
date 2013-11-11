/*
 * gwdonuif.h
 *
 *  Created on: 2012-11-5
 *      Author: tommy
 */

#ifndef GWDONUIF_H_
#define GWDONUIF_H_


#define version_len 100
typedef enum{
    GWD_ETH_PORT_LOOP_ALARM=1,
    GWD_ETH_PORT_LOOP_ALARM_CLEAR,
}GWD_ETH_LOOP_STATUS_T;

typedef enum{
	PORT_ADMIN_DOWN,
	PORT_ADMIN_UP
}gwd_port_admin_t;

typedef enum{
	PORT_OPER_STATUS_DOWN,
	PORT_OPER_STATUS_UP
}gwd_port_oper_status_t;

typedef enum{
	GWD_PORT_SPD_AUNEG = 1,
	GWD_PORT_SPD_10,
	GWD_PORT_SPD_100,
	GWD_PORT_SPD_1000
}gwd_port_mode_spd_t;

typedef enum{
	GWD_PORT_DUPLEX_AUNEG = 1,
	GWD_PORT_DUPLEX_FULL,
	GWD_PORT_DUPLEX_HALF
}gwd_port_mode_duplex_t;

typedef enum {
        PORT_INGRESS_LIMIT_ALL = 0,
        PORT_INGRESS_LIMIT_MC_BC_FLOODEDUC,
        PORT_INGRESS_LIMIT_BC_MC,
        PORT_INGRESS_LIMIT_BC, 
        PORT_INGRESS_LIMIT_UC
}gwd_sw_port_inratelimit_mode_t;

typedef struct {
        gw_uint64 RxFramesOk;
        gw_uint64 RxUnicasts;
        gw_uint64 RxMulticasts;
        gw_uint64 RxBroadcasts;
        gw_uint64 Rx64Octets;
        gw_uint64 Rx127Octets;
        gw_uint64 Rx255Octets;
        gw_uint64 Rx511Octets;
        gw_uint64 Rx1023Octets;
        gw_uint64 RxMaxOctets;
        gw_uint64 RxJumboOctets;
        gw_uint64 RxUndersize;
        gw_uint64 RxOversize;
        gw_uint64 RxFragments;
        gw_uint64 RxJabber;
        gw_uint64 RxFCSErrors;
        gw_uint64 RxDiscards;
        gw_uint64 RxAlignErrors;
        gw_uint64 RxIntMACErrors;
        gw_uint64 RxPppoes;
        gw_uint64 RxQueueFull;
        gw_uint64 RxPause;
        gw_uint64 RxOctetsOkMsb;
        gw_uint64 RxOctetsOKLsb;
        gw_uint64 RxError;
        gw_uint64 TxFramesOk;
        gw_uint64 TxUnicasts;
        gw_uint64 TxMulticasts;
        gw_uint64 TxBroadcasts;
        gw_uint64 Tx64Octets;
        gw_uint64 Tx127Octets;
        gw_uint64 Tx255Octets;
        gw_uint64 Tx511Octets;
        gw_uint64 Tx1023Octets;
        gw_uint64 TxMaxOctets;
        gw_uint64 TxJumboOctets;
        gw_uint64 TxDeferred;
        gw_uint64 TxTooLongFrames;
        gw_uint64 TxCarrierErrFrames;
        gw_uint64 TxSqeErrFrames;
        gw_uint64 TxSingleCollisions;
        gw_uint64 TxMultipleCollisions;
        gw_uint64 TxExcessiveCollisions;
        gw_uint64 TxLateCollisions;
        gw_uint64 TxMacErrFrames;
        gw_uint64 TxQueueFull;
        gw_uint64 TxPause;
        gw_uint64 TxOctetsOk;
        gw_uint64 TxError;
} __attribute__((packed)) gw_onu_counter_t;
typedef struct gw_tm {
    int tm_sec;      // seconds after the minute - [0..61] 
                     //   (note 61 allows for two leap seconds)
    int tm_min;      // minutes after the hour - [0..59]
    int tm_hour;     // hours since midnight - [0..23]
    int tm_mday;     // days of the month - [1..31]
    int tm_mon;      // months since January - [0..11]
    int tm_year;     // years since 1900
    int tm_wday;     // days since Sunday - [0..6]
    int tm_yday;     // days since January 1 - [0..365]
    int tm_isdst;    // Daylight Saving Time flag - positive if DST is in
                     // effect, 0 if not in effect, and negative if the info
                     // is not available
}localtime_tm;

typedef struct{
	gw_uint32 rxrate;
	gw_uint32 txrate;
	gw_onu_counter_t counter;
} __attribute__((packed)) gw_onu_port_counter_t;

typedef enum {
    GwEponTxLaserNormal       = 0,
    GwEponTxLaserAlwaysOn     = 1,
    GwEponTxLaserDisable      = 2,
    GwEponTxLaserStatusNum
} gw_EponTxLaserStatus;

typedef enum{
	channel_serial=1,
	channel_tcp,
	channel_pty,
	channel_oam
}gw_cli_channel_type;

typedef struct {
	gw_uint32 port;
	gw_uint32 vid;
	gw_uint32 queue;
}gw_qos_vlan_queue_data_t;

typedef gw_int32 (*libgwdonu_special_frame_handler_t)(gw_int8 *pkt, const gw_int32 len, gw_int32 portid);
typedef gw_status (*libgwdonu_out_hw_version)(gw_int8 *hwbuf, const gw_int32 hwbuflen);

typedef gw_status (*libgwdonu_port_send_t)(gw_int32 portid, gw_uint8 *buf, gw_uint32 len);
typedef gw_uint32 (*libgwdonu_oam_std_hdr_builer_t)(gw_uint8*, gw_uint32);
typedef gw_status (*libgwdonu_onu_llid_get_t)(gw_uint32 *llid);
typedef gw_status (*libgwdonu_sys_info_get_t)(gw_uint8 * sysmac, gw_uint32 *uniportnum);
typedef gw_uint32 (*libgwdonu_sys_conf_save_t)(gw_uint8 * info, gw_uint32 len);
typedef gw_uint32 (*libgwdonu_sys_conf_restore_t)(gw_uint8 *info, gw_uint32 len);
typedef gw_uint32 (*libgwdonu_sw_conf_save_t)(gw_uint8 * info, gw_uint32 len);
typedef gw_uint32 (*libgwdonu_sw_conf_restore_t)(gw_uint8 *info, gw_uint32 len);
typedef gw_status (*libgwdonu_olt_mac_get_t)(gw_uint8 * mac);

typedef gw_status (*libgwdonu_port_admin_status_get_t)(gw_int32 portid, gwd_port_admin_t *status);
typedef gw_status (*libgwdonu_port_admin_status_set_t)(gw_int32 portid, gwd_port_admin_t status);
typedef gw_status (*libgwdonu_port_oper_status_get_t)(gw_int32 portid, gwd_port_oper_status_t *status);
typedef gw_status (*libgwdonu_port_mode_get_t)(gw_int32 portid, gw_int32 * auneg_en, gw_int32 *spd, gw_int32 *duplex);
typedef gw_status (*libgwdonu_port_mode_set_t)(gw_int32 portid, gw_int32 spd, gw_int32 duplex);
typedef gw_status (*libgwdonu_port_isolate_get_t)(gw_int32 portid, gw_int32 *en);
typedef gw_status (*libgwdonu_port_isolate_set_t)(gw_int32 portid, gw_int32 en);
typedef gw_status (*libgwdonu_port_statistic_get_t)(gw_int32 portid, gw_int8 * data, gw_int32 * len);
typedef gw_status (*libgwdonu_port_pvid_get_t)(gw_int32 portid, gw_int16 *vlanid);

typedef gw_status (*libgwdonu_vlan_entry_getnext_t)(gw_uint32 index, gw_uint16 *vlanid, gw_uint32 *tag_portlist, gw_uint32 *untag_portlist);
typedef gw_status (*libgwdonu_vlan_entry_get_t)(gw_uint32 vlanid, gw_uint32 *tag_portlist, gw_uint32 *untag_portlist);
typedef gw_status (*libgwdonu_fdb_entry_get_t)(gw_uint32 vid, gw_uint8 * macaddr, gw_uint32 *eg_portlist);
typedef gw_status (*libgwdonu_fdb_entry_getnext_t)(gw_uint32 vid, gw_uint8 * macaddr, gw_uint32 *nextvid, gw_uint8 *nextmac, gw_uint32 * eg_portlist,gw_uint32*statics);
typedef gw_status (*libgwdonu_fdb_mgt_mac_set_t)(gw_uint8 * mac);
typedef gw_status (*libgwdonu_atu_learn_get_t)(gw_int32 portid, gw_int32 *en);
typedef gw_status (*libgwdonu_atu_learn_set_t)(gw_int32 portid, gw_int32 en);
typedef gw_status (*libgwdonu_atu_age_get_t)(gw_uint32 *age);
typedef gw_status (*libgwdonu_atu_age_set_t)(gw_uint32 age);
typedef gw_status(*libgwdonu_set_mac_t)(gw_uint8 *mac);

typedef gw_status (*libgwdonu_opm_get_t)(gw_uint16 *temp,gw_uint16 *vcc,gw_uint16 *bias,gw_uint16 *txpow,gw_uint16 *rxpow);
typedef gw_status (*libgwdonu_laser_get_t)(gw_EponTxLaserStatus * state);
typedef gw_status (*libgwdonu_laser_set_t)(gw_EponTxLaserStatus state);

typedef gw_status (*libgwdonu_port_loop_event_post_t)(gw_uint32 status);
typedef gw_status (*libgwdonu_onu_register_special_frame_hanler_t)(libgwdonu_special_frame_handler_t handler);
typedef gw_status (*libgwdonu_onu_register_out_if_t)(void * handler);
typedef gw_status (*libgwdonu_onu_current_timer_get_t)(gw_uint32* gw_time);
typedef gw_status (*libgwdonu_onu_broadcast_speed_limit_set_t)(gw_uint32 gw_port,gwd_sw_port_inratelimit_mode_t gw_mode,gw_uint32 gw_rate);
typedef gw_status (*libgwdonu_onu_localtime_get_t)(localtime_tm * tm);
typedef gw_status (*libgwdonu_onu_static_mac_add_t)(gw_int8* gw_mac,gw_uint32 gw_port,gw_uint32 gw_vlan);
typedef gw_status (*libgwdonu_onu_static_mac_del_t)(gw_int8* gw_mac,gw_uint32 gw_vlan);
typedef gw_status (*libgwdonu_onu_register_stat_get)(gw_uint8* onuregister);
#ifndef CYG_LINUX
typedef gw_status (*libgwdonu_onu_reset)(gw_int32 a);
#endif

typedef void      (*libgwdonu_onu_set_loopalm_led)();
typedef gw_status (*libgwdonu_ver_get)(char * sw_ver, const int sw_ver_len);
typedef gw_status(*libgwdonu_syslog_heandler_t)(gw_int32 telve,gw_int8* logstring,...);
typedef gw_status(*libgwdonu_syslog_register_heandler_t)(libgwdonu_syslog_heandler_t handler);

typedef gw_int32 (*libgwdonu_console_read_t)(gw_uint8 *buf, gw_uint32 count);
typedef gw_int32 (*libgwdonu_console_write_t)(gw_uint8 *buf, gw_uint32 count);
typedef void (*libgwdonu_console_cli_entry_t)(gw_int32 type, gw_int32 fd, libgwdonu_console_read_t r, libgwdonu_console_write_t w);

typedef gw_int32 (*libgwdonu_vfile_open)(gw_uint8 * fname, gw_int32 mode, gw_int32 * fd, gw_uint8 ** pv);
typedef gw_int32 (*libgwdonu_vfile_close)(void *data);

typedef gw_int32 (*libgwdonu_qos_vlan_queue_map_t)(gw_int32 count, gw_qos_vlan_queue_data_t * data);

typedef gw_int32 (*libgwdonu_config_write_to_flash_t)();

typedef gw_int32 (*libgwdonu_port_mirror_stat_get_t)(gw_int32 unit,gw_int32*mode);
typedef gw_int32 (*libgwdonu_port_mirror_stat_set_t)(gw_int32 unit,gw_int32 mode);
typedef gw_int32 (*libgwdonu_port_ingress_mirror_set_t)(gw_int32 unit,gw_int32 port,gw_int32 stat_val);
typedef gw_int32 (*libgwdonu_port_egress_mirror_set_t)(gw_int32 unit,gw_int32 port,gw_int32 stat_val);
typedef gw_int32 (*libgwdonu_port_mirror_to_port_set_t)(gw_int32 port,gw_int32 portmap);
typedef gw_int32 (*libgwdonu_version_build_time_get_t)(gw_int8 buildtime[version_len]);

typedef gw_int32 (*libgwdonu_cpld_register_write)(gw_uint32 reg,gw_uint32 date);
typedef gw_int32 (*libgwdonu_cpld_register_read)(gw_uint32 reg,gw_uint8 * date);

typedef gw_int32 (*libgwdonu_poe_port_operation_set)(gw_int32 port,gw_int32 stat);

typedef gw_int32 (*libgwdonu_multicast_transmission_set)(gw_uint8 en);
typedef gw_int32 (*libgwdonu_multicast_transmission_get)(gw_uint8 *en);
typedef gw_int32 (*libgwdonu_real_product_type_get)(gw_uint8 *st);

typedef struct gwdonu_im_if_s{

	libgwdonu_onu_llid_get_t onullidget;
	libgwdonu_sys_info_get_t sysinfoget;
	libgwdonu_sys_conf_save_t sysconfsave;
	libgwdonu_sys_conf_restore_t sysconfrestore;
	libgwdonu_sw_conf_save_t	swconfsave;
	libgwdonu_sw_conf_restore_t swconfrestore;
	libgwdonu_port_send_t portsend;
	libgwdonu_oam_std_hdr_builer_t oamhdrbuilder;

	libgwdonu_port_admin_status_get_t portadminget;
	libgwdonu_port_admin_status_set_t portadminset;
	libgwdonu_port_oper_status_get_t    portoperstatusget;
	libgwdonu_port_mode_get_t		portmodeget;
	libgwdonu_port_mode_set_t		portmodeset;
	libgwdonu_port_isolate_get_t		portisolateget;
	libgwdonu_port_isolate_set_t		portisolateset;
	libgwdonu_port_statistic_get_t	portstatget;
	libgwdonu_port_pvid_get_t		portpvidget;
    libgwdonu_port_mirror_stat_get_t portmirrorstatget;
    libgwdonu_port_mirror_stat_set_t portmirrorstatset;
    libgwdonu_port_ingress_mirror_set_t portingressmirrorset;
	libgwdonu_port_egress_mirror_set_t portegressmirrorset;
    libgwdonu_port_mirror_to_port_set_t mirrortoportset;
    
	libgwdonu_vlan_entry_getnext_t		vlanentrygetnext;
	libgwdonu_vlan_entry_get_t		vlanentryget;
	libgwdonu_fdb_entry_get_t		fdbentryget;
	libgwdonu_fdb_entry_getnext_t	fdbentrygetnext;
	libgwdonu_fdb_mgt_mac_set_t	    fdbmgtmacset;
	libgwdonu_atu_learn_get_t		atulearnget;
	libgwdonu_atu_learn_set_t		atulearnset;
	libgwdonu_atu_age_get_t			atuageget;
	libgwdonu_atu_age_set_t			atuageset;

	libgwdonu_set_mac_t			onumacset;

	libgwdonu_opm_get_t			opmget;
	libgwdonu_laser_get_t           laserget;
	libgwdonu_laser_set_t           laserset;

	libgwdonu_port_loop_event_post_t portloopnotify;

	libgwdonu_onu_register_special_frame_hanler_t specialpkthandler;
	libgwdonu_onu_register_out_if_t onuhwverget;
	libgwdonu_onu_register_out_if_t console_cli_register;

	libgwdonu_onu_current_timer_get_t currenttimeget;
	libgwdonu_onu_broadcast_speed_limit_set_t broadlimit;
	libgwdonu_onu_localtime_get_t localtimeget;
	libgwdonu_onu_static_mac_add_t staticmacadd;
	libgwdonu_onu_static_mac_del_t staticmacdel;
	libgwdonu_onu_register_stat_get registerget;
#ifndef CYG_LINUX
	libgwdonu_onu_reset onureset;
#endif
	libgwdonu_onu_set_loopalm_led startloopled;
	libgwdonu_onu_set_loopalm_led stoploopled;

	libgwdonu_olt_mac_get_t			oltmacget;
	libgwdonu_ver_get				onuverget;

	libgwdonu_syslog_register_heandler_t     sysloghandler;

	libgwdonu_vfile_open         vfileopen;
	libgwdonu_vfile_close        vfileclose;

	libgwdonu_qos_vlan_queue_map_t qosvlanqueuemap;
	libgwdonu_config_write_to_flash_t wrflash;
    libgwdonu_version_build_time_get_t vertimeget;

	libgwdonu_cpld_register_read	cpldread;
    libgwdonu_cpld_register_write cpldwrite;
    libgwdonu_poe_port_operation_set poeportoperation;

    libgwdonu_multicast_transmission_set multicasttransmissionset;
    libgwdonu_multicast_transmission_get multicasttransmissionget;
    libgwdonu_real_product_type_get onurealproducttypeget;

}gwdonu_im_if_t;

gw_status reg_gwdonu_im_interfaces(gwdonu_im_if_t * ifs, gw_int32 size);

#endif /* GWDONUIF_H_ */
