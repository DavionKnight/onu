#ifndef __IGMP_TVM__
#define __IGMP_TVM__

#define TVM_ENABLE		1
#define TVM_DISABLE		2
typedef enum{
    GwdTvmMcastTabelADD = 1,
    GwdTvmMcastTabelDelete_IpIdx,
    GwdTvmMcastStatusSync,
    GwdTvmMcastTabelDelete_VlanIdx,
    GwdTvmMcastTabel_OnuRespond
}

typedef struct igmp_relation_table
{
	gw_uint8 VID[2];
	gw_uint8 pon_id[2];
	gw_uint8 start_ip[4];
	gw_uint8 end_ip[4];
	gw_uint8 ulIfindex[4];
	gw_uint8 llid[4];
}igmp_relation_table_t;


typedef struct oam_through_vlan_igmp
{
	gw_uint8 enable[2];
	gw_uint8 type[2];
	gw_uint8 crc[4];
	gw_uint8 count[2];
	igmp_relation_table_t igmp_relation_table;
}oam_through_vlan_igmp_t;


typedef struct Through_Vlan_Group {
	gw_uint16 IVid;			/*vlan id*/
	gw_uint16 PonId;	/*onu pon id*/
	gw_ulong32 GroupSt;		/*连续组播对应表的第一项*/
	gw_ulong32 GroupEnd;		/*连续组播对应表的最后一项*/
	gw_ulong32 ulIfIndex;		/*ONU端口号，预留*/
	gw_ulong32 llid;			/*onu llid recerved*/
	struct Through_Vlan_Group *next;
} Through_Vlan_Group_t;

typedef struct TVM_Cont_Head{
	int TVMCOUNT;
	int  CrcResult;
	Through_Vlan_Group_t *Through_Vlan_Group_head;
}TVM_Cont_Head_t;
#endif

