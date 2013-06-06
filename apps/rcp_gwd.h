#if 1
#ifndef _RRCP_GW_H_
#define _RRCP_GW_H_
#if 1
#ifndef oid
typedef unsigned long	oid;
#endif
#endif

#define NEW_SUPPORT_8326 1  // 0: close new function for 8326
#define NEW_FEATURE2 1      // 0: close the function of Build 06 version
#define NEW_SUPPORT_NEW_CHIP 1 // 1: Support 8324P serial chips
#define Mirrored_Patch 1
#define THREAD_CABLETESTER  1
#define THREAD_SIGNALQUALITY 1
#define NEW_INI_CONFIG 1    // 0: Doesn't read rmt_config.ini file
#define NEW_ADD_AUTHKEY 1
#define NEW_SUPPORT_RL5866F 1 // 1: Support RL5866F chip

#define FLASH_GWD_RCG_SWITCH_CFG_MAX_SIZE	(11*1024)	/*the end of the gwd rcg switch config file*/
#define FLASH_GWD_RCG_SWITCH_CFG_OFFSET		(8*1024) /*the start of the gwd rc switch config file*/

#define VENDOR_AUTHKEY_LEN_MAX                    0x20   // 32 bytes
#define OAM_CONFIG_LEN_MAX                        0x100  // For  OAM config
#define GPIO_CONFIG_LEN_MAX                       0x14   // 4 bytes magic number + 16 bytes, magic number 0x179e5173 in little endianess.

#define LIOD_PASSWD_LEN_MAX                      0x24   //36 bytes

#define USER_DATA_USED_LEN                       (VENDOR_AUTHKEY_LEN_MAX+OAM_CONFIG_LEN_MAX+GPIO_CONFIG_LEN_MAX+LIOD_PASSWD_LEN_MAX)

//platform added new type and micro define

#define IFM_NAME_SIZE (31)
#define ETH_FILTER_MAX_PORT_NUM_ON_SLOT     ( 4 )
#define VTY_NEWLINE "\r\n"

struct slot_port{
	//void*  onuPort;
	unsigned long	ulSlot;
	unsigned long	ulPort;
};


#define ETH_SLOTPORT_TO_PORTNO( _slot, _port )           \
          ( (  ( _slot )  - 1 ) * ( ETH_FILTER_MAX_PORT_NUM_ON_SLOT ) + (  ( _port )  ) )


#define PORTNO_TO_ETH_SLOT( _portno )           \
            ( ( ( _portno ) / ( ETH_FILTER_MAX_PORT_NUM_ON_SLOT ) ) + 1 )
#define PORTNO_TO_ETH_PORTID( _portno )         \
            ( ( _portno ) % ( ETH_FILTER_MAX_PORT_NUM_ON_SLOT ) )


#define dot3MauType10BaseTHD 		10
#define dot3MauType10BaseTFD 		11
#define dot3MauType100BaseTXHD 	15
#define dot3MauType100BaseTXFD 	16
#define dot3MauType1000BaseTXHD 	29
#define dot3MauType1000BaseTXFD 	30
//end

/*Structure*/
typedef struct SwitchInfo
{
   unsigned char name[32];
   unsigned char mac[6];
   unsigned char mac_info[18];
   int uplinkPort;
   int downlinkPort[26];
   int depth;
   struct SwitchInfo* uplinkSwitch;
   struct SwitchInfo* downlinkSwitch[26];
} SwitchInformation;

typedef struct switch_form {
   unsigned char name[32];
   unsigned char mac[6];
   unsigned char mac_info[18];
   unsigned char chip_id[2];     // 16 bit
   unsigned char chip_id_info[7];
   unsigned char vendor_id[4];   // 32 bit
   unsigned char vendor_id_info[11];
   // New Add-ed
   unsigned char chip_type;
   unsigned char model_type;
   unsigned char panel_type;
   unsigned char exist_8051;
   unsigned char exist_eeprom;
   unsigned char g1_status;
   unsigned char g2_status;
   // For Port Authentication
   unsigned char dl_port;
   unsigned char mirr_mask[26];
   unsigned short mirr_regdata1;
   unsigned short mirr_regdata2;
} switch_;


#ifdef __cplusplus
extern "C" {
#endif
/* 
 * Exported Function Declarations
 */
#if 0
__declspec(dllexport) void  RRCP_Initialize(void);
__declspec(dllexport) void  RRCP_Finalize(void);

__declspec(dllexport) int   RRCP_GetAdapterNumber(void);
__declspec(dllexport) const char* RRCP_GetAdapterDescription(unsigned int num);
__declspec(dllexport) const char* RRCP_GetAdapterMacAddress(unsigned int num);
__declspec(dllexport) const char* RRCP_GetAdapterIpAddress(unsigned int num);

__declspec(dllexport) int   RRCP_SetAdapter(unsigned int num);
__declspec(dllexport) void  RRCP_BuildLinkDataBase(void);
__declspec(dllexport) void  RRCP_ReBuildLink(void);
__declspec(dllexport) void  RRCP_CheckMirrorFunction(void);

__declspec(dllexport) int   RRCP_GetLinkVectorNumber(void);
__declspec(dllexport) const char *RRCP_GetLinkVectorDownLinkMac(unsigned int num);
__declspec(dllexport) const char *RRCP_GetLinkVectorUpLinkMac(unsigned int num);
__declspec(dllexport) int   RRCP_GetLinkVectorDownLinkPort(unsigned int num);
__declspec(dllexport) int   RRCP_GetLinkVectorUpLinkPort(unsigned int num);

__declspec(dllexport) int   RRCP_GetLinkVectorDownLinkSwitchPort(unsigned int num);
__declspec(dllexport) int   RRCP_GetLinkVectorUpLinkSwitchPort(unsigned int num);

__declspec(dllexport) int   RRCP_GetSwitchNumber(void);
__declspec(dllexport) int   RRCP_GetSwitchInfo(unsigned int num);
__declspec(dllexport) const char* RRCP_GetSwitchName(unsigned int num);
__declspec(dllexport) const char* RRCP_GetSwitchMacAddress(unsigned int num);
__declspec(dllexport) const char* RRCP_GetSwitchChipId(unsigned int num);
__declspec(dllexport) const char* RRCP_GetSwitchVendorId(unsigned int num);
__declspec(dllexport) unsigned int RRCP_GetSwitchChipType(void);
__declspec(dllexport) unsigned int RRCP_GetSwitchVersionType(void);
__declspec(dllexport) unsigned int RRCP_GetSwitchModelType(void);
__declspec(dllexport) unsigned int RRCP_GetSwitchPanelType(void);

__declspec(dllexport) unsigned int RRCP_IsSwitchHas8051(void);
__declspec(dllexport) unsigned int RRCP_IsSwitchHasEEPROM(void);
__declspec(dllexport) unsigned int RRCP_GetSwitchG1Status(void);
__declspec(dllexport) unsigned int RRCP_GetSwitchG2Status(void);

__declspec(dllexport) unsigned int RRCP_GetSwitchDlPortNumber(void);
__declspec(dllexport) unsigned int RRCP_GetSwitchMirrPortStatus(unsigned int mum);

__declspec(dllexport) unsigned int RRCP_GetSwitchFastPortNumber(void);
__declspec(dllexport) unsigned int RRCP_GetSwitchGigaPortNumber(void);
__declspec(dllexport) unsigned int RRCP_GetSwitchMaxPortNumber(void);
__declspec(dllexport) unsigned int RRCP_GetSwitchMIIPortNumber(void);

__declspec(dllexport) int   RRCP_SetSwitch(unsigned int num);
__declspec(dllexport) const char* RRCP_GetAddressValue(unsigned short address);
__declspec(dllexport) unsigned int RRCP_GetAddressValueMask(unsigned short address, unsigned int mask);

__declspec(dllexport) void  RRCP_QueueReset(void);
__declspec(dllexport) int   RRCP_QueueRequest(unsigned short address);
__declspec(dllexport) void  RRCP_QueueSend(void);
__declspec(dllexport) const char* RRCP_QueueGetData(unsigned int num);
__declspec(dllexport) unsigned int RRCP_QueueGetDataMask(unsigned int num, unsigned int mask);

__declspec(dllexport) void  RRCP_QueueUpdate(void);
__declspec(dllexport) void  RRCP_QueueSetDataMask(unsigned int num, unsigned int mask, unsigned int data);
__declspec(dllexport) int   RRCP_QueueSetRequest(unsigned short address, unsigned long data);

__declspec(dllexport) void  RRCP_SetAddressValue(unsigned short address, unsigned value);

__declspec(dllexport) unsigned short RRCP_GetPhyAddressValue(unsigned int id, unsigned int regaddr);
__declspec(dllexport) unsigned short RRCP_GetPhyAddressValueNoWait(unsigned int id, unsigned int regaddr);

__declspec(dllexport) void RRCP_SetPhyAddressValue(unsigned int id, unsigned int regaddr, unsigned value);
__declspec(dllexport) void RRCP_SetPhyAddressValueNoWait(unsigned int id, unsigned int regaddr, unsigned value);

__declspec(dllexport) unsigned short RRCP_GetEepromAddressValue(unsigned int addr);
__declspec(dllexport) void RRCP_SetEepromAddressValue(unsigned int addr, unsigned int value);



__declspec(dllexport) int RRCP_GetSwitchChipStatus(unsigned int num);
__declspec(dllexport) int RRCP_GetSwitchFastPortStatus(unsigned int num);
__declspec(dllexport) int RRCP_GetSwitchFastPortMapping(unsigned int num);
__declspec(dllexport) int RRCP_GetSwitchGigaPortStatus(unsigned int num);
__declspec(dllexport) int RRCP_GetSwitchGigaPortMapping(unsigned int num);
__declspec(dllexport) int RRCP_GetSwitchMIIPortMapping(unsigned int num);
__declspec(dllexport) int RRCP_GetSwitchMIIPortStatus(unsigned int num);

__declspec(dllexport) void RRCP_SaveConfigurationToEEPROM(void);
__declspec(dllexport) void RRCP_LoadConfigurationFromEEPROM(void);
__declspec(dllexport) void RRCP_LoadFactoryDefaultConfiguration(void);

__declspec(dllexport) void  RRCP_AddSwitchDatabaseEntry(unsigned char *name, unsigned char *addr, unsigned char *key);
__declspec(dllexport) void  RRCP_ClearSwitchDatabase(void);

__declspec(dllexport) void  RRCP_UpdateAuthKey();

__declspec(dllexport) int RRCP_GetErrorFlag(void);
__declspec(dllexport) void RRCP_ClearErrorFlag(void);

__declspec(dllexport) unsigned int RRCP_PortMaxLinkSpeed(unsigned int *pidx, int num);
__declspec(dllexport) void RRCP_QueueClearReadRequestNumber(void);
__declspec(dllexport) void RRCP_PortLinkSpeedChange(unsigned int *pidx, int num);
__declspec(dllexport) int RRCP_TagBasedVlanChecking(unsigned int *pidx, int num);
__declspec(dllexport) int RRCP_VlanIDChecking(unsigned int *pidx, int num);
__declspec(dllexport) int RRCP_PortLinkUpChecking(unsigned int *pidx, int num);
__declspec(dllexport) int RRCP_PortLinkSpeedChecking(unsigned int *vidx, unsigned int *pidx, int num);
__declspec(dllexport) int RRCP_PortLinkStatusChecking(unsigned int *vidx, unsigned int *pidx, int num);
__declspec(dllexport) unsigned int RRCP_SetMacAddress(AnsiString macAddr);

__declspec(dllexport) void RRCP_SaveSecurityKeyToEEPROM(); /*save Security Key to EEPROM*/
__declspec(dllexport) void RRCP_SetFactoryDefaultMacAddress(unsigned char* mac); //get mac address from eeprom(factory default)

__declspec(dllexport) unsigned short RRCP_GetEepromAddressValue8051(unsigned int addr);
__declspec(dllexport) void RRCP_SetEepromAddressValue8051(unsigned int addr, unsigned int value);
__declspec(dllexport) unsigned short RRCP_GetSwitchSubVer(void);
// Topology
__declspec(dllexport) void RRCP_FindTopology(void);
__declspec(dllexport) SwitchInformation *RRCP_GetRootSwitch();
__declspec(dllexport) switch_ * RRCP_GetSwitchInformaition(const char* mac_info);
__declspec(dllexport) void freeNode(SwitchInformation *node);
#endif
#ifdef __cplusplus
}
#endif
/*
 * Exported Macros
 */

#define MASK_ALL_16                       0xFFFF
#define MASK_ALL_32                       0xFFFFFFFF

/*
 * System Configuration Registers
 */
#define RT8326_SYS_RESET_CTL_BASE         0x0000
#define     _SRST_Pos                     0
#define     _SRST_Mask                    0x1
#define     _HRST_Pos                     1
#define     _HRST_Mask                    0x1
#define RT8326_SYS_RESET_CTL              RT8326_SYS_RESET_CTL_BASE
#define     SRST_Val(val)                 ((val) << _SRST_Pos)
#define     HRST_Val(val)                 ((val) << _HRST_Pos)
#define     SRST_Mask                     (_SRST_Mask << _SRST_Pos)
#define     HRST_Mask                     (_HRST_Mask << _HRST_Pos)



#define RT8326_SWITCH_CTL                 0x0001
#define    _MAX_PKT_LEN_POS              0                   /*added by dushb */
#define    _MAX_PAUSE_CNT_POS          3
#define RT8326_RX_IO_PAD                  0x0002
#define RT8326_TX_IO_PAD                  0x0003
#define RT8326_USER_GPIO                  0x0004
#define RT8326_LED_DSPLY                  0x0005


#define RT8326_8051_COMM_BASE             0x0006
#define     _RstEventCtrlFlag_Pos         0
#define     _RstEventCtrlFlag_Mask        0x1
#define     _Exist8051_Pos                1
#define     _Exist8051_Mask               0x1
#define     _EnSaveCfg2EE_Pos             2
#define     _EnSaveCfg2EE_Mask            0x1
#define     _EnDownload_Pos               3
#define     _EnDownload_Mask              0x1
#define     _EnFactoryDefault_Pos         4
#define     _EnFactoryDefault_Mask        0x1
#define     _EE_Warning_Pos               5
#define     _EE_Warning_Mask              0x1
#define     _VlanTableInit_Pos            6
#define     _VlanTableInit_Mask           0x1
#define     _MaxUserVlan_Pos              8
#define     _MaxUserVlan_Mask             0x1F
#define RT8326_8051_COMM                  RT8326_8051_COMM_BASE
#define     RstEventCtrlFlag_Val(val)     ((val) << _RstEventCtrlFlag_Pos)
#define     Exist8051_Val(val)            ((val) << _Exist8051_Pos)
#define     EnSaveCfg2EE_Val(val)         ((val) << _EnSaveCfg2EE_Pos)
#define     EnDownload_Val(val)           ((val) << _EnDownload_Pos)
#define     EnFactoryDefault_Val(val)     ((val) << _EnFactoryDefault_Pos)
#define     EE_Warning_Val(val)           ((val) << _EE_Warning_Pos)
#define     VlanTableInit_Val(val)        ((val) << _VlanTableInit_Pos)
#define     MaxUserVlan_Val(val)          ((val) << _MaxUserVlan_Pos)
#define     RstEventCtrlFlag_Mask         (_RstEventCtrlFlag_Mask << _RstEventCtrlFlag_Pos)
#define     Exist8051_Mask                (_8051Exist_Mask << _Exist8051_Pos)
#define     EnSaveCfg2EE_Mask             (_EnSaveCfg2EE_Mask << _EnSaveCfg2EE_Pos)
#define     EnDownload_Mask               (_EnDownload_Mask << _EnDownload_Pos)
#define     EnFactoryDefault_Mask         (_EnFactoryDefault_Mask << _EnFactoryDefault_Pos)
#define     EE_Warning_Mask               (_EE_Warning_Mask << _EE_Warning_Pos)
#define     VlanTableInit_Mask            (_VlanTableInit_Mask << _VlanTableInit_Pos)
#define     MaxUserVlan_Mask              (_MaxUserVlan_Mask << _MaxUserVlan_Pos)


/*
 * System Status Registers
 */
#define RT8326_BOARD_TRAP                 0x0100

#define RT8326_PORT_LOOP_DETECT_BASE      0x0101
#define     _Port_Loop_Detect_Mask        0x1
#define RT8326_PORT_LOOP_DETECT(i)        (RT8326_PORT_LOOP_DETECT_BASE)
#define     Port_Loop_Detect_Mask(i)      (_Port_Loop_Detect_Mask << i)

#define RT8326_SYS_FAULT_BASE             0x0102
#define     _Trunk_Fault_Pos              1
#define     _Trunk_Fault_Mask             0x1
#define     _Loop_Fault_Pos               2
#define     _Loop_Fault_Mask              0x1
#define     _GigaPHY_NotExist_G0_Pos      3
#define     _GigaPHY_NotExist_G0_Mask     0x1
#define     _GigaPHY_NotExist_G1_Pos      4
#define     _GigaPHY_NotExist_G1_Mask     0x1
#define     _FaultTkGroup_Pos             6  // 5
#define     _FaultTkGroup_Pos_1           5
#define     _FaultTkGroup_Mask            0x1
#define RT8326_SYS_FAULT(i)               RT8326_SYS_FAULT_BASE
#define     Trunk_Fault_Val(val)          ((val) << _Trunk_Fault_Pos)
#define     Loop_Fault_Val(val)           ((val) << _Loop_Fault_Pos)
#define     GigaPHY_NotExist_G0_Val(val)  ((val) << _GigaPHY_NotExist_G0_Pos)
#define     GigaPHY_NotExist_G1_Val(val)  ((val) << _GigaPHY_NotExist_G1_Pos)
#define     FaultTkGroup_Val(i,val)       (((val) << _FaultTkGroup_Pos) << (i >> 2))
//#define     FaultTkGroup_Val(i,val)       (i >> 2) ? (((val) << _FaultTkGroup_Pos) << ((i >> 1) + 2) / 2) : (((val) << _FaultTkGroup_Pos) << (i >> 1))
#define     Trunk_Fault_Mask              (_Trunk_Fault_Mask << _Trunk_Fault_Pos)
#define     Loop_Fault_Mask               (_Loop_Fault_Mask << _Loop_Fault_Pos)
#define     GigaPHY_NotExist_G0_Mask      (_GigaPHY_NotExist_G0_Mask << _GigaPHY_NotExist_G0_Pos)
#define     GigaPHY_NotExist_G1_Mask      (_GigaPHY_NotExist_G1_Mask << _GigaPHY_NotExist_G1_Pos)
#define     FaultTkGroup_Mask(i)          ((_FaultTkGroup_Mask << _FaultTkGroup_Pos) << (i >> 2))
#define     FaultTkGroup_Mask_1(i)        ((_FaultTkGroup_Mask << _FaultTkGroup_Pos_1) << (i >> 1))
//#define     FaultTkGroup_Mask(i)          (i >> 2) ? ((_FaultTkGroup_Mask << _FaultTkGroup_Pos) << ((i >> 1) + 2) / 2) : ((_FaultTkGroup_Mask << _FaultTkGroup_Pos) << (i >> 1))

//#define     GigaPHY_NotExist_G0           0x0008
//#define     GigaPHY_NotExist_G1           0x0010


/*
 * Management Configuration Registers
 */
#define RT8326_RRCP_CTL_BASE              0x0200
#define     _RRCP_Disabled_Pos            0
#define     _RRCP_Disabled_Mask           0x1
#define     _REcho_Disabled_Pos           1
#define     _REcho_Disabled_Mask          0x1
#define     _Loop_Fault_Detect_Pos        2
#define     _Loop_Fault_Detect_Mask       0x1
#define RT8326_RRCP_CTL                   RT8326_RRCP_CTL_BASE
#define     RRCP_Disabled_Val(val)        ((val) << _RRCP_Disabled_Pos)
#define     REcho_Disabled_Val(val)       ((val) << _REcho_Disabled_Pos)
#define     Loop_Fault_Detect_Val(val)    ((val) << _Loop_Fault_Detect_Pos)
#define     RRCP_Disabled_Mask            (_RRCP_Disabled_Mask << _RRCP_Disabled_Pos)
#define     REcho_Disabled_Mask           (_REcho_Disabled_Mask << _REcho_Disabled_Pos)
#define     Loop_Fault_Detect_Mask        (_Loop_Fault_Detect_Mask << _Loop_Fault_Detect_Pos)


#define RT8326_RRCP_SECURITY_BASE         0x0201
#define     _RRCP_Security_Reg_Width      1
#define     _RRCP_Security_Reg_Packed     16
#define     _RRCP_Security_Pos            0
#define     _RRCP_Security_Mask           0x1
#define RT8326_RRCP_SECURITY_MASK(i)      (RT8326_RRCP_SECURITY_BASE + 0x0001 * (i > 15))
#define     RRCP_Security_Val(i,val)      (((val) << _RRCP_Security_Pos) << (((i) & (_RRCP_Security_Reg_Packed-1)) * _RRCP_Security_Reg_Width))
#define     RRCP_Security_Mask(i)         ((_RRCP_Security_Mask << _RRCP_Security_Pos) << (((i) & (_RRCP_Security_Reg_Packed-1)) * _RRCP_Security_Reg_Width))

//#define RT8326_RRCP_MASK_0                0x0201
//#define RT8326_RRCP_MASK_1                0x0202


#define     RT8326_RRCP_MAC_0                 0x0203
#define     RT8326_RRCP_MAC_1                 0x0204
#define     RT8326_RRCP_MAC_2                 0x0205
#define     RT8326_RRCP_CHIPID                0x0206
#define     RT8326_RRCP_VENDORID0             0x0207
#define     RT8326_RRCP_VENDORID1             0x0208
#define     RT8326_RRCP_AUTHKEY               0x0209

// OK
#define RT8326_PORT_CTL_BASE              0x020A
#define     _Port_Ctl_Reg_Width           8
#define     _Port_Ctl_Reg_Packed          2
#define     _RX_Bandwidth_Pos             0
#define     _RX_Bandwidth_Mask            0x7
#define     _TX_Bandwidth_Pos             4
#define     _TX_Bandwidth_Mask            0x7
#define RT8326_PORT_CTL(i)                (RT8326_PORT_CTL_BASE + 0x0001 * (i >> 1))
#define     RX_Bandwidth_Val(i, val)      (((val) << _RX_Bandwidth_Pos) << (((i) & (_Port_Ctl_Reg_Packed-1)) * _Port_Ctl_Reg_Width))
#define     TX_Bandwidth_Val(i, val)      (((val) << _TX_Bandwidth_Pos) << (((i) & (_Port_Ctl_Reg_Packed-1)) * _Port_Ctl_Reg_Width))
#define     RX_Bandwidth_Mask(i)          ((_RX_Bandwidth_Mask << _RX_Bandwidth_Pos) << (((i) & (_Port_Ctl_Reg_Packed-1)) * _Port_Ctl_Reg_Width))
#define     TX_Bandwidth_Mask(i)          ((_TX_Bandwidth_Mask << _TX_Bandwidth_Pos) << (((i) & (_Port_Ctl_Reg_Packed-1)) * _Port_Ctl_Reg_Width))
#define     RX_Bandwidth_Pos(i)            ((_RX_Bandwidth_Pos) + (((i) & (_Port_Ctl_Reg_Packed-1)) * _Port_Ctl_Reg_Width))    /* added by dushb */
#define     TX_Bandwidth_Pos(i)            ((_TX_Bandwidth_Pos) + (((i) & (_Port_Ctl_Reg_Packed-1)) * _Port_Ctl_Reg_Width))

#define RT8326_EEPROM_RW_CTL              0x0217   /*added by dushb */
#define     _Eeprom_Addr_Pos              0
#define     _Eeprom_Chip_Pos              8
#define     _Eeprom_RW_Pos                11
#define     _Eeprom_Busy_Pos              12
#define     _Eeprom_Result_Pos            13
#define RT8326_EEPROM_RW_DATA             0x0218   /*added by dushb */
#define     _Eeprom_Write_Pos             0
#define     _Eeprom_Read_Pos              8

#define RT8326_PORT_MIRROR_BASE           0x0219
#define     _Port_Mirror_Reg_Width        1
#define     _Port_Mirror_Reg_Packed       16
#define     _Port_Mirror_Pos              0
#define     _Port_Mirror_Mask             0x1
#define RT8326_PORT_MIRROR(i)             (RT8326_PORT_MIRROR_BASE + 0x0001 * (i > 15))
#define     Port_Mirror_Val(i,val)        (((val) << _Port_Mirror_Pos) << (((i) & (_Port_Mirror_Reg_Packed-1)) * _Port_Mirror_Reg_Width))
#define     Port_Mirror_Mask(i)           ((_Port_Mirror_Mask << _Port_Mirror_Pos) << (((i) & (_Port_Mirror_Reg_Packed-1)) * _Port_Mirror_Reg_Width))
#define     Port_Mirror_Pos(i)             ((_Port_Mirror_Pos) + (((i) & (_Port_Mirror_Reg_Packed-1)) * _Port_Mirror_Reg_Width))   /*added by dushb*/

#define RT8326_RX_MIRROR_BASE             0x021B
#define     _RX_Mirror_Reg_Width          1
#define     _RX_Mirror_Reg_Packed         16
#define     _RX_Mirror_Pos                0
#define     _RX_Mirror_Mask               0x1
#define RT8326_RX_MIRROR(i)               (RT8326_RX_MIRROR_BASE + 0x0001 * (i > 15))
#define     RX_Mirror_Val(i,val)          (((val) << _RX_Mirror_Pos) << (((i) & (_RX_Mirror_Reg_Packed-1)) * _RX_Mirror_Reg_Width))
#define     RX_Mirror_Mask(i)             ((_RX_Mirror_Mask << _RX_Mirror_Pos) << (((i) & (_RX_Mirror_Reg_Packed-1)) * _RX_Mirror_Reg_Width))
#define     RX_Mirror_Pos(i)               ((_RX_Mirror_Pos) + (((i) & (_TX_Mirror_Reg_Packed-1)) * _TX_Mirror_Reg_Width))   /*added by dushb*/

#define RT8326_TX_MIRROR_BASE             0x021D
#define     _TX_Mirror_Reg_Width          1
#define     _TX_Mirror_Reg_Packed         16
#define     _TX_Mirror_Pos                0
#define     _TX_Mirror_Mask               0x1
#define RT8326_TX_MIRROR(i)               (RT8326_TX_MIRROR_BASE + 0x0001 * (i > 15))
#define     TX_Mirror_Val(i,val)          (((val) << _TX_Mirror_Pos) << (((i) & (_TX_Mirror_Reg_Packed-1)) * _TX_Mirror_Reg_Width))
#define     TX_Mirror_Mask(i)             ((_TX_Mirror_Mask << _TX_Mirror_Pos) << (((i) & (_TX_Mirror_Reg_Packed-1)) * _TX_Mirror_Reg_Width))
#define     TX_Mirror_Pos(i)              ((_TX_Mirror_Pos) + (((i) & (_TX_Mirror_Reg_Packed-1)) * _TX_Mirror_Reg_Width))   /*added by dushb*/



/*
 * Address Lookup Table Control Registers
 */
#define RT8326_ALT_CONFIG                 0x0300
#define     _ALT_AGING_POS                0
#define     _ALT_FAST_AGING_POS           1
#define     _ALT_CTRL_FILTER_POS          2

#define RT8326_ALT_LEARN_BASE             0x0301
#define     _ALT_Learn_Reg_Width          1
#define     _ALT_Learn_Reg_Packed         16
#define     _ALT_Learn_Pos                1
#define     _ALT_Learn_Mask               0x0001
#define RT8326_ALT_LEARN(i)               (RT8326_ALT_LEARN_BASE + 0x0001 * (i > 15))
#define     ALT_Learn_Mask(i)             ((_ALT_Learn_Mask) << ((i & 0x000F) * 1))
#define     ALT_Learn_POS(i)              ((_ALT_Learn_Pos) + (((i) & (_ALT_Learn_Reg_Packed-1)) * _ALT_Learn_Reg_Width)) 
//#define RT8326_INTERNAL_0               0x0303
//#define RT8326_INTERNAL_1               0x0304
//#define RT8326_INTERNAL_2               0x0305
//#define RT8326_INTERNAL_3               0x0306

#define RT8326_PORT_TRUNK_BASE            0x0307
#define     _Trunk_Pos                    0x1
#define     _Trunk_Pos_1                  0
#define     _Trunk_Mask                   0x1
#define RT8326_PORT_TRUNK(i)              RT8326_PORT_TRUNK_BASE
#define     Trunk_Val(i,val)              (((val) << _Trunk_Pos) << (i >> 2))

#define     Trunk_Val_1(i,val)            (((val) << _Trunk_Pos_1) << (i >> 1))
#define     Trunk_Mask(i)                 ((_Trunk_Mask << _Trunk_Pos) << (i >> 2))
#define     Trunk_Mask_1(i)               ((_Trunk_Mask << _Trunk_Pos_1) << (i >> 1))
#define     Trunk_Bit(i)                  (i >> 2) 
#define     Trunk_Bit_1(i)                (i >> 1)
#define     Trunk_Index_Val(val, i)       (val & ((0x1 << (i >> 2)) << _Trunk_Pos))
#define     Trunk_Index_Val_1(val, i)     (val & ((0x1 << (i >> 1)) << _Trunk_Pos_1))

/* FOR 8326 */
//#define     Trunk_Val(i,val)              (i >> 2) ? ((val) << ((i >> 1) + 2) / 2) : ((val) << (i >> 1))
//#define     Trunk_Mask(i)                 (i >> 2) ? (_Trunk_Mask << ((i >> 1) + 2) / 2) : (_Trunk_Mask << (i >> 1))


#define RT8326_IGMP_SNOOP_BASE            0x0308
#define     _IGMP_Snoop_Enabled_Pos       0
#define     _IGMP_Snoop_Enabled_Mask      0x1
#define RT8326_IGMP_SNOOP                 RT8326_IGMP_SNOOP_BASE
#define     IGMP_Snoop_Enabled_Val(val)   ((val) << _IGMP_Snoop_Enabled_Pos)
#define     IGMP_Snoop_Enabled_Mask       (_IGMP_Snoop_Enabled_Mask << _IGMP_Snoop_Enabled_Pos)


#define RT8326_ROUTER_PORT_BASE           0x0309
#define     _Router_Port_Reg_Width        1
#define     _Router_Port_Reg_Packed       32
#define     _Router_Port_Pos              0
#define     _Router_Port_Mask             0x1
#define RT8326_ROUTER_PORT(i)             RT8326_ROUTER_PORT_BASE
#define     Router_Port_Val(i, val)       ((val) << _Router_Port_Pos)
#define     Router_Port_Mask(i)           ((_Router_Port_Mask << _Router_Port_Pos) << (((i) & (_Router_Port_Reg_Packed-1)) * _Router_Port_Reg_Width))


//#define RT8326_RESERVED                 0x030A

#define RT8326_VLAN_CTL_BASE              0x030B
#define     _EnVlan_Pos                   0
#define     _EnVlan_Mask                  0x1
#define     _EnUCleaky_Pos                1
#define     _EnUCleaky_Mask               0x1
#define     _EnARPleaky_Pos               2
#define     _EnARPleaky_Mask              0x1
#define     _EnIPMleaky_Pos               3
#define     _EnIPMleaky_Mask              0x1
#define     _En8021Qaware_Pos             4
#define     _En8021Qaware_Mask            0x1
#define     _EnIR_TagAdmit_Pos            5
#define     _EnIR_TagAdmit_Mask           0x1
#define     _EnIR_Membset_Pos             6
#define     _EnIR_Membset_Mask            0x1
#define     _EnPortIsolate_Pos               7             /* added by dushb */
#define     _EnPortIsolate_Maxk             0x1
#define RT8326_VLAN_CTL                   RT8326_VLAN_CTL_BASE
#define     EnVlan_Val(val)               ((val) << _EnVlan_Pos)
#define     EnUCleaky_Val(val)            ((val) << _EnUCleaky_Pos)
#define     EnARPleaky_Val(val)           ((val) << _EnARPleaky_Pos)
#define     EnIPMleaky_Val(val)           ((val) << _EnIPMleaky_Pos)
#define     En8021Qaware_Val(val)         ((val) << _En8021Qaware_Pos)
#define     EnIR_TagAdmit_Val(val)        ((val) << _EnIR_TagAdmit_Pos)
#define     EnIR_Membset_Val(val)         ((val) << _EnIR_Membset_Pos)
#define     EnVlan_Mask                   (_EnVlan_Mask << _EnVlan_Pos)
#define     EnUCleaky_Mask                (_EnUCleaky_Mask << _EnUCleaky_Pos)
#define     EnARPleaky_Mask               (_EnARPleaky_Mask << _EnARPleaky_Pos)
#define     EnIPMleaky_Mask               (_EnIPMleaky_Mask << _EnIPMleaky_Pos)
#define     En8021Qaware_Mask             (_En8021Qaware_Mask << _En8021Qaware_Pos)
#define     EnIR_TagAdmit_Mask            (_EnIR_TagAdmit_Mask << _EnIR_TagAdmit_Pos)
#define     EnIR_Membset_Mask             (_EnIR_Membset_Mask << _EnIR_Membset_Pos)


#define RT8326_VLAN_IDX_BASE              0x030C
#define     _Vlan_IDX_Reg_Width           8
#define     _Vlan_IDX_Reg_Packed          2
#define     _VID_Index_Pos                0
#define     _VID_Index_Mask               0x1F
#define RT8326_VLAN_IDX(i)                (RT8326_VLAN_IDX_BASE + 0x0001 * (i >> 1))
#define     VID_Index_Val(i,val)          (((val) << _VID_Index_Pos) << (((i) & (_Vlan_IDX_Reg_Packed-1)) * _Vlan_IDX_Reg_Width))
#define     VID_Index_Mask(i)             ((_VID_Index_Mask << _VID_Index_Pos) << (((i) & (_Vlan_IDX_Reg_Packed-1)) * _Vlan_IDX_Reg_Width))
#define     VID_Index_Shift(i)            (((i) & (_Vlan_IDX_Reg_Packed-1)) * _Vlan_IDX_Reg_Width)
#define     VID_Index_Pos(i)               ((_VID_Index_Pos) + (((i) & (_Vlan_IDX_Reg_Packed-1)) * _Vlan_IDX_Reg_Width))   /*added by dushb*/

#define RT8326_VLAN_PRI_BASE              0x0319
#define     _Vlan_Pri_Reg_Width           2
#define     _Vlan_Pri_Reg_Packed          8
#define     _PriTagCtrl_Pos               0
#define     _PriTagCtrl_Mask              0x3
#define RT8326_VLAN_PRI(i)                (RT8326_VLAN_PRI_BASE + 0x0001 * (i >> 3))
#define     PriTagCtrl_Val(i,val)         (((val) << _PriTagCtrl_Pos) << (((i) & (_Vlan_Pri_Reg_Packed-1)) * _Vlan_Pri_Reg_Width))
#define     PriTagCtrl_Mask(i)            ((_PriTagCtrl_Mask << _Port_Disable_Pos) << (((i) & (_Vlan_Pri_Reg_Packed-1)) * _Vlan_Pri_Reg_Width))
#define     PriTagCtrl_Pos(i)             ((_PriTagCtrl_Pos) + (((i) & (_Vlan_Pri_Reg_Packed-1)) * _Vlan_Pri_Reg_Width))   /*added by dushb*/

#define RT8326_VLAN_PORT_MEMBER_BASE      0x031D
#define     _Vlan_Port_Member_Reg_Width   1
#define     _Vlan_Port_Member_Reg_Packed  16
#define     _IsMember_Pos                 0
#define     _IsMember_Mask                0x1
#define RT8326_VLAN_PORT_MEMBER(e, p)     (RT8326_VLAN_PORT_MEMBER_BASE + (0x0003 * e) + 0x0001 * (p))
#define     IsMember_Val(p,val)           (((val) << _IsMember_Pos) << (((p) & (_Vlan_Port_Member_Reg_Packed-1)) * _Vlan_Port_Member_Reg_Width))
#define     IsMember_Mask(p)              ((_IsMember_Mask << _IsMember_Pos) << (((p) & (_Vlan_Port_Member_Reg_Packed-1)) * _Vlan_Port_Member_Reg_Width))


#define RT8326_VLAN_VID_BASE              0x031F
#define     _Vlan_VID_Reg_Width           12
#define     _Vlan_VID_Reg_Packed          1
#define     _Vlan_VID_Pos                 0
#define     _Vlan_VID_Mask                0x0FFF
#define RT8326_VLAN_VID(e, p)             (RT8326_VLAN_VID_BASE + (0x0003 * e))
#define     Vlan_VID_Val(p,val)           (((val) << _Vlan_VID_Pos) << (((p) & (_Vlan_VID_Reg_Packed-1)) * _Vlan_VID_Reg_Width))
#define     Vlan_VID_Mask(p)              ((_Vlan_VID_Mask << _Vlan_VID_Pos) << (((p) & (_Vlan_VID_Reg_Packed-1)) * _Vlan_VID_Reg_Width))


#define RT8326_VLAN_PVID_CTL_BASE         0x037D
#define     _Vlan_PVID_CTL_Reg_Width      1
#define     _Vlan_PVID_CTL_Reg_Packed     16
#define     _PVID_Enabled_Pos             0
#define     _PVID_Enabled_Mask            0x1
#define RT8326_VLAN_PVID_CTL(i)           (RT8326_VLAN_PVID_CTL_BASE + 0x0001 * (i > 15))
#define     PVID_Enabled_Val(i,val)       (((val) << _PVID_Enabled_Pos) << (((i) & (_Vlan_PVID_CTL_Reg_Packed-1)) * _Vlan_PVID_CTL_Reg_Width))
#define     PVID_Enabled_Mask(i)          ((_PVID_Enabled_Mask << _PVID_Enabled_Pos) << (((i) & (_Vlan_PVID_CTL_Reg_Packed-1)) * _Vlan_PVID_CTL_Reg_Width))


/*
 * Queue Control Registers
 */
#define RT8326_QOS_CTL_BASE               0x0400
#define     _DiffServ_Pri_Pos             0
#define     _DiffServ_Pri_Mask            0x1
#define     _TagVlan_Pri_Pos              1
#define     _TagVlan_Pri_Mask             0x1
#define     _FlowCtrl_Auto_Pos            2
#define     _FlowCtrl_Auto_Mask           0x1
#define     _HighLow_Ratio_Pos            3
#define     _HighLow_Ratio_Mask           0x3
#define RT8326_QOS_CTL                    RT8326_QOS_CTL_BASE
#define     DiffServ_Pri_Val(val)         ((val) << _DiffServ_Pri_Pos)
#define     TagVlan_Pri_Val(val)          ((val) << _TagVlan_Pri_Pos)
#define     FlowCtrl_Auto_Val(val)        ((val) << _FlowCtrl_Auto_Pos)
#define     HighLow_Ratio_Val(val)        ((val) << _HighLow_Ratio_Pos)
#define     DiffServ_Pri_Mask             (_DiffServ_Pri_Mask << _DiffServ_Pri_Pos)
#define     TagVlan_Pri_Mask              (_TagVlan_Pri_Mask << _TagVlan_Pri_Pos)
#define     FlowCtrl_Auto_Mask            (_FlowCtrl_Auto_Mask << _FlowCtrl_Auto_Pos)
#define     HighLow_Ratio_Mask            (_HighLow_Ratio_Mask << _HighLow_Ratio_Pos)


#define RT8326_PORT_PRI_BASE              0x0401
#define     _Port_Pri_Reg_Width           1
#define     _Port_Pri_Reg_Packed          16
#define     _Port_Pri_Pos                 0
#define     _Port_Pri_Mask                0x1
#define RT8326_PORT_PRI(i)                (RT8326_PORT_PRI_BASE + 0x0001 * (i > 15))
#define     Port_Pri_Val(i,val)           (((val) << _Port_Pri_Pos) << (((i) & (_Port_Pri_Reg_Packed-1)) * _Port_Pri_Reg_Width))
#define     Port_Pri_Mask(i)              ((_Port_Pri_Mask << _Port_Pri_Pos) << (((i) & (_Port_Pri_Reg_Packed-1)) * _Port_Pri_Reg_Width))
#define     Port_Pri_Pos(i)              ((_Port_Pri_Pos) + (((i) & (_Port_Pri_Reg_Packed-1)) * _Port_Pri_Reg_Width)) 
/*
 * PHY Access Control Registers
 */
#define RT8326_ACCESS_CTL                 0x0500
#define     PHY_Register_Pos                   0              /* added by dushb */
#define     PHY_Address_Pos                   5
#define     PHY_RW_Pos                         14
#define     PHY_Busy_Pos                       15
#define RT8326_ACCESS_WRITE               0x0501
#define     PHY_Write_Data_Pos               0              /* added by dushb */
#define RT8326_ACCESS_READ                0x0502
#define     PHY_Read_Data_Pos                0              /* added by dushb */

/*
 * Port Control Registers
 */
//#define RT8326_RESERVED                 0x0600
//#define RT8326_RESERVED                 0x0601
//#define RT8326_RESERVED                 0x0602
//#define RT8326_RESERVED                 0x0603
//#define RT8326_RESERVED                 0x0604
//#define RT8326_RESERVED                 0x0605
//#define RT8326_RESERVED                 0x0606

#define RT8326_GLOBAL_PORT_CTL_BASE       0x0607
#define     _FlowControl_FD_Pos           0
#define     _FlowControl_FD_Mask          0x1
#define     _FlowControl_HD_Pos           1
#define     _FlowControl_HD_Mask          0x1
#define     _FlowControl_BC_Pos           2
#define     _FlowControl_BC_Mask          0x1
#define     _FlowControl_MC_Pos           3
#define     _FlowControl_MC_Mask          0x1
#define     _Filter_BC_Storm_Pos          4
#define     _Filter_BC_Storm_Mask         0x1
#define RT8326_GLOBAL_PORT_CTL            RT8326_GLOBAL_PORT_CTL_BASE
#define     FlowControl_FD_Val(val)       ((val) << _FlowControl_FD_Pos)
#define     FlowControl_HD_Val(val)       ((val) << _FlowControl_HD_Pos)
#define     FlowControl_BC_Val(val)       ((val) << _FlowControl_BC_Pos)
#define     FlowControl_MC_Val(val)       ((val) << _FlowControl_MC_Pos)
#define     Filter_BC_Storm_Val(val)      ((val) << _Filter_BC_Storm_Pos)
#define     FlowControl_FD_Mask           (_FlowControl_FD_Mask << _FlowControl_FD_Pos)
#define     FlowControl_HD_Mask           (_FlowControl_HD_Mask << _FlowControl_HD_Pos)
#define     FlowControl_BC_Mask           (_FlowControl_BC_Mask << _FlowControl_BC_Pos)
#define     FlowControl_MC_Mask           (_FlowControl_MC_Mask << _FlowControl_MC_Pos)
#define     Filter_BC_Storm_Mask          (_Filter_BC_Storm_Mask << _Filter_BC_Storm_Pos)


// OK
#define RT8326_PORT_DISABLE_BASE          0x0608
#define     _Port_Disable_Reg_Width       1
#define     _Port_Disable_Reg_Packed      16
#define     _Port_Disable_Pos             0
#define     _Port_Disable_Mask            0x1
#define RT8326_PORT_DISABLE(i)            (RT8326_PORT_DISABLE_BASE + 0x0001 * (i > 15))
#define     Port_Disable_Val(i,val)       (((val) << _Port_Disable_Pos) << (((i) & (_Port_Disable_Reg_Packed-1)) * _Port_Disable_Reg_Width))
#define     Port_Disable_Mask(i)          ((_Port_Disable_Mask << _Port_Disable_Pos) << (((i) & (_Port_Disable_Reg_Packed-1)) * _Port_Disable_Reg_Width))
#define     Port_Disable_Pos(i)            ((_Port_Disable_Pos) + (((i) & (_Port_Disable_Reg_Packed- 1)) * _Port_Disable_Reg_Width))   /* added by dushb */

#define RT8326_PORT_PROPERTY_BASE         0x060A
#define     _Port_Property_Reg_Width      8
#define     _Port_Property_Reg_Packed     2
#define     _Media_Capability_Pos         0
#define     _Media_Capability_Mask        0x1F
#define     _Pause_Pos                    5
#define     _Pause_Mask                   0x1
#define     _AsyPause_Pos                 6
#define     _AsyPause_Mask                0x1
#define     _AutoNegotiation_Pos          7
#define     _AutoNegotiation_Mask         0x1
#define RT8326_PORT_PROPERTY(i)           (RT8326_PORT_PROPERTY_BASE + 0x0001 * (i >> 1))
#define     Media_Capability_Val(i,val)   (((val) << _Media_Capability_Pos) << (((i) & (_Port_Property_Reg_Packed-1)) * _Port_Property_Reg_Width))
#define     Pause_Val(i,val)              (((val) << _Pause_Pos) << (((i) & (_Port_Property_Reg_Packed-1)) * _Port_Property_Reg_Width))
#define     AsyPause_Val(i, val)          (((val) << _AsyPause_Pos) << ((i & (_Port_Property_Reg_Packed-1)) * _Port_Property_Reg_Width))
#define     AutoNegotiation_Val(i, val)   (((val) << _AutoNegotiation_Pos) << ((i & (_Port_Property_Reg_Packed-1)) * _Port_Property_Reg_Width))
#define     Media_Capability_Mask(i)      ((_Media_Capability_Mask << _Media_Capability_Pos) << (((i) & (_Port_Property_Reg_Packed-1)) * _Port_Property_Reg_Width))
#define     Pause_Mask(i)                 ((_Pause_Mask << _Pause_Pos) << (((i) & (_Port_Property_Reg_Packed-1)) * _Port_Property_Reg_Width))
#define     AsyPause_Mask(i)              ((_AsyPause_Mask << _AsyPause_Pos) << ((i & (_Port_Property_Reg_Packed-1)) * _Port_Property_Reg_Width))
#define     AutoNegotiation_Mask(i)       ((_AutoNegotiation_Mask << _AutoNegotiation_Pos) << ((i & (_Port_Property_Reg_Packed-1)) * _Port_Property_Reg_Width))
#define     Media_Capability_Shift(i)     ((((i) & (_Port_Property_Reg_Packed-1)) * _Port_Property_Reg_Width) + _Media_Capability_Pos)
#define     Media_Capability_Pos(i)       ((_Media_Capability_Pos) + (((i) & (_Port_Property_Reg_Packed- 1)) * _Port_Status_Reg_Width))    /* added by dushb */
#define     Pause_Pos(i)                     ((_Pause_Pos) + (((i) & (_Port_Property_Reg_Packed- 1)) * _Port_Status_Reg_Width))
#define     AsyPause_Pos(i)                ((_AsyPause_Pos) + (((i) & (_Port_Property_Reg_Packed- 1)) * _Port_Status_Reg_Width))
#define     AutoNegotiation_Pos(i)              ((_AutoNegotiation_Pos) + (((i) & (_Port_Property_Reg_Packed- 1)) * _Port_Status_Reg_Width))

//#define RT8326_RESERVED                 0x0617
//#define RT8326_RESERVED                 0x0618

// OK
#define RT8326_PORT_STATUS_BASE           0x0619
#define     _Port_Status_Reg_Width        8
#define     _Port_Status_Reg_Packed       2
#define     _Link_Status_Pos              0
#define     _Link_Status_Mask             0x7
#define     _Link_Speed_Pos               0
#define     _Link_Speed_Mask              0x3
#define     _Full_Duplex_Pos              2
#define     _Full_Duplex_Mask             0x1
#define     _Link_Up_Pos                  4
#define     _Link_Up_Mask                 0x1
#define     _Flow_Ctl_En_Pos              5
#define     _Flow_Ctl_En_Mask             0x1
#define     _Asy_Pause_En_Pos             6
#define     _Asy_Pause_En_Mask            0x1
#define     _Auto_Negotiation_En_Pos      7
#define     _Auto_Negotiation_En_Mask     0x1
#define RT8326_PORT_STATUS(i)             (RT8326_PORT_STATUS_BASE + 0x0001 * (i >> 1))
#define     Link_Speed_Val(i,val)         (((val) << _Link_Speed__Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Full_Duplex_Val(i,val)        (((val) << _Full_Duplex_Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Link_Up_Val(i,val)            (((val) << _Link_Up_Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Flow_Ctl_En_Val(i,val)        (((val) << _Flow_Ctl_En_Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Asy_Pause_En_Val(i,val)       (((val) << _Asy_Pause_En_Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Auto_Negotiation_En_Val(i,val)(((val) << _Auto_Negotiation_En_Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Link_Status_Mask(i)           ((_Link_Status_Mask << _Link_Status_Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Link_Speed_Mask(i)            ((_Link_Speed_Mask << _Link_Speed_Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Full_Duplex_Mask(i)           ((_Full_Duplex_Mask << _Full_Duplex_Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Link_Up_Mask(i)               ((_Link_Up_Mask << _Link_Up_Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Flow_Ctl_En_Mask(i)           ((_Flow_Ctl_En_Mask << _Flow_Ctl_En_Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Asy_Pause_En_Mask(i)          ((_Asy_Pause_En_Mask << _Asy_Pause_En_Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Auto_Negotiation_En_Mask(i)   ((_Auto_Negotiation_En_Mask << _Auto_Negotiation_En_Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Link_Status_Shift(i)          ((((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width) + _Link_Status_Pos)
#define     Link_Speed_Shift(i)           ((((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width) + _Link_Speed_Pos)
#define     Full_Duplex_Shift(i)          ((((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width) + _Full_Duplex_Pos)
#define     Link_Up_Shift(i)              ((((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width) + _Link_Up_Pos)
#define     Link_Status_Pos(i)      ((_Link_Status_Pos) + (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))          /* added by dushb */
#define     Link_Speed_Pos(i)       ((_Link_Speed_Pos) + (((i) & (_Port_Status_Reg_Packed- 1)) * _Port_Status_Reg_Width))
#define     Full_Duplex_Pos(i)       ((_Full_Duplex_Pos) + (((i) & (_Port_Status_Reg_Packed- 1)) * _Port_Status_Reg_Width))
#define     Link_Up_Pos(i)            ((_Link_Up_Pos) + (((i) & (_Port_Status_Reg_Packed- 1)) * _Port_Status_Reg_Width))
#define     Flow_Ctl_En_Pos(i)      ((_Flow_Ctl_En_Pos) + (((i) & (_Port_Status_Reg_Packed- 1)) * _Port_Status_Reg_Width))
#define     Asy_Pause_En_Pos(i)          ((_Asy_Pause_En_Pos) + (((i) & (_Port_Status_Reg_Packed- 1)) * _Port_Status_Reg_Width))
#define     Auto_Negotiation_En_Pos(i)     ((_Auto_Negotiation_En_Pos) + (((i) & (_Port_Status_Reg_Packed- 1)) * _Port_Status_Reg_Width))
/*
 * MIB Counter Registers
 */
#define RT8326_MIB_OBJ_SEL_BASE           0x0700
#define     _MIB_Obj_Sel_Reg_Width        6
#define     _MIB_Obj_Sel_Reg_Packed       2
#define     _RX_MIB_OBJ_Pos               0
#define     _RX_MIB_OBJ_Mask              0x3
#define     _TX_MIB_OBJ_Pos               2
#define     _TX_MIB_OBJ_Mask              0x3
#define     _DN_MIB_OBJ_Pos               4
#define     _DN_MIB_OBJ_Mask              0x3
#define RT8326_MIB_OBJ_SEL(i)             (RT8326_MIB_OBJ_SEL_BASE + 0x0001 * (i >> 1))
#define     RX_MIB_OBJ_Val(i,val)         (((val) << _RX_MIB_OBJ_Pos) << (((i) & (_MIB_Obj_Sel_Reg_Packed-1)) * _MIB_Obj_Sel_Reg_Width))
#define     TX_MIB_OBJ_Val(i,val)         (((val) << _TX_MIB_OBJ_Pos) << (((i) & (_MIB_Obj_Sel_Reg_Packed-1)) * _MIB_Obj_Sel_Reg_Width))
#define     DN_MIB_OBJ_Val(i,val)         (((val) << _DN_MIB_OBJ_Pos) << (((i) & (_MIB_Obj_Sel_Reg_Packed-1)) * _MIB_Obj_Sel_Reg_Width))
#define     RX_MIB_OBJ_Mask(i)            ((_RX_MIB_OBJ_Mask << _RX_MIB_OBJ_Pos) << (((i) & (_MIB_Obj_Sel_Reg_Packed-1)) * _MIB_Obj_Sel_Reg_Width))
#define     TX_MIB_OBJ_Mask(i)            ((_TX_MIB_OBJ_Mask << _TX_MIB_OBJ_Pos) << (((i) & (_MIB_Obj_Sel_Reg_Packed-1)) * _MIB_Obj_Sel_Reg_Width))
#define     DN_MIB_OBJ_Mask(i)            ((_DN_MIB_OBJ_Mask << _DN_MIB_OBJ_Pos) << (((i) & (_MIB_Obj_Sel_Reg_Packed-1)) * _MIB_Obj_Sel_Reg_Width))
#define     RX_MIB_OBJ_Pos(i)               ((_RX_MIB_OBJ_Pos) + (((i) & (_MIB_Obj_Sel_Reg_Packed-1)) * _MIB_Obj_Sel_Reg_Width))    /* added by dushb */
#define     TX_MIB_OBJ_Pos(i)               ((_TX_MIB_OBJ_Pos) + (((i) & (_MIB_Obj_Sel_Reg_Packed-1)) * _MIB_Obj_Sel_Reg_Width))
#define     DN_MIB_OBJ_Pos(i)               ((_DN_MIB_OBJ_Pos) + (((i) & (_MIB_Obj_Sel_Reg_Packed-1)) * _MIB_Obj_Sel_Reg_Width))

/*
 * Port MIB Counter 1 Registers (RX Counter)
 */
#define RT8326_MIB_RX_CNT_BASE            0x070D
#define RT8326_MIB_RX_CNT(i)              (RT8326_MIB_RX_CNT_BASE + 0x0001 * i)


/*
 * Port MIB Counter 2 Registers (TX Counter)
 */
#define RT8326_MIB_TX_CNT_BASE            0x0727
#define RT8326_MIB_TX_CNT(i)              (RT8326_MIB_TX_CNT_BASE + 0x0001 * i)


/*
 * Port MIB Counter 1 Registers (Diagnostics Counter)
 */
#define RT8326_MIB_DN_CNT_BASE            0x0741
#define RT8326_MIB_DN_CNT(i)              (RT8326_MIB_DN_CNT_BASE + 0x0001 * i)


/*
 * System Parameter Register (Reserved)
 */
//#define RT8326_SYS_PARAMETER            0xFFFF

/*
 * Bit Operation
 */
 
#define Get_Bit(val, i)                   (((val) & (1 << (i))) >> (i))
extern unsigned int AdvSpeed;
extern unsigned int TrunkDlg_flag;
//extern int SelectedSwitch;

/*
 * GWD added
 */

#define RCP_EEPROM_BASIC_ADDR                 0x400     /* product information basic address */
#define RCP_MAC_BASIC_ADDR                      0x12
#undef IN
#define IN
#undef OUT
#define OUT

typedef enum {
    RCP_SPEED_10M_HALF = 0x1,
    RCP_SPEED_10M_FULL = 0x2,
    RCP_SPEED_100M_HALF = 0x4,
    RCP_SPEED_100M_FULL = 0x8,
}RCP_PORT_SPEED;


typedef enum {
    RCP_NO_LIMIT = 0,
    RCP_128K,
    RCP_256K,
    RCP_512K,
    RCP_1M,
    RCP_2M,
    RCP_4M,
    RCP_8M
}RCP_RX_RATE,RCP_TX_RATE;

typedef enum {
    RCP_CONFIG_2_REGISTER = 1,
    RCP_CONFIG_2_EEPROM   = 2,
    RCP_CONFIG_2_BOTH     = 3
}RCP_FLAG;

#define	RCP_MAC_SIZE					6
#define RCP_SIZE_MAN					30
#define MAX_RRCP_SWITCH_TO_MANAGE		5
#define MAX_RTL_REG_TO_ACCESS			10
#define MAX_RCP_SWITCH_TO_MANAGE		24
#define MAX_RCP_VLAN_NUM                32
#define MAX_RCP_PORT_NUM                32
#define RCP_PKT_MAX_LENGTH				80
#define RCP_RESPONSE_TIMEOUT			500
#define RCP_HELLO_PAYLOAD_OFFSET		(RCP_MAC_SIZE + RCP_MAC_SIZE + 4 + 4)
#define RCP_HELLO_CHIPID_OFFSET			(RCP_HELLO_PAYLOAD_OFFSET + 10)

#define RCP_ALARM_STATUS_REGISTER		0x01
#define RCP_ALARM_STATUS_RE_REGISTER	0x02
#define RCP_ALARM_STATUS_OFFLINE		0x03
#define RCP_SW_MAX_PORT 33

#define ONU_UNI_PORT_FILST 1
#define ONU_UNI_PORT_MAX 4


#define ONU_PAREANT_PORT_MAX 4
#define ONU_PAREANT_PORT_LOW 1

/* Define the different device type that may exist in system */
typedef enum
{
    GH1508		= 0,
    GH1516		= 1,
    GH1524  	= 2,  /*for GH1524*/
    GH1532  	= 3,  /*for GH1532*/
    MAX_RCP_DEV_ID
}RCP_DEVICE_ID;

typedef enum
{
	RTL_8208OR8316       	= 0xc,
	RTL_8324_FOR_GH1524		= 0xd,
	RTL_8324_FOR_GH1532     = 0xe,
	UNKNOWN_CHIP_ID
}RCP_CHIP_ID;
/*Define the chip id for rcp device register authentication*/

typedef struct _RCP_DEV RCP_DEV;

typedef int (*FRCP_READ_REG)(
				RCP_DEV*  dev,
				unsigned short regAddr, 
				unsigned short* data);

typedef int (*FRCP_WRITE_REG)(
				RCP_DEV*   dev,
				unsigned short regAddr, 
				unsigned short data);

#if 0
typedef void (*RCP_SEM_CREATE)(
						cyg_sem_t *semId, int countVal);
typedef void (*RCP_SEM_DELETE)(
						cyg_sem_t *semId);
typedef int (*RCP_SEM_TAKE)(
						cyg_sem_t *semId, unsigned long long timOut);
typedef void (*RCP_SEM_GIVE)(
						cyg_sem_t *semId);
#else
typedef void (*RCP_SEM_CREATE)(
						unsigned int  *semId, unsigned char *name, int initVal);
typedef void (*RCP_SEM_DELETE)(
						unsigned int semId);
typedef int (*RCP_SEM_TAKE)(
						unsigned int semId, unsigned long timOut);
typedef void (*RCP_SEM_GIVE)(
						unsigned int semId);
#endif
						
typedef int (*FRCP_GET_PHYPORT)(
				RCP_DEV*   dev,
				unsigned long slot,
				unsigned long lPort,
				unsigned long *unit,
				unsigned long *phyPort);
typedef int (*FRCP_GET_LPORT)(
				RCP_DEV*   dev,
				unsigned long *slot,
				unsigned long *lPort,
				unsigned long unit,
				unsigned long phyPort);
				

struct _RCP_DEV
{
    RCP_DEVICE_ID  	deviceId;

	unsigned char 	switchMac[RCP_MAC_SIZE];
	unsigned char 	previousswitchMac[RCP_MAC_SIZE]; /*mac address of previous switch  */
	unsigned char 	parentMac[RCP_MAC_SIZE];
	unsigned char 	chipID[2];  
	unsigned char 	vendorID[4];
	unsigned char 	modelID[16];

	unsigned char 	existEeprom;
	/* For Port Authentication */
    unsigned char   authenKey[2];

    unsigned char   upLinkPort;
    unsigned long   previousUplinkPort;
    unsigned char   parentPort;
    unsigned char   paSlot;	/* parentPort's slot id */
    unsigned char   paPort; /* parentPort's port id */
    unsigned long   loopStatus;
    unsigned long   loopAndDown;/*port looped and shut down */
	unsigned char   lpbDetIncapableFlag;
    unsigned char   onlineStatus;
    unsigned char   previousOnlineStatus;
    unsigned char   alarmStatus; /*alarm type flag*/
    unsigned char   alarmMask;
    unsigned char   alarmLocalLog; 
    unsigned long   vlanPort[MAX_RCP_VLAN_NUM];
    unsigned short  vlanVid[MAX_RCP_VLAN_NUM];
    unsigned char 	vlanPortIsolate;
    
    unsigned char	devEnabled;
    unsigned char   numOfPorts;
	unsigned char	maxPorts;
    unsigned char	devStorage;
    unsigned char	timeoutCounter;
    unsigned char	timeoutFlag;
    unsigned short  mgtVid;
    unsigned short  phyAddr;
    unsigned short  oamCounter;

	unsigned long   rxalarmcount[RCP_SW_MAX_PORT];
	unsigned long   txalarmcount[RCP_SW_MAX_PORT];
	unsigned long 	dropalarmcount[RCP_SW_MAX_PORT];
    unsigned char   portlink[RCP_SW_MAX_PORT];
    unsigned long	ingresslim[RCP_SW_MAX_PORT];
    unsigned long   egresslim[RCP_SW_MAX_PORT];
    unsigned long   tm_the_last_rx[RCP_SW_MAX_PORT];
    unsigned long   tm_the_last_tx[RCP_SW_MAX_PORT];


    FRCP_READ_REG 	frcpReadReg;
    FRCP_WRITE_REG 	frcpWriteReg;

    RCP_SEM_CREATE	semCreate; 	/* create semaphore */
    RCP_SEM_DELETE	semDelete; 	/* delete the semaphore */
    RCP_SEM_TAKE	semTake;	/* try to get a semaphore */
    RCP_SEM_GIVE	semGive;	/* return semaphore */

    FRCP_GET_LPORT 		frcpPort2LPort;
    FRCP_GET_PHYPORT 	frcpLPort2Port;

    void*		appData;

};

typedef struct _RCP_REG
{
	unsigned short address;
	unsigned long value;  /*unsigned long for 32-bit register*/
	unsigned char *note;
	RCP_DEV *dev;
	unsigned short validFlag;
	unsigned int semAccess;
}RCP_REG;

typedef struct _ALARM_FIFO_ENTRY_
{
	unsigned char *MsgBody;				
	unsigned short MsgLen;				
	struct _ALARM_FIFO_ENTRY_ *pNext;	
	struct _ALARM_FIFO_ENTRY_ *pPrevious;	
}ALARM_FIFO_ENTRY;


typedef struct _ALARM_FIFO_
{
	ALARM_FIFO_ENTRY *top;				
	ALARM_FIFO_ENTRY *bottom;				
	unsigned short occupancy;								
	unsigned short lengthLimitMax;								
	unsigned short lengthLimitMin;								
}ALARM_FIFO;

typedef struct _RCP_HELLO_PAYLOAD
{
	unsigned char authKey[2];
	unsigned char downLinkPort;
	unsigned char upLinkPort;
	unsigned char upLinkMac[6];
	unsigned char chipID[2];
	unsigned char vendorID[4];
}RCP_HELLO_PAYLOAD;

typedef struct _RCP_GET_REPLY_PAYLOAD
{
	unsigned char authKey[2];
	unsigned char regAddress[2];
	unsigned char regValue[4];
}RCP_GET_REPLY_PAYLOAD;

typedef struct _RCP_DEV_INFO
{
	unsigned char maxSlot;
	unsigned char maxPortPerSlot;
}RCP_DEV_INFO;

typedef struct _RCP_LPORT_INFO
{
	unsigned char unit;
	unsigned char port;
}RCP_LPORT_INFO;

typedef struct _RCP_LPORT_MAP
{
	RCP_LPORT_INFO L2Pmap[25];
}RCP_LPORT_MAP;

typedef struct _RCP_PPORT_INFO
{
	unsigned char slot;
	unsigned char port;
}RCP_PPORT_INFO;

typedef struct _RCP_PPORT_MAP
{
	RCP_PPORT_INFO P2Lmap[25];
}RCP_PPORT_MAP;

#define RCP_MAX_PORTS_PER_UNIT	24
#define RCP_MAX_PORTS_PER_SLOT	24

#define RCP_ERROR		   		(-1)
#define RCP_OK			   		(0x00)	 /* Operation succeeded                   */
#define RCP_FAIL			   	(0x01)	 /* Operation failed                      */
#define RCP_BAD_VALUE       	(0x02)   /* Illegal value (general)               */
#define RCP_BAD_PARAM       	(0x03)   /* Illegal parameter in function called  */
#define RCP_NOT_FOUND       	(0x04)   /* Item not found                        */
#define RCP_NO_MORE         	(0x05)   /* No more items found                   */
#define RCP_NO_SUCH         	(0x06)   /* No such item                          */
#define RCP_TIMEOUT         	(0x07)   /* Time Out                              */
#define RCP_NOT_SUPPORTED   	(0x08)   /* This request is not support           */
#define RCP_ALREADY_EXIST   	(0x09)   /* Tried to create existing item         */
#define RCP_NOT_INITIALIZED 	(0x0A)   /* Tried to access not intialize item    */
#define RCP_BAD_CONFIGURATION 	(0x0B)   /* Tried to access with error config     */
#define RCP_ILLEGAL_ADDRESS 	(0x0C) 	 /* Tried to access with error address    */
#define RCP_OS_ERROR 			(0x0D) 	 /* OS call return with error             */
#define RCP_FUTURE_USE 			(0x0E) 	 /* Not implement in current version      */
#define RCP_NO_MEM 				(0x0F) 	 /* No memory      */
#define RCP_UNKOWN 				(0x10) 	 /* Unknown status      */
#define RCP_NO_RESOURCE			(0x11) 	 /* No more resource    */

#define RCP_TRANS_FAILD		 	(0x12)	 /* Operation failed                      */
#define RCP_GETREG_FAILD    	(0x13)   /* Illegal value (general)               */
#define RCP_SETREG_FAILD    	(0x14)   /* Illegal parameter in function called  */
#define RCP_GETFIELD_FAILD  	(0x15)   /* Item not found                        */
#define RCP_SETFIELD_FAILD  	(0x16)   /* No more items found                   */
#define RCP_READEE_FAILD    	(0x17)   /* No such item                          */
#define RCP_WRITEE_FAILD    	(0x18)   /* Time Out                              */
#define RCP_ILLEGAL_AUTHKEY 	(0x19) 	 /* Tried to access with error auth_key   */
#define RCP_LAST_STATUS 		(0x1A) 	 /* Last value      */

#define  REALTEK_PROTOCOL_RRCP      0x01   
#define  REALTEK_PROTOCOL_ECHO      0x02
#define  REALTEK_PROTOCOL_LOOP      0x03
#define  REALTEK_RRCP_OPCODE_HELLO  0x00
#define  REALTEK_RRCP_OPCODE_GET    0x01
#define  REALTEK_RRCP_OPCODE_SET    0x02  
#define  REALTEK_RRCP_HELLO_REPLY   0x0180
#define  REALTEK_RRCP_GET_REPLY     0x0181
#define  REALTEK_AUTHEN_KEY         0x2379
#define  REALTEK_EEPROM_WRITE       0x0000
#define  REALTEK_EEPROM_READ        0x0800   

#define  RCP_PORT_MODE_100F        0x8
#define  RCP_PORT_MODE_100H        0x4
#define  RCP_PORT_MODE_10F         0x2
#define  RCP_PORT_MODE_10H         0x1

#define  RCP_CONFIG_REGISTER     	0x01
#define  RCP_CONFIG_EEPROM       	0x02  

#define  ETH_TYPE_RRCP    0x8899      /*added by dushaobo for realtek rrcp */

/*#define GET_RCP_PKT_SHORT( _BUF )   ((((unsigned short)(((unsigned char *)(_BUF))[1])) << 8) | (((unsigned short)(((unsigned char *)(_BUF))[0])) << 0))
#define SET_SHORT_TO_RCP_PKT( _BUF, value)	\
{ \

    unsigned short _ulValue;\
    _ulValue = (unsigned short)(((((value)& 0xFF00)>>8) | (((value) & 0x00FF)<<8)));\
    VOS_MemCpy(_BUF,&_ulValue,2);\
}*/
#define GET_RCP_PKT_SHORT( _BUF )   ((((unsigned short)(((unsigned char *)(_BUF))[1])) << 8) | (((unsigned short)(((unsigned char *)(_BUF))[0])) << 0))
#define SET_SHORT_TO_RCP_PKT( _BUF, value)	\
{ \
    _BUF[1] = (char)(((value)&0xFF00)>>8); \
    _BUF[0] = (char)(((value)&0x00FF)>>0); \
}

#define GET_RCP_PKT_LONG( _BUF ) ((((unsigned long)(((unsigned char *)(_BUF))[0])) << 0) | (((unsigned long)(((unsigned char *)(_BUF))[1])) << 8) | (((unsigned long)(((unsigned char *)(_BUF))[2])) << 16) | (((unsigned long)(((unsigned char *)(_BUF))[3])) << 24))

extern unsigned char rrcpHWPortID[33];
extern unsigned char rrcpSWPortID[33];
#define RCP_LPORT_2_PORT(_lport)       (rrcpHWPortID[_lport])
#define RCP_PORT_2_LPORT(_port)        (rrcpSWPortID[_port])

void gw_rcp_sem_create(unsigned int * semid, unsigned char * name ,unsigned int initval);
void gw_rcp_sem_delete(unsigned int semid);
int gw_rcp_sem_take(unsigned int semid, unsigned int timeout);
void gw_rcp_sem_give(unsigned int semid);


//#define RCP_GET_MGTPORT_VLAN_TAG(_dev, _ulTagged) IFM_VlanPortRelationApi( IFM_ETH_CREATE_INDEX(_dev->paSlot,_dev->parentPort), _dev->mgtVid, &_ulTagged)

int RCP_Init(void);
int RCP_DevList_Update(unsigned long parentPort, char *pkt);
int RCP_RegList_Update(unsigned long parentPort, char *pkt);
int RCP_RegList_Insert(RCP_REG *reg);
RCP_REG *RCP_RegList_Search(RCP_DEV *dev, unsigned short regAddr);
int RCP_RegList_Delete(RCP_REG *reg);
int RCP_Read_Reg(RCP_DEV *dev, unsigned short regAddr, unsigned short *data);
int RCP_Read_32bit_Reg(RCP_DEV *dev, unsigned short regAddr, unsigned long *data);
int RCP_Write_Reg(RCP_DEV *dev, unsigned short regAddr, unsigned short data);
int RCP_Say_Hello(int parentPort, unsigned short broadcastVid);
int RCP_Dev_Is_Exist(unsigned long parentPort);
int RCP_Dev_Is_Valid(unsigned long parentPort);
int RCP_Dev_Status_Set(unsigned long parentPort, unsigned char up);
RCP_DEV *RCP_Get_Dev_Ptr(unsigned long parentPort);
int Rcp_ChipId_AuthCheck(char *packet);
int RCP_GetEeaddrFromReg(unsigned short regAddr, unsigned short fieldOffset, unsigned short fieldLength, unsigned short *eeAddr, unsigned char *eeOffset, unsigned char *eeLength);
int RCP_GetRegField(RCP_DEV  *rrcp_dev, unsigned short  regAddr, unsigned short  fieldOffset, unsigned short  fieldLength, unsigned short  *data);
int RCP_SetRegisterField(RCP_DEV  *rrcp_dev, unsigned short  regAddr, unsigned short  fieldOffset, unsigned short  fieldLength, unsigned short  data);
int RCP_GetEepromValue(RCP_DEV  *rrcp_dev, unsigned short  eeAddr, unsigned char  *value);
int RCP_SetEepromValue(RCP_DEV  *rrcp_dev, unsigned short  eeAddr, unsigned char  value);
int RCP_GetEepromField(RCP_DEV  *rrcp_dev, unsigned short  eeAddr, unsigned char  fieldOffset, unsigned char  fieldLen, unsigned char  *value);
int RCP_SetEepromField(RCP_DEV  *rrcp_dev, unsigned short  eeAddr, unsigned char  fieldOffset, unsigned char  fieldLen, unsigned char  value);
int RCP_SetRegField(RCP_DEV  *rrcp_dev, unsigned short  regAddr, unsigned short fieldOffset, unsigned short fieldLength, unsigned short data, RCP_FLAG flag);
int RCP_SysSWReset(RCP_DEV  *rrcp_dev);
int RCP_SysHWReset(RCP_DEV  *rrcp_dev);
int RCP_GetSysEeprom(RCP_DEV  *rrcp_dev, unsigned short *data);
int RCP_GetAuthenkey(RCP_DEV  *rrcp_dev, unsigned short *data);
int RCP_SetAuthenKey(RCP_DEV  *rrcp_dev,unsigned short data);
int RCP_GetMaxPktLength(RCP_DEV  *rrcp_dev,  unsigned short  *data);
int RCP_SetMaxPktLength(RCP_DEV  *rrcp_dev,  unsigned short data);
int RCP_GetMaxPauseCnt(RCP_DEV * rrcp_dev,  unsigned short * data);
int RCP_GetPortLink(RCP_DEV  *rrcp_dev, unsigned long  slot,  unsigned long  port, unsigned short  *data);
int RCP_GetPortSpeed(RCP_DEV  *rrcp_dev, unsigned long  slot,  unsigned long  port, unsigned short  *data);
int RCP_GetPortDuplex(RCP_DEV  *rrcp_dev, unsigned long  slot,  unsigned long  port, unsigned short  *data);
int RCP_GetPortAutoNego(RCP_DEV  *rrcp_dev, unsigned long  slot,  unsigned long  port, unsigned short  *data);
int RCP_GetPortFC(RCP_DEV  *rrcp_dev, unsigned long  slot,  unsigned long  port, unsigned short  *data);
int RCP_GetPortLoopDetect(RCP_DEV  *rrcp_dev, unsigned long  slot,  unsigned long  port, unsigned short  *data);
int RCP_GetPortTruckFault(RCP_DEV  *rrcp_dev, unsigned long  slot,  unsigned long  port, unsigned short  *data);
int RCP_GetPortRouterPort(RCP_DEV  *rrcp_dev, unsigned long  slot,  unsigned long  port, unsigned short  *data);
int RCP_GetFullDuplexFC(RCP_DEV * rrcp_dev, unsigned short * data);
int RCP_SetFullDuplexFC(RCP_DEV * rrcp_dev, unsigned short data, RCP_FLAG falg);
int RCP_GetHalfDuplexFC(RCP_DEV * rrcp_dev, unsigned short * data);
int RCP_SetHalfDuplexFC(RCP_DEV * rrcp_dev,  unsigned short data,  RCP_FLAG falg);
int RCP_GetBroadcastFC(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetBroadcastFC(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetMulticastFC(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetMulticastFC(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetBroadcastStormFilter(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetBroadcastStormFilter(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetLoopDetect(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetLoopDetect(RCP_DEV  *rrcp_dev, unsigned short  data,  RCP_FLAG  flag);
int RCP_GetIGMPSnooping(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetIGMPSnooping(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetMIBRxObject(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short  *data);
int RCP_SetMIBRxObject(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short data,  RCP_FLAG  flag);
int RCP_GetMIBTxObject(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short  *data);
int RCP_SetMIBTxObject(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short data,  RCP_FLAG  flag);
int RCP_GetMIBDnObject(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short  *data);
int RCP_SetMIBDnObject(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short data,  RCP_FLAG  flag);
int RCP_GetMIBRxCounter(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port, unsigned long  *data);
int RCP_GetMIBTxCounter(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port, unsigned long  *data);
int RCP_GetMIBDiagCounter(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port, unsigned long  *data);
int RCP_GetPortEnable(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port, unsigned short *data);
int RCP_SetPortEnable(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short data,  RCP_FLAG flag);
int RCP_GetPortSpeedConfig(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port, unsigned short *data);
int RCP_SetPortSpeedConfig(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port, unsigned short  data,  RCP_FLAG flag);
int RCP_GetPortAutoNegoEnable(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port, unsigned short *data);
int RCP_SetPortAutoNegoEnable(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short data,  RCP_FLAG flag);
int RCP_GetPortPauseFC(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short *data);
int RCP_SetPortPauseFC(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short   data,  RCP_FLAG flag);
int RCP_GetPortRxBandwidth(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  RCP_RX_RATE *data);
int RCP_SetPortRxBandwidth(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  RCP_RX_RATE data,  RCP_FLAG flag);
int RCP_GetPortTxBandwidth(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  RCP_TX_RATE *data);
int RCP_SetPortTxBandwidth(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  RCP_TX_RATE   data,  RCP_FLAG flag);
int RCP_GetPortTrucking(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short *data);
int RCP_SetPortTrucking(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetQosDiffPriority(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetQosDiffPriority(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetQosVlanPriority(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetQosVlanPriority(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetQosMapMode(RCP_DEV  *rrcp_dev,  unsigned short *data);
int RCP_GetQosAdapterFC(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetQosAdapterFC(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetQosPrioRatio(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetQosPrioRatio(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetQosPortPriority(RCP_DEV  *rrcp_dev,  unsigned long slot, unsigned long port, unsigned short  *data);
int RCP_SetQosPortPriority(RCP_DEV  *rrcp_dev,  unsigned long slot, unsigned long port,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetQosPriorityCfg(RCP_DEV  *rrcp_dev,  unsigned long  *data);
int RCP_GetVlanEnable(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetVlanEnable(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetVlanUnicastLeaky(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetVlanUnicastLeaky(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetVlanMulticastLeaky(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetVlanMulticastLeaky(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetVlanArpLeaky(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetVlanArpLeaky(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetVlan8021Q(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetVlan8021Q(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetVlanPortIsolate(RCP_DEV  *rrcp_dev,  unsigned char  *data);
int RCP_SetVlanPortIsolate(RCP_DEV  *rrcp_dev,  unsigned char  data);
int RCP_GetVlan8021QTagOnly(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetVlan8021QTagOnly(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetVlanIRMember(RCP_DEV  *rrcp_dev, unsigned short *data);
int RCP_SetVlanIRMember(RCP_DEV * rrcp_dev, unsigned short data,  RCP_FLAG flag);
int RCP_GetVlanIngressFilter(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetVlanIngressFilter(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetVlanPortIDAssignment(RCP_DEV  *rrcp_dev,  unsigned long slot, unsigned long port,  unsigned short  *data);
int RCP_SetVlanPortIDAssignment(RCP_DEV  *rrcp_dev,  unsigned long slot, unsigned long port,  unsigned short data,  RCP_FLAG  flag);
int RCP_GetVlanOutPortTag(RCP_DEV  *rrcp_dev,  unsigned long slot, unsigned long port,  unsigned short  *data);
int RCP_SetVlanOutPortTag(RCP_DEV  *rrcp_dev,  unsigned long slot, unsigned long port,  unsigned short  data,  RCP_FLAG  flag);
int RCP_GetVlanPort(RCP_DEV  *rrcp_dev,  unsigned short vlanum, unsigned long *data);
int RCP_SetVlanPort(RCP_DEV  *rrcp_dev, unsigned short vlanum, unsigned long data, RCP_FLAG flag);
int RCP_GetVlanPortExist(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short vlanum, unsigned short *data);
int RCP_SetVlanPortAdd(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short vlanum,  RCP_FLAG flag);
int RCP_SetVlanPortDelete(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short vlanum,  RCP_FLAG flag);
int RCP_GetVlanVID(RCP_DEV  *rrcp_dev,  unsigned short  vlanum,  unsigned short  *data);
int RCP_SetVlanVID(RCP_DEV  *rrcp_dev,  unsigned short  vlanum,  unsigned short  data,  RCP_FLAG  flag);
int RCP_GetVlanPVID(RCP_DEV  *rrcp_dev,   unsigned long slot,  unsigned long port, unsigned short  *data);
int RCP_SetVlanPVID(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short  data,  RCP_FLAG  flag);
int RCP_GetVlanInsertPVID(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short *data);
int RCP_SetVlanInsertPVID(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short data,  RCP_FLAG flag);
int RCP_SetVlanInsert(RCP_DEV * rrcp_dev, unsigned short data, RCP_FLAG flag);
int RCP_GetVlanIdleNum(RCP_DEV *rrcp_dev,  unsigned short *idlenum);
int RCP_GetVlanExist(RCP_DEV *rrcp_dev,  unsigned short vid,   unsigned short *isExist);
int RCP_GetVlanExistNum(RCP_DEV  *rrcp_dev,  unsigned short *totalnum);
int RCP_SetVlanMgtPortIsolate(RCP_DEV *pRcpDev, unsigned long mgtPort, RCP_FLAG flag);
int RCP_GetVlanNumFromVID(RCP_DEV *rrcp_dev,   unsigned short vid,   unsigned short *vlanum);
int RCP_GetMirrorPort(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short  *data);
int RCP_SetMirrorPort(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetMirrorDestPort(RCP_DEV  *rrcp_dev, unsigned long *data);
int RCP_SetMirrorDestPort(RCP_DEV  *rrcp_dev,unsigned long   data);
int RCP_GetMirrorRxPort(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port, unsigned short  *data);
int RCP_SetMirrorRxPort(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetMirrorRxSrcPort(RCP_DEV  *rrcp_dev, unsigned long *data);
int RCP_GetMirrorTxPort(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port, unsigned short  *data);
int RCP_SetMirrorTxPort(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetMirrorTxSrcPort(RCP_DEV  *rrcp_dev, unsigned long *data);
int RCP_GetPhyRegValue(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short phyReg,  unsigned short *data);
int RCP_SetPhyRegValue(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short phyReg,  unsigned short data);
int RCP_GetPhyRegField(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short phyReg,  unsigned char offset,  unsigned char length,  unsigned short *data);
int RCP_SetPhyRegField(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short phyReg,  unsigned char offset,  unsigned char length,  unsigned short data);
int RCP_GetPhySpeed(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short *data);
int RCP_SetPhySpeed(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short data);
int RCP_GetPhyAutoNegoEnable(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short *data);
int RCP_SetPhyAutoNegoEnable(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short data);
int RCP_SetPhyReset(RCP_DEV *rrcp_dev,  unsigned long slot,  unsigned long port);
int RCP_SetPhyReAutoNego(RCP_DEV *rrcp_dev,  unsigned long slot,  unsigned long port);
int RCP_GetPhyPowerDown(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short *data);
int RCP_SetPhyPowerDown(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short data);
int RCP_GetPhyDuplex(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short *data);
int RCP_SetPhyDuplex(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short data);
int RCP_GetPhyFC(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short *data);
int RCP_SetPhyFC(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short data);
int RCP_GetPhy10H(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short *data);
int RCP_SetPhy10H(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short data);
int RCP_GetPhy10F(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short *data);
int RCP_SetPhy10F(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short data);
int RCP_GetPhy100H(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short *data);
int RCP_SetPhy100H(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short data);
int RCP_GetPhy100F(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short *data);
int RCP_SetPhy100F(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short data);
int RCP_GetAtuLearning(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long  port,  unsigned short *data);
int RCP_GetAtuLearningList(RCP_DEV  *rrcp_dev,  unsigned short *data);
int RCP_SetAtuLearning(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long  port,  unsigned short data, RCP_FLAG  flag);
int RCP_GetAtuAgingEnable(RCP_DEV  *rrcp_dev,  unsigned short *data);
int RCP_SetAtuAgingEnable(RCP_DEV  *rrcp_dev,  unsigned short data,  RCP_FLAG flag);
int RCP_GetAtuFastAgingEnable(RCP_DEV  *rrcp_dev,  unsigned short *data);
int RCP_SetAtuFastAgingEnable(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetAtuCtrlFrameFilter(RCP_DEV  *rrcp_dev,  unsigned short  *data);
int RCP_SetAtuCtrlFrameFilter(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG  flag);
void convertOidToChar(oid * oidMac,unsigned char *charMac);
void convertMacToOid(oid * oidMac,unsigned char *charMac);
int RCP_GetDevIndexByMac(oid *mac, unsigned long *devId);
int RCP_GetValidIndex(oid *mac, unsigned long *index);
int RCP_GetFirstIndex(unsigned long *index);
int RCP_GetNextIndex(unsigned long index, unsigned long *nextIndex);
int RCP_GetPortListSNMP(RCP_DEV * rcpDev,unsigned short portList, unsigned short *newList);
int RCP_PortListToSNMP(RCP_DEV * rcpDev,unsigned long portList, unsigned char *newList);
int RCP_SNMPToPortList(RCP_DEV * rcpDev,unsigned long *portList, unsigned char *newList);
int RCP_SetPortListSNMP(RCP_DEV * rcpDev,unsigned short portList, unsigned short *newList);
int RCP_GetPortList16SNMP(unsigned short portList, unsigned short *newList);
int RCP_GetPortList8SNMP(unsigned char portList, unsigned char *newList);
int RCP_SetVlanPortIsolateEn(RCP_DEV * rcpDev, unsigned short data, unsigned long mgtPort, RCP_FLAG flag);
int RCP_GetVlanUntagPortList(RCP_DEV  *rrcp_dev,  unsigned long *portList);
int RCP_SetVlanUntagPortList(RCP_DEV  *rcpDev,  unsigned short vlanum,  unsigned long   *portList,  unsigned long  port_len,  RCP_FLAG flag);
int RCP_SetVlanTaggedPortList(RCP_DEV  *rcpDev, unsigned short vlanum,  unsigned long *portList,  unsigned long  port_len,  RCP_FLAG flag);
int RCP_SetVlanPortListDelete(RCP_DEV  *rcpDev, unsigned short vlanum, unsigned long  *portList, unsigned long  port_len, RCP_FLAG flag);
int RCP_SetVlanSave(RCP_DEV *rcpDev, unsigned short vlanum);
int RCP_SetVlanEnableSave(RCP_DEV *rcpDev, unsigned short enable);
int RCP_SetVlanDelete(RCP_DEV  *rcpDev,unsigned short vid,RCP_FLAG flag);
int RCP_GetQosPriPort(RCP_DEV  *rcpDev,unsigned long *data);
int RCP_SetConfigClear(RCP_DEV  *rcpDev);
int RCP_GetPortMauType(RCP_DEV  *rcpDev,unsigned long slot, unsigned long port,oid *mau,unsigned long *mau_len);
int RCP_SetPortMauType(RCP_DEV *dev,unsigned long slot,unsigned long port,unsigned short mau,unsigned char data,RCP_FLAG flag);
int RCP_SetVlan8021QEnable(RCP_DEV  *rcpDev,unsigned short enable,RCP_FLAG flag);
int Rcp_Get_MGT_Port(RCP_DEV *dev, unsigned long *slot, unsigned long *port);
int sendOamRcpLpbDetectNotifyMsg(unsigned char onuPort, unsigned char rcpPort, unsigned char state, unsigned short uvid,unsigned char *session);
int pushOneSwitchStatusChgMsg(unsigned char onuPort, unsigned char* switchMac, unsigned char msgType, unsigned char reason);
int popAllSwitchStatusChgMsg(void);
int saveAllRcpDevCfgToFlash(void);
int saveOneRcpDevCfgToFlash(RCP_DEV  *rcpDev);
int restoreRcpDevCfgFromFlash(RCP_DEV  *rcpDev);
int ereaseRcpDevCfgInFlash(RCP_DEV  *rcpDev, int allDev);
int IS_Flash_Data_Valid(RCP_DEV  *rcpDev);
int getRegValueInFlash(RCP_DEV  *rcpDev, unsigned short usRegAddr, unsigned short *pRegValue);
int RCP_GetVlanPortIsolate_InFlash(RCP_DEV  *rcpDev, unsigned char *data);
int RCP_GetVlanPortIsolate_InEeprom(RCP_DEV  *rrcp_dev, unsigned char *data);
int RCP_GetDeviceID_InFlash(RCP_DEV  *rcpDev, unsigned short *data);
RCP_DEV *RCP_Get_Dev_Ptr_For_Flash(unsigned long parentPort);
int rrcp_packet_handler(unsigned int srcPort, unsigned int len, unsigned char *packet);
int RcpFrameRevHandle(unsigned int  portid ,unsigned int   len, unsigned char  *frame);
int gw_Onu_Rcp_Detect_Set_FDB(unsigned char  opr);

int device_conf_write_switch_conf_to_flash(char * buf, long int length);
int device_conf_read_switch_conf_from_flash(char *buf, long int *length);

#endif /* _RRCP_H_ */

#else
#ifndef _RRCP_GW_H_

#ifndef oid
typedef unsigned long	oid;
#endif

#define _RRCP_GW_H_
#define NEW_SUPPORT_8326 1  // 0: close new function for 8326
#define NEW_FEATURE2 1      // 0: close the function of Build 06 version
#define NEW_SUPPORT_NEW_CHIP 1 // 1: Support 8324P serial chips
#define Mirrored_Patch 1
#define THREAD_CABLETESTER  1
#define THREAD_SIGNALQUALITY 1
#define NEW_INI_CONFIG 1    // 0: Doesn't read rmt_config.ini file
#define NEW_ADD_AUTHKEY 1
#define NEW_SUPPORT_RL5866F 1 // 1: Support RL5866F chip

#define FLASH_GWD_RCG_SWITCH_CFG_MAX_SIZE	(11*1024)	/*the end of the gwd rcg switch config file*/
#define FLASH_GWD_RCG_SWITCH_CFG_OFFSET		(8*1024) /*the start of the gwd rc switch config file*/

#define RCP_SW_MAX_PORT 33 

//platform added new type and micro define

#define IFM_NAME_SIZE (31)
#define ETH_FILTER_MAX_PORT_NUM_ON_SLOT     ( 4 )
#define VTY_NEWLINE "\r\n"

struct slot_port{
	//void*  onuPort;
	unsigned long	ulSlot;
	unsigned long	ulPort;
};


#define ETH_SLOTPORT_TO_PORTNO( _slot, _port )           \
          ( (  ( _slot )  - 1 ) * ( ETH_FILTER_MAX_PORT_NUM_ON_SLOT ) + (  ( _port )  ) )


#define PORTNO_TO_ETH_SLOT( _portno )           \
            ( ( ( _portno ) / ( ETH_FILTER_MAX_PORT_NUM_ON_SLOT ) ) + 1 )
#define PORTNO_TO_ETH_PORTID( _portno )         \
            ( ( _portno ) % ( ETH_FILTER_MAX_PORT_NUM_ON_SLOT ) )




#define dot3MauType10BaseTHD 		10
#define dot3MauType10BaseTFD 		11
#define dot3MauType100BaseTXHD 	15
#define dot3MauType100BaseTXFD 	16
#define dot3MauType1000BaseTXHD 	29
#define dot3MauType1000BaseTXFD 	30

//end

/*Structure*/
typedef struct SwitchInfo
{
   unsigned char name[32];
   unsigned char mac[6];
   unsigned char mac_info[18];
   int uplinkPort;
   int downlinkPort[26];
   int depth;
   struct SwitchInfo* uplinkSwitch;
   struct SwitchInfo* downlinkSwitch[26];
} SwitchInformation;

typedef struct switch_form {
   unsigned char name[32];
   unsigned char mac[6];
   unsigned char mac_info[18];
   unsigned char chip_id[2];     // 16 bit
   unsigned char chip_id_info[7];
   unsigned char vendor_id[4];   // 32 bit
   unsigned char vendor_id_info[11];
   // New Add-ed
   unsigned char chip_type;
   unsigned char model_type;
   unsigned char panel_type;
   unsigned char exist_8051;
   unsigned char exist_eeprom;
   unsigned char g1_status;
   unsigned char g2_status;
   // For Port Authentication
   unsigned char dl_port;
   unsigned char mirr_mask[26];
   unsigned short mirr_regdata1;
   unsigned short mirr_regdata2;
} switch_;


#ifdef __cplusplus
extern "C" {
#endif
/* 
 * Exported Function Declarations
 */
#if 0
__declspec(dllexport) void  RRCP_Initialize(void);
__declspec(dllexport) void  RRCP_Finalize(void);

__declspec(dllexport) int   RRCP_GetAdapterNumber(void);
__declspec(dllexport) const char* RRCP_GetAdapterDescription(unsigned int num);
__declspec(dllexport) const char* RRCP_GetAdapterMacAddress(unsigned int num);
__declspec(dllexport) const char* RRCP_GetAdapterIpAddress(unsigned int num);

__declspec(dllexport) int   RRCP_SetAdapter(unsigned int num);
__declspec(dllexport) void  RRCP_BuildLinkDataBase(void);
__declspec(dllexport) void  RRCP_ReBuildLink(void);
__declspec(dllexport) void  RRCP_CheckMirrorFunction(void);

__declspec(dllexport) int   RRCP_GetLinkVectorNumber(void);
__declspec(dllexport) const char *RRCP_GetLinkVectorDownLinkMac(unsigned int num);
__declspec(dllexport) const char *RRCP_GetLinkVectorUpLinkMac(unsigned int num);
__declspec(dllexport) int   RRCP_GetLinkVectorDownLinkPort(unsigned int num);
__declspec(dllexport) int   RRCP_GetLinkVectorUpLinkPort(unsigned int num);

__declspec(dllexport) int   RRCP_GetLinkVectorDownLinkSwitchPort(unsigned int num);
__declspec(dllexport) int   RRCP_GetLinkVectorUpLinkSwitchPort(unsigned int num);

__declspec(dllexport) int   RRCP_GetSwitchNumber(void);
__declspec(dllexport) int   RRCP_GetSwitchInfo(unsigned int num);
__declspec(dllexport) const char* RRCP_GetSwitchName(unsigned int num);
__declspec(dllexport) const char* RRCP_GetSwitchMacAddress(unsigned int num);
__declspec(dllexport) const char* RRCP_GetSwitchChipId(unsigned int num);
__declspec(dllexport) const char* RRCP_GetSwitchVendorId(unsigned int num);
__declspec(dllexport) unsigned int RRCP_GetSwitchChipType(void);
__declspec(dllexport) unsigned int RRCP_GetSwitchVersionType(void);
__declspec(dllexport) unsigned int RRCP_GetSwitchModelType(void);
__declspec(dllexport) unsigned int RRCP_GetSwitchPanelType(void);

__declspec(dllexport) unsigned int RRCP_IsSwitchHas8051(void);
__declspec(dllexport) unsigned int RRCP_IsSwitchHasEEPROM(void);
__declspec(dllexport) unsigned int RRCP_GetSwitchG1Status(void);
__declspec(dllexport) unsigned int RRCP_GetSwitchG2Status(void);

__declspec(dllexport) unsigned int RRCP_GetSwitchDlPortNumber(void);
__declspec(dllexport) unsigned int RRCP_GetSwitchMirrPortStatus(unsigned int mum);

__declspec(dllexport) unsigned int RRCP_GetSwitchFastPortNumber(void);
__declspec(dllexport) unsigned int RRCP_GetSwitchGigaPortNumber(void);
__declspec(dllexport) unsigned int RRCP_GetSwitchMaxPortNumber(void);
__declspec(dllexport) unsigned int RRCP_GetSwitchMIIPortNumber(void);

__declspec(dllexport) int   RRCP_SetSwitch(unsigned int num);
__declspec(dllexport) const char* RRCP_GetAddressValue(unsigned short address);
__declspec(dllexport) unsigned int RRCP_GetAddressValueMask(unsigned short address, unsigned int mask);

__declspec(dllexport) void  RRCP_QueueReset(void);
__declspec(dllexport) int   RRCP_QueueRequest(unsigned short address);
__declspec(dllexport) void  RRCP_QueueSend(void);
__declspec(dllexport) const char* RRCP_QueueGetData(unsigned int num);
__declspec(dllexport) unsigned int RRCP_QueueGetDataMask(unsigned int num, unsigned int mask);

__declspec(dllexport) void  RRCP_QueueUpdate(void);
__declspec(dllexport) void  RRCP_QueueSetDataMask(unsigned int num, unsigned int mask, unsigned int data);
__declspec(dllexport) int   RRCP_QueueSetRequest(unsigned short address, unsigned long data);

__declspec(dllexport) void  RRCP_SetAddressValue(unsigned short address, unsigned value);

__declspec(dllexport) unsigned short RRCP_GetPhyAddressValue(unsigned int id, unsigned int regaddr);
__declspec(dllexport) unsigned short RRCP_GetPhyAddressValueNoWait(unsigned int id, unsigned int regaddr);

__declspec(dllexport) void RRCP_SetPhyAddressValue(unsigned int id, unsigned int regaddr, unsigned value);
__declspec(dllexport) void RRCP_SetPhyAddressValueNoWait(unsigned int id, unsigned int regaddr, unsigned value);

__declspec(dllexport) unsigned short RRCP_GetEepromAddressValue(unsigned int addr);
__declspec(dllexport) void RRCP_SetEepromAddressValue(unsigned int addr, unsigned int value);



__declspec(dllexport) int RRCP_GetSwitchChipStatus(unsigned int num);
__declspec(dllexport) int RRCP_GetSwitchFastPortStatus(unsigned int num);
__declspec(dllexport) int RRCP_GetSwitchFastPortMapping(unsigned int num);
__declspec(dllexport) int RRCP_GetSwitchGigaPortStatus(unsigned int num);
__declspec(dllexport) int RRCP_GetSwitchGigaPortMapping(unsigned int num);
__declspec(dllexport) int RRCP_GetSwitchMIIPortMapping(unsigned int num);
__declspec(dllexport) int RRCP_GetSwitchMIIPortStatus(unsigned int num);

__declspec(dllexport) void RRCP_SaveConfigurationToEEPROM(void);
__declspec(dllexport) void RRCP_LoadConfigurationFromEEPROM(void);
__declspec(dllexport) void RRCP_LoadFactoryDefaultConfiguration(void);

__declspec(dllexport) void  RRCP_AddSwitchDatabaseEntry(unsigned char *name, unsigned char *addr, unsigned char *key);
__declspec(dllexport) void  RRCP_ClearSwitchDatabase(void);

__declspec(dllexport) void  RRCP_UpdateAuthKey();

__declspec(dllexport) int RRCP_GetErrorFlag(void);
__declspec(dllexport) void RRCP_ClearErrorFlag(void);

__declspec(dllexport) unsigned int RRCP_PortMaxLinkSpeed(unsigned int *pidx, int num);
__declspec(dllexport) void RRCP_QueueClearReadRequestNumber(void);
__declspec(dllexport) void RRCP_PortLinkSpeedChange(unsigned int *pidx, int num);
__declspec(dllexport) int RRCP_TagBasedVlanChecking(unsigned int *pidx, int num);
__declspec(dllexport) int RRCP_VlanIDChecking(unsigned int *pidx, int num);
__declspec(dllexport) int RRCP_PortLinkUpChecking(unsigned int *pidx, int num);
__declspec(dllexport) int RRCP_PortLinkSpeedChecking(unsigned int *vidx, unsigned int *pidx, int num);
__declspec(dllexport) int RRCP_PortLinkStatusChecking(unsigned int *vidx, unsigned int *pidx, int num);
__declspec(dllexport) unsigned int RRCP_SetMacAddress(AnsiString macAddr);

__declspec(dllexport) void RRCP_SaveSecurityKeyToEEPROM(); /*save Security Key to EEPROM*/
__declspec(dllexport) void RRCP_SetFactoryDefaultMacAddress(unsigned char* mac); //get mac address from eeprom(factory default)

__declspec(dllexport) unsigned short RRCP_GetEepromAddressValue8051(unsigned int addr);
__declspec(dllexport) void RRCP_SetEepromAddressValue8051(unsigned int addr, unsigned int value);
__declspec(dllexport) unsigned short RRCP_GetSwitchSubVer(void);
// Topology
__declspec(dllexport) void RRCP_FindTopology(void);
__declspec(dllexport) SwitchInformation *RRCP_GetRootSwitch();
__declspec(dllexport) switch_ * RRCP_GetSwitchInformaition(const char* mac_info);
__declspec(dllexport) void freeNode(SwitchInformation *node);
#endif
#ifdef __cplusplus
}
#endif
/*
 * Exported Macros
 */

#define MASK_ALL_16                       0xFFFF
#define MASK_ALL_32                       0xFFFFFFFF

/*
 * System Configuration Registers
 */
#define RT8326_SYS_RESET_CTL_BASE         0x0000
#define     _SRST_Pos                     0
#define     _SRST_Mask                    0x1
#define     _HRST_Pos                     1
#define     _HRST_Mask                    0x1
#define RT8326_SYS_RESET_CTL              RT8326_SYS_RESET_CTL_BASE
#define     SRST_Val(val)                 ((val) << _SRST_Pos)
#define     HRST_Val(val)                 ((val) << _HRST_Pos)
#define     SRST_Mask                     (_SRST_Mask << _SRST_Pos)
#define     HRST_Mask                     (_HRST_Mask << _HRST_Pos)



#define RT8326_SWITCH_CTL                 0x0001
#define    _MAX_PKT_LEN_POS              0                   /*added by dushb */
#define    _MAX_PAUSE_CNT_POS          3
#define RT8326_RX_IO_PAD                  0x0002
#define RT8326_TX_IO_PAD                  0x0003
#define RT8326_USER_GPIO                  0x0004
#define RT8326_LED_DSPLY                  0x0005


#define RT8326_8051_COMM_BASE             0x0006
#define     _RstEventCtrlFlag_Pos         0
#define     _RstEventCtrlFlag_Mask        0x1
#define     _Exist8051_Pos                1
#define     _Exist8051_Mask               0x1
#define     _EnSaveCfg2EE_Pos             2
#define     _EnSaveCfg2EE_Mask            0x1
#define     _EnDownload_Pos               3
#define     _EnDownload_Mask              0x1
#define     _EnFactoryDefault_Pos         4
#define     _EnFactoryDefault_Mask        0x1
#define     _EE_Warning_Pos               5
#define     _EE_Warning_Mask              0x1
#define     _VlanTableInit_Pos            6
#define     _VlanTableInit_Mask           0x1
#define     _MaxUserVlan_Pos              8
#define     _MaxUserVlan_Mask             0x1F
#define RT8326_8051_COMM                  RT8326_8051_COMM_BASE
#define     RstEventCtrlFlag_Val(val)     ((val) << _RstEventCtrlFlag_Pos)
#define     Exist8051_Val(val)            ((val) << _Exist8051_Pos)
#define     EnSaveCfg2EE_Val(val)         ((val) << _EnSaveCfg2EE_Pos)
#define     EnDownload_Val(val)           ((val) << _EnDownload_Pos)
#define     EnFactoryDefault_Val(val)     ((val) << _EnFactoryDefault_Pos)
#define     EE_Warning_Val(val)           ((val) << _EE_Warning_Pos)
#define     VlanTableInit_Val(val)        ((val) << _VlanTableInit_Pos)
#define     MaxUserVlan_Val(val)          ((val) << _MaxUserVlan_Pos)
#define     RstEventCtrlFlag_Mask         (_RstEventCtrlFlag_Mask << _RstEventCtrlFlag_Pos)
#define     Exist8051_Mask                (_8051Exist_Mask << _Exist8051_Pos)
#define     EnSaveCfg2EE_Mask             (_EnSaveCfg2EE_Mask << _EnSaveCfg2EE_Pos)
#define     EnDownload_Mask               (_EnDownload_Mask << _EnDownload_Pos)
#define     EnFactoryDefault_Mask         (_EnFactoryDefault_Mask << _EnFactoryDefault_Pos)
#define     EE_Warning_Mask               (_EE_Warning_Mask << _EE_Warning_Pos)
#define     VlanTableInit_Mask            (_VlanTableInit_Mask << _VlanTableInit_Pos)
#define     MaxUserVlan_Mask              (_MaxUserVlan_Mask << _MaxUserVlan_Pos)


/*
 * System Status Registers
 */
#define RT8326_BOARD_TRAP                 0x0100

#define RT8326_PORT_LOOP_DETECT_BASE      0x0101
#define     _Port_Loop_Detect_Mask        0x1
#define RT8326_PORT_LOOP_DETECT(i)        (RT8326_PORT_LOOP_DETECT_BASE)
#define     Port_Loop_Detect_Mask(i)      (_Port_Loop_Detect_Mask << i)

#define RT8326_SYS_FAULT_BASE             0x0102
#define     _Trunk_Fault_Pos              1
#define     _Trunk_Fault_Mask             0x1
#define     _Loop_Fault_Pos               2
#define     _Loop_Fault_Mask              0x1
#define     _GigaPHY_NotExist_G0_Pos      3
#define     _GigaPHY_NotExist_G0_Mask     0x1
#define     _GigaPHY_NotExist_G1_Pos      4
#define     _GigaPHY_NotExist_G1_Mask     0x1
#define     _FaultTkGroup_Pos             6  // 5
#define     _FaultTkGroup_Pos_1           5
#define     _FaultTkGroup_Mask            0x1
#define RT8326_SYS_FAULT(i)               RT8326_SYS_FAULT_BASE
#define     Trunk_Fault_Val(val)          ((val) << _Trunk_Fault_Pos)
#define     Loop_Fault_Val(val)           ((val) << _Loop_Fault_Pos)
#define     GigaPHY_NotExist_G0_Val(val)  ((val) << _GigaPHY_NotExist_G0_Pos)
#define     GigaPHY_NotExist_G1_Val(val)  ((val) << _GigaPHY_NotExist_G1_Pos)
#define     FaultTkGroup_Val(i,val)       (((val) << _FaultTkGroup_Pos) << (i >> 2))
//#define     FaultTkGroup_Val(i,val)       (i >> 2) ? (((val) << _FaultTkGroup_Pos) << ((i >> 1) + 2) / 2) : (((val) << _FaultTkGroup_Pos) << (i >> 1))
#define     Trunk_Fault_Mask              (_Trunk_Fault_Mask << _Trunk_Fault_Pos)
#define     Loop_Fault_Mask               (_Loop_Fault_Mask << _Loop_Fault_Pos)
#define     GigaPHY_NotExist_G0_Mask      (_GigaPHY_NotExist_G0_Mask << _GigaPHY_NotExist_G0_Pos)
#define     GigaPHY_NotExist_G1_Mask      (_GigaPHY_NotExist_G1_Mask << _GigaPHY_NotExist_G1_Pos)
#define     FaultTkGroup_Mask(i)          ((_FaultTkGroup_Mask << _FaultTkGroup_Pos) << (i >> 2))
#define     FaultTkGroup_Mask_1(i)        ((_FaultTkGroup_Mask << _FaultTkGroup_Pos_1) << (i >> 1))
//#define     FaultTkGroup_Mask(i)          (i >> 2) ? ((_FaultTkGroup_Mask << _FaultTkGroup_Pos) << ((i >> 1) + 2) / 2) : ((_FaultTkGroup_Mask << _FaultTkGroup_Pos) << (i >> 1))

//#define     GigaPHY_NotExist_G0           0x0008
//#define     GigaPHY_NotExist_G1           0x0010


/*
 * Management Configuration Registers
 */
#define RT8326_RRCP_CTL_BASE              0x0200
#define     _RRCP_Disabled_Pos            0
#define     _RRCP_Disabled_Mask           0x1
#define     _REcho_Disabled_Pos           1
#define     _REcho_Disabled_Mask          0x1
#define     _Loop_Fault_Detect_Pos        2
#define     _Loop_Fault_Detect_Mask       0x1
#define RT8326_RRCP_CTL                   RT8326_RRCP_CTL_BASE
#define     RRCP_Disabled_Val(val)        ((val) << _RRCP_Disabled_Pos)
#define     REcho_Disabled_Val(val)       ((val) << _REcho_Disabled_Pos)
#define     Loop_Fault_Detect_Val(val)    ((val) << _Loop_Fault_Detect_Pos)
#define     RRCP_Disabled_Mask            (_RRCP_Disabled_Mask << _RRCP_Disabled_Pos)
#define     REcho_Disabled_Mask           (_REcho_Disabled_Mask << _REcho_Disabled_Pos)
#define     Loop_Fault_Detect_Mask        (_Loop_Fault_Detect_Mask << _Loop_Fault_Detect_Pos)


#define RT8326_RRCP_SECURITY_BASE         0x0201
#define     _RRCP_Security_Reg_Width      1
#define     _RRCP_Security_Reg_Packed     16
#define     _RRCP_Security_Pos            0
#define     _RRCP_Security_Mask           0x1
#define RT8326_RRCP_SECURITY_MASK(i)      (RT8326_RRCP_SECURITY_BASE + 0x0001 * (i > 15))
#define     RRCP_Security_Val(i,val)      (((val) << _RRCP_Security_Pos) << (((i) & (_RRCP_Security_Reg_Packed-1)) * _RRCP_Security_Reg_Width))
#define     RRCP_Security_Mask(i)         ((_RRCP_Security_Mask << _RRCP_Security_Pos) << (((i) & (_RRCP_Security_Reg_Packed-1)) * _RRCP_Security_Reg_Width))

//#define RT8326_RRCP_MASK_0                0x0201
//#define RT8326_RRCP_MASK_1                0x0202


#define     RT8326_RRCP_MAC_0                 0x0203
#define     RT8326_RRCP_MAC_1                 0x0204
#define     RT8326_RRCP_MAC_2                 0x0205
#define     RT8326_RRCP_CHIPID                0x0206
#define     RT8326_RRCP_VENDORID0             0x0207
#define     RT8326_RRCP_VENDORID1             0x0208
#define     RT8326_RRCP_AUTHKEY               0x0209

// OK
#define RT8326_PORT_CTL_BASE              0x020A
#define     _Port_Ctl_Reg_Width           8
#define     _Port_Ctl_Reg_Packed          2
#define     _RX_Bandwidth_Pos             0
#define     _RX_Bandwidth_Mask            0x7
#define     _TX_Bandwidth_Pos             4
#define     _TX_Bandwidth_Mask            0x7
#define RT8326_PORT_CTL(i)                (RT8326_PORT_CTL_BASE + 0x0001 * (i >> 1))
#define     RX_Bandwidth_Val(i, val)      (((val) << _RX_Bandwidth_Pos) << (((i) & (_Port_Ctl_Reg_Packed-1)) * _Port_Ctl_Reg_Width))
#define     TX_Bandwidth_Val(i, val)      (((val) << _TX_Bandwidth_Pos) << (((i) & (_Port_Ctl_Reg_Packed-1)) * _Port_Ctl_Reg_Width))
#define     RX_Bandwidth_Mask(i)          ((_RX_Bandwidth_Mask << _RX_Bandwidth_Pos) << (((i) & (_Port_Ctl_Reg_Packed-1)) * _Port_Ctl_Reg_Width))
#define     TX_Bandwidth_Mask(i)          ((_TX_Bandwidth_Mask << _TX_Bandwidth_Pos) << (((i) & (_Port_Ctl_Reg_Packed-1)) * _Port_Ctl_Reg_Width))
#define     RX_Bandwidth_Pos(i)            ((_RX_Bandwidth_Pos) + (((i) & (_Port_Ctl_Reg_Packed-1)) * _Port_Ctl_Reg_Width))    /* added by dushb */
#define     TX_Bandwidth_Pos(i)            ((_TX_Bandwidth_Pos) + (((i) & (_Port_Ctl_Reg_Packed-1)) * _Port_Ctl_Reg_Width))

#define RT8326_EEPROM_RW_CTL              0x0217   /*added by dushb */
#define     _Eeprom_Addr_Pos              0
#define     _Eeprom_Chip_Pos              8
#define     _Eeprom_RW_Pos                11
#define     _Eeprom_Busy_Pos              12
#define     _Eeprom_Result_Pos            13
#define RT8326_EEPROM_RW_DATA             0x0218   /*added by dushb */
#define     _Eeprom_Write_Pos             0
#define     _Eeprom_Read_Pos              8

#define RT8326_PORT_MIRROR_BASE           0x0219
#define     _Port_Mirror_Reg_Width        1
#define     _Port_Mirror_Reg_Packed       16
#define     _Port_Mirror_Pos              0
#define     _Port_Mirror_Mask             0x1
#define RT8326_PORT_MIRROR(i)             (RT8326_PORT_MIRROR_BASE + 0x0001 * (i > 15))
#define     Port_Mirror_Val(i,val)        (((val) << _Port_Mirror_Pos) << (((i) & (_Port_Mirror_Reg_Packed-1)) * _Port_Mirror_Reg_Width))
#define     Port_Mirror_Mask(i)           ((_Port_Mirror_Mask << _Port_Mirror_Pos) << (((i) & (_Port_Mirror_Reg_Packed-1)) * _Port_Mirror_Reg_Width))
#define     Port_Mirror_Pos(i)             ((_Port_Mirror_Pos) + (((i) & (_Port_Mirror_Reg_Packed-1)) * _Port_Mirror_Reg_Width))   /*added by dushb*/

#define RT8326_RX_MIRROR_BASE             0x021B
#define     _RX_Mirror_Reg_Width          1
#define     _RX_Mirror_Reg_Packed         16
#define     _RX_Mirror_Pos                0
#define     _RX_Mirror_Mask               0x1
#define RT8326_RX_MIRROR(i)               (RT8326_RX_MIRROR_BASE + 0x0001 * (i > 15))
#define     RX_Mirror_Val(i,val)          (((val) << _RX_Mirror_Pos) << (((i) & (_RX_Mirror_Reg_Packed-1)) * _RX_Mirror_Reg_Width))
#define     RX_Mirror_Mask(i)             ((_RX_Mirror_Mask << _RX_Mirror_Pos) << (((i) & (_RX_Mirror_Reg_Packed-1)) * _RX_Mirror_Reg_Width))
#define     RX_Mirror_Pos(i)               ((_RX_Mirror_Pos) + (((i) & (_TX_Mirror_Reg_Packed-1)) * _TX_Mirror_Reg_Width))   /*added by dushb*/

#define RT8326_TX_MIRROR_BASE             0x021D
#define     _TX_Mirror_Reg_Width          1
#define     _TX_Mirror_Reg_Packed         16
#define     _TX_Mirror_Pos                0
#define     _TX_Mirror_Mask               0x1
#define RT8326_TX_MIRROR(i)               (RT8326_TX_MIRROR_BASE + 0x0001 * (i > 15))
#define     TX_Mirror_Val(i,val)          (((val) << _TX_Mirror_Pos) << (((i) & (_TX_Mirror_Reg_Packed-1)) * _TX_Mirror_Reg_Width))
#define     TX_Mirror_Mask(i)             ((_TX_Mirror_Mask << _TX_Mirror_Pos) << (((i) & (_TX_Mirror_Reg_Packed-1)) * _TX_Mirror_Reg_Width))
#define     TX_Mirror_Pos(i)              ((_TX_Mirror_Pos) + (((i) & (_TX_Mirror_Reg_Packed-1)) * _TX_Mirror_Reg_Width))   /*added by dushb*/



/*
 * Address Lookup Table Control Registers
 */
#define RT8326_ALT_CONFIG                 0x0300
#define     _ALT_AGING_POS                0
#define     _ALT_FAST_AGING_POS           1
#define     _ALT_CTRL_FILTER_POS          2

#define RT8326_ALT_LEARN_BASE             0x0301
#define     _ALT_Learn_Reg_Width          1
#define     _ALT_Learn_Reg_Packed         16
#define     _ALT_Learn_Pos                1
#define     _ALT_Learn_Mask               0x0001
#define RT8326_ALT_LEARN(i)               (RT8326_ALT_LEARN_BASE + 0x0001 * (i > 15))
#define     ALT_Learn_Mask(i)             ((_ALT_Learn_Mask) << ((i & 0x000F) * 1))
#define     ALT_Learn_POS(i)              ((_ALT_Learn_Pos) + (((i) & (_ALT_Learn_Reg_Packed-1)) * _ALT_Learn_Reg_Width)) 
//#define RT8326_INTERNAL_0               0x0303
//#define RT8326_INTERNAL_1               0x0304
//#define RT8326_INTERNAL_2               0x0305
//#define RT8326_INTERNAL_3               0x0306

#define RT8326_PORT_TRUNK_BASE            0x0307
#define     _Trunk_Pos                    0x1
#define     _Trunk_Pos_1                  0
#define     _Trunk_Mask                   0x1
#define RT8326_PORT_TRUNK(i)              RT8326_PORT_TRUNK_BASE
#define     Trunk_Val(i,val)              (((val) << _Trunk_Pos) << (i >> 2))

#define     Trunk_Val_1(i,val)            (((val) << _Trunk_Pos_1) << (i >> 1))
#define     Trunk_Mask(i)                 ((_Trunk_Mask << _Trunk_Pos) << (i >> 2))
#define     Trunk_Mask_1(i)               ((_Trunk_Mask << _Trunk_Pos_1) << (i >> 1))
#define     Trunk_Bit(i)                  (i >> 2) 
#define     Trunk_Bit_1(i)                (i >> 1)
#define     Trunk_Index_Val(val, i)       (val & ((0x1 << (i >> 2)) << _Trunk_Pos))
#define     Trunk_Index_Val_1(val, i)     (val & ((0x1 << (i >> 1)) << _Trunk_Pos_1))

/* FOR 8326 */
//#define     Trunk_Val(i,val)              (i >> 2) ? ((val) << ((i >> 1) + 2) / 2) : ((val) << (i >> 1))
//#define     Trunk_Mask(i)                 (i >> 2) ? (_Trunk_Mask << ((i >> 1) + 2) / 2) : (_Trunk_Mask << (i >> 1))


#define RT8326_IGMP_SNOOP_BASE            0x0308
#define     _IGMP_Snoop_Enabled_Pos       0
#define     _IGMP_Snoop_Enabled_Mask      0x1
#define RT8326_IGMP_SNOOP                 RT8326_IGMP_SNOOP_BASE
#define     IGMP_Snoop_Enabled_Val(val)   ((val) << _IGMP_Snoop_Enabled_Pos)
#define     IGMP_Snoop_Enabled_Mask       (_IGMP_Snoop_Enabled_Mask << _IGMP_Snoop_Enabled_Pos)


#define RT8326_ROUTER_PORT_BASE           0x0309
#define     _Router_Port_Reg_Width        1
#define     _Router_Port_Reg_Packed       32
#define     _Router_Port_Pos              0
#define     _Router_Port_Mask             0x1
#define RT8326_ROUTER_PORT(i)             RT8326_ROUTER_PORT_BASE
#define     Router_Port_Val(i, val)       ((val) << _Router_Port_Pos)
#define     Router_Port_Mask(i)           ((_Router_Port_Mask << _Router_Port_Pos) << (((i) & (_Router_Port_Reg_Packed-1)) * _Router_Port_Reg_Width))


//#define RT8326_RESERVED                 0x030A

#define RT8326_VLAN_CTL_BASE              0x030B
#define     _EnVlan_Pos                   0
#define     _EnVlan_Mask                  0x1
#define     _EnUCleaky_Pos                1
#define     _EnUCleaky_Mask               0x1
#define     _EnARPleaky_Pos               2
#define     _EnARPleaky_Mask              0x1
#define     _EnIPMleaky_Pos               3
#define     _EnIPMleaky_Mask              0x1
#define     _En8021Qaware_Pos             4
#define     _En8021Qaware_Mask            0x1
#define     _EnIR_TagAdmit_Pos            5
#define     _EnIR_TagAdmit_Mask           0x1
#define     _EnIR_Membset_Pos             6
#define     _EnIR_Membset_Mask            0x1
#define     _EnPortIsolate_Pos               7             /* added by dushb */
#define     _EnPortIsolate_Maxk             0x1
#define RT8326_VLAN_CTL                   RT8326_VLAN_CTL_BASE
#define     EnVlan_Val(val)               ((val) << _EnVlan_Pos)
#define     EnUCleaky_Val(val)            ((val) << _EnUCleaky_Pos)
#define     EnARPleaky_Val(val)           ((val) << _EnARPleaky_Pos)
#define     EnIPMleaky_Val(val)           ((val) << _EnIPMleaky_Pos)
#define     En8021Qaware_Val(val)         ((val) << _En8021Qaware_Pos)
#define     EnIR_TagAdmit_Val(val)        ((val) << _EnIR_TagAdmit_Pos)
#define     EnIR_Membset_Val(val)         ((val) << _EnIR_Membset_Pos)
#define     EnVlan_Mask                   (_EnVlan_Mask << _EnVlan_Pos)
#define     EnUCleaky_Mask                (_EnUCleaky_Mask << _EnUCleaky_Pos)
#define     EnARPleaky_Mask               (_EnARPleaky_Mask << _EnARPleaky_Pos)
#define     EnIPMleaky_Mask               (_EnIPMleaky_Mask << _EnIPMleaky_Pos)
#define     En8021Qaware_Mask             (_En8021Qaware_Mask << _En8021Qaware_Pos)
#define     EnIR_TagAdmit_Mask            (_EnIR_TagAdmit_Mask << _EnIR_TagAdmit_Pos)
#define     EnIR_Membset_Mask             (_EnIR_Membset_Mask << _EnIR_Membset_Pos)


#define RT8326_VLAN_IDX_BASE              0x030C
#define     _Vlan_IDX_Reg_Width           8
#define     _Vlan_IDX_Reg_Packed          2
#define     _VID_Index_Pos                0
#define     _VID_Index_Mask               0x1F
#define RT8326_VLAN_IDX(i)                (RT8326_VLAN_IDX_BASE + 0x0001 * (i >> 1))
#define     VID_Index_Val(i,val)          (((val) << _VID_Index_Pos) << (((i) & (_Vlan_IDX_Reg_Packed-1)) * _Vlan_IDX_Reg_Width))
#define     VID_Index_Mask(i)             ((_VID_Index_Mask << _VID_Index_Pos) << (((i) & (_Vlan_IDX_Reg_Packed-1)) * _Vlan_IDX_Reg_Width))
#define     VID_Index_Shift(i)            (((i) & (_Vlan_IDX_Reg_Packed-1)) * _Vlan_IDX_Reg_Width)
#define     VID_Index_Pos(i)               ((_VID_Index_Pos) + (((i) & (_Vlan_IDX_Reg_Packed-1)) * _Vlan_IDX_Reg_Width))   /*added by dushb*/

#define RT8326_VLAN_PRI_BASE              0x0319
#define     _Vlan_Pri_Reg_Width           2
#define     _Vlan_Pri_Reg_Packed          8
#define     _PriTagCtrl_Pos               0
#define     _PriTagCtrl_Mask              0x3
#define RT8326_VLAN_PRI(i)                (RT8326_VLAN_PRI_BASE + 0x0001 * (i >> 3))
#define     PriTagCtrl_Val(i,val)         (((val) << _PriTagCtrl_Pos) << (((i) & (_Vlan_Pri_Reg_Packed-1)) * _Vlan_Pri_Reg_Width))
#define     PriTagCtrl_Mask(i)            ((_PriTagCtrl_Mask << _Port_Disable_Pos) << (((i) & (_Vlan_Pri_Reg_Packed-1)) * _Vlan_Pri_Reg_Width))
#define     PriTagCtrl_Pos(i)             ((_PriTagCtrl_Pos) + (((i) & (_Vlan_Pri_Reg_Packed-1)) * _Vlan_Pri_Reg_Width))   /*added by dushb*/

#define RT8326_VLAN_PORT_MEMBER_BASE      0x031D
#define     _Vlan_Port_Member_Reg_Width   1
#define     _Vlan_Port_Member_Reg_Packed  16
#define     _IsMember_Pos                 0
#define     _IsMember_Mask                0x1
#define RT8326_VLAN_PORT_MEMBER(e, p)     (RT8326_VLAN_PORT_MEMBER_BASE + (0x0003 * e) + 0x0001 * (p))
#define     IsMember_Val(p,val)           (((val) << _IsMember_Pos) << (((p) & (_Vlan_Port_Member_Reg_Packed-1)) * _Vlan_Port_Member_Reg_Width))
#define     IsMember_Mask(p)              ((_IsMember_Mask << _IsMember_Pos) << (((p) & (_Vlan_Port_Member_Reg_Packed-1)) * _Vlan_Port_Member_Reg_Width))


#define RT8326_VLAN_VID_BASE              0x031F
#define     _Vlan_VID_Reg_Width           12
#define     _Vlan_VID_Reg_Packed          1
#define     _Vlan_VID_Pos                 0
#define     _Vlan_VID_Mask                0x0FFF
#define RT8326_VLAN_VID(e, p)             (RT8326_VLAN_VID_BASE + (0x0003 * e))
#define     Vlan_VID_Val(p,val)           (((val) << _Vlan_VID_Pos) << (((p) & (_Vlan_VID_Reg_Packed-1)) * _Vlan_VID_Reg_Width))
#define     Vlan_VID_Mask(p)              ((_Vlan_VID_Mask << _Vlan_VID_Pos) << (((p) & (_Vlan_VID_Reg_Packed-1)) * _Vlan_VID_Reg_Width))


#define RT8326_VLAN_PVID_CTL_BASE         0x037D
#define     _Vlan_PVID_CTL_Reg_Width      1
#define     _Vlan_PVID_CTL_Reg_Packed     16
#define     _PVID_Enabled_Pos             0
#define     _PVID_Enabled_Mask            0x1
#define RT8326_VLAN_PVID_CTL(i)           (RT8326_VLAN_PVID_CTL_BASE + 0x0001 * (i > 15))
#define     PVID_Enabled_Val(i,val)       (((val) << _PVID_Enabled_Pos) << (((i) & (_Vlan_PVID_CTL_Reg_Packed-1)) * _Vlan_PVID_CTL_Reg_Width))
#define     PVID_Enabled_Mask(i)          ((_PVID_Enabled_Mask << _PVID_Enabled_Pos) << (((i) & (_Vlan_PVID_CTL_Reg_Packed-1)) * _Vlan_PVID_CTL_Reg_Width))


/*
 * Queue Control Registers
 */
#define RT8326_QOS_CTL_BASE               0x0400
#define     _DiffServ_Pri_Pos             0
#define     _DiffServ_Pri_Mask            0x1
#define     _TagVlan_Pri_Pos              1
#define     _TagVlan_Pri_Mask             0x1
#define     _FlowCtrl_Auto_Pos            2
#define     _FlowCtrl_Auto_Mask           0x1
#define     _HighLow_Ratio_Pos            3
#define     _HighLow_Ratio_Mask           0x3
#define RT8326_QOS_CTL                    RT8326_QOS_CTL_BASE
#define     DiffServ_Pri_Val(val)         ((val) << _DiffServ_Pri_Pos)
#define     TagVlan_Pri_Val(val)          ((val) << _TagVlan_Pri_Pos)
#define     FlowCtrl_Auto_Val(val)        ((val) << _FlowCtrl_Auto_Pos)
#define     HighLow_Ratio_Val(val)        ((val) << _HighLow_Ratio_Pos)
#define     DiffServ_Pri_Mask             (_DiffServ_Pri_Mask << _DiffServ_Pri_Pos)
#define     TagVlan_Pri_Mask              (_TagVlan_Pri_Mask << _TagVlan_Pri_Pos)
#define     FlowCtrl_Auto_Mask            (_FlowCtrl_Auto_Mask << _FlowCtrl_Auto_Pos)
#define     HighLow_Ratio_Mask            (_HighLow_Ratio_Mask << _HighLow_Ratio_Pos)


#define RT8326_PORT_PRI_BASE              0x0401
#define     _Port_Pri_Reg_Width           1
#define     _Port_Pri_Reg_Packed          16
#define     _Port_Pri_Pos                 0
#define     _Port_Pri_Mask                0x1
#define RT8326_PORT_PRI(i)                (RT8326_PORT_PRI_BASE + 0x0001 * (i > 15))
#define     Port_Pri_Val(i,val)           (((val) << _Port_Pri_Pos) << (((i) & (_Port_Pri_Reg_Packed-1)) * _Port_Pri_Reg_Width))
#define     Port_Pri_Mask(i)              ((_Port_Pri_Mask << _Port_Pri_Pos) << (((i) & (_Port_Pri_Reg_Packed-1)) * _Port_Pri_Reg_Width))
#define     Port_Pri_Pos(i)              ((_Port_Pri_Pos) + (((i) & (_Port_Pri_Reg_Packed-1)) * _Port_Pri_Reg_Width)) 
/*
 * PHY Access Control Registers
 */
#define RT8326_ACCESS_CTL                 0x0500
#define     PHY_Register_Pos                   0              /* added by dushb */
#define     PHY_Address_Pos                   5
#define     PHY_RW_Pos                         14
#define     PHY_Busy_Pos                       15
#define RT8326_ACCESS_WRITE               0x0501
#define     PHY_Write_Data_Pos               0              /* added by dushb */
#define RT8326_ACCESS_READ                0x0502
#define     PHY_Read_Data_Pos                0              /* added by dushb */

/*
 * Port Control Registers
 */
//#define RT8326_RESERVED                 0x0600
//#define RT8326_RESERVED                 0x0601
//#define RT8326_RESERVED                 0x0602
//#define RT8326_RESERVED                 0x0603
//#define RT8326_RESERVED                 0x0604
//#define RT8326_RESERVED                 0x0605
//#define RT8326_RESERVED                 0x0606

#define RT8326_GLOBAL_PORT_CTL_BASE       0x0607
#define     _FlowControl_FD_Pos           0
#define     _FlowControl_FD_Mask          0x1
#define     _FlowControl_HD_Pos           1
#define     _FlowControl_HD_Mask          0x1
#define     _FlowControl_BC_Pos           2
#define     _FlowControl_BC_Mask          0x1
#define     _FlowControl_MC_Pos           3
#define     _FlowControl_MC_Mask          0x1
#define     _Filter_BC_Storm_Pos          4
#define     _Filter_BC_Storm_Mask         0x1
#define RT8326_GLOBAL_PORT_CTL            RT8326_GLOBAL_PORT_CTL_BASE
#define     FlowControl_FD_Val(val)       ((val) << _FlowControl_FD_Pos)
#define     FlowControl_HD_Val(val)       ((val) << _FlowControl_HD_Pos)
#define     FlowControl_BC_Val(val)       ((val) << _FlowControl_BC_Pos)
#define     FlowControl_MC_Val(val)       ((val) << _FlowControl_MC_Pos)
#define     Filter_BC_Storm_Val(val)      ((val) << _Filter_BC_Storm_Pos)
#define     FlowControl_FD_Mask           (_FlowControl_FD_Mask << _FlowControl_FD_Pos)
#define     FlowControl_HD_Mask           (_FlowControl_HD_Mask << _FlowControl_HD_Pos)
#define     FlowControl_BC_Mask           (_FlowControl_BC_Mask << _FlowControl_BC_Pos)
#define     FlowControl_MC_Mask           (_FlowControl_MC_Mask << _FlowControl_MC_Pos)
#define     Filter_BC_Storm_Mask          (_Filter_BC_Storm_Mask << _Filter_BC_Storm_Pos)


// OK
#define RT8326_PORT_DISABLE_BASE          0x0608
#define     _Port_Disable_Reg_Width       1
#define     _Port_Disable_Reg_Packed      16
#define     _Port_Disable_Pos             0
#define     _Port_Disable_Mask            0x1
#define RT8326_PORT_DISABLE(i)            (RT8326_PORT_DISABLE_BASE + 0x0001 * (i > 15))
#define     Port_Disable_Val(i,val)       (((val) << _Port_Disable_Pos) << (((i) & (_Port_Disable_Reg_Packed-1)) * _Port_Disable_Reg_Width))
#define     Port_Disable_Mask(i)          ((_Port_Disable_Mask << _Port_Disable_Pos) << (((i) & (_Port_Disable_Reg_Packed-1)) * _Port_Disable_Reg_Width))
#define     Port_Disable_Pos(i)            ((_Port_Disable_Pos) + (((i) & (_Port_Disable_Reg_Packed- 1)) * _Port_Disable_Reg_Width))   /* added by dushb */

#define RT8326_PORT_PROPERTY_BASE         0x060A
#define     _Port_Property_Reg_Width      8
#define     _Port_Property_Reg_Packed     2
#define     _Media_Capability_Pos         0
#define     _Media_Capability_Mask        0x1F
#define     _Pause_Pos                    5
#define     _Pause_Mask                   0x1
#define     _AsyPause_Pos                 6
#define     _AsyPause_Mask                0x1
#define     _AutoNegotiation_Pos          7
#define     _AutoNegotiation_Mask         0x1
#define RT8326_PORT_PROPERTY(i)           (RT8326_PORT_PROPERTY_BASE + 0x0001 * (i >> 1))
#define     Media_Capability_Val(i,val)   (((val) << _Media_Capability_Pos) << (((i) & (_Port_Property_Reg_Packed-1)) * _Port_Property_Reg_Width))
#define     Pause_Val(i,val)              (((val) << _Pause_Pos) << (((i) & (_Port_Property_Reg_Packed-1)) * _Port_Property_Reg_Width))
#define     AsyPause_Val(i, val)          (((val) << _AsyPause_Pos) << ((i & (_Port_Property_Reg_Packed-1)) * _Port_Property_Reg_Width))
#define     AutoNegotiation_Val(i, val)   (((val) << _AutoNegotiation_Pos) << ((i & (_Port_Property_Reg_Packed-1)) * _Port_Property_Reg_Width))
#define     Media_Capability_Mask(i)      ((_Media_Capability_Mask << _Media_Capability_Pos) << (((i) & (_Port_Property_Reg_Packed-1)) * _Port_Property_Reg_Width))
#define     Pause_Mask(i)                 ((_Pause_Mask << _Pause_Pos) << (((i) & (_Port_Property_Reg_Packed-1)) * _Port_Property_Reg_Width))
#define     AsyPause_Mask(i)              ((_AsyPause_Mask << _AsyPause_Pos) << ((i & (_Port_Property_Reg_Packed-1)) * _Port_Property_Reg_Width))
#define     AutoNegotiation_Mask(i)       ((_AutoNegotiation_Mask << _AutoNegotiation_Pos) << ((i & (_Port_Property_Reg_Packed-1)) * _Port_Property_Reg_Width))
#define     Media_Capability_Shift(i)     ((((i) & (_Port_Property_Reg_Packed-1)) * _Port_Property_Reg_Width) + _Media_Capability_Pos)
#define     Media_Capability_Pos(i)       ((_Media_Capability_Pos) + (((i) & (_Port_Property_Reg_Packed- 1)) * _Port_Status_Reg_Width))    /* added by dushb */
#define     Pause_Pos(i)                     ((_Pause_Pos) + (((i) & (_Port_Property_Reg_Packed- 1)) * _Port_Status_Reg_Width))
#define     AsyPause_Pos(i)                ((_AsyPause_Pos) + (((i) & (_Port_Property_Reg_Packed- 1)) * _Port_Status_Reg_Width))
#define     AutoNegotiation_Pos(i)              ((_AutoNegotiation_Pos) + (((i) & (_Port_Property_Reg_Packed- 1)) * _Port_Status_Reg_Width))

//#define RT8326_RESERVED                 0x0617
//#define RT8326_RESERVED                 0x0618

// OK
#define RT8326_PORT_STATUS_BASE           0x0619
#define     _Port_Status_Reg_Width        8
#define     _Port_Status_Reg_Packed       2
#define     _Link_Status_Pos              0
#define     _Link_Status_Mask             0x7
#define     _Link_Speed_Pos               0
#define     _Link_Speed_Mask              0x3
#define     _Full_Duplex_Pos              2
#define     _Full_Duplex_Mask             0x1
#define     _Link_Up_Pos                  4
#define     _Link_Up_Mask                 0x1
#define     _Flow_Ctl_En_Pos              5
#define     _Flow_Ctl_En_Mask             0x1
#define     _Asy_Pause_En_Pos             6
#define     _Asy_Pause_En_Mask            0x1
#define     _Auto_Negotiation_En_Pos      7
#define     _Auto_Negotiation_En_Mask     0x1
#define RT8326_PORT_STATUS(i)             (RT8326_PORT_STATUS_BASE + 0x0001 * (i >> 1))
#define     Link_Speed_Val(i,val)         (((val) << _Link_Speed__Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Full_Duplex_Val(i,val)        (((val) << _Full_Duplex_Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Link_Up_Val(i,val)            (((val) << _Link_Up_Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Flow_Ctl_En_Val(i,val)        (((val) << _Flow_Ctl_En_Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Asy_Pause_En_Val(i,val)       (((val) << _Asy_Pause_En_Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Auto_Negotiation_En_Val(i,val)(((val) << _Auto_Negotiation_En_Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Link_Status_Mask(i)           ((_Link_Status_Mask << _Link_Status_Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Link_Speed_Mask(i)            ((_Link_Speed_Mask << _Link_Speed_Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Full_Duplex_Mask(i)           ((_Full_Duplex_Mask << _Full_Duplex_Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Link_Up_Mask(i)               ((_Link_Up_Mask << _Link_Up_Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Flow_Ctl_En_Mask(i)           ((_Flow_Ctl_En_Mask << _Flow_Ctl_En_Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Asy_Pause_En_Mask(i)          ((_Asy_Pause_En_Mask << _Asy_Pause_En_Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Auto_Negotiation_En_Mask(i)   ((_Auto_Negotiation_En_Mask << _Auto_Negotiation_En_Pos) << (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))
#define     Link_Status_Shift(i)          ((((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width) + _Link_Status_Pos)
#define     Link_Speed_Shift(i)           ((((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width) + _Link_Speed_Pos)
#define     Full_Duplex_Shift(i)          ((((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width) + _Full_Duplex_Pos)
#define     Link_Up_Shift(i)              ((((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width) + _Link_Up_Pos)
#define     Link_Status_Pos(i)      ((_Link_Status_Pos) + (((i) & (_Port_Status_Reg_Packed-1)) * _Port_Status_Reg_Width))          /* added by dushb */
#define     Link_Speed_Pos(i)       ((_Link_Speed_Pos) + (((i) & (_Port_Status_Reg_Packed- 1)) * _Port_Status_Reg_Width))
#define     Full_Duplex_Pos(i)       ((_Full_Duplex_Pos) + (((i) & (_Port_Status_Reg_Packed- 1)) * _Port_Status_Reg_Width))
#define     Link_Up_Pos(i)            ((_Link_Up_Pos) + (((i) & (_Port_Status_Reg_Packed- 1)) * _Port_Status_Reg_Width))
#define     Flow_Ctl_En_Pos(i)      ((_Flow_Ctl_En_Pos) + (((i) & (_Port_Status_Reg_Packed- 1)) * _Port_Status_Reg_Width))
#define     Asy_Pause_En_Pos(i)          ((_Asy_Pause_En_Pos) + (((i) & (_Port_Status_Reg_Packed- 1)) * _Port_Status_Reg_Width))
#define     Auto_Negotiation_En_Pos(i)     ((_Auto_Negotiation_En_Pos) + (((i) & (_Port_Status_Reg_Packed- 1)) * _Port_Status_Reg_Width))
/*
 * MIB Counter Registers
 */
#define RT8326_MIB_OBJ_SEL_BASE           0x0700
#define     _MIB_Obj_Sel_Reg_Width        6
#define     _MIB_Obj_Sel_Reg_Packed       2
#define     _RX_MIB_OBJ_Pos               0
#define     _RX_MIB_OBJ_Mask              0x3
#define     _TX_MIB_OBJ_Pos               2
#define     _TX_MIB_OBJ_Mask              0x3
#define     _DN_MIB_OBJ_Pos               4
#define     _DN_MIB_OBJ_Mask              0x3
#define RT8326_MIB_OBJ_SEL(i)             (RT8326_MIB_OBJ_SEL_BASE + 0x0001 * (i >> 1))
#define     RX_MIB_OBJ_Val(i,val)         (((val) << _RX_MIB_OBJ_Pos) << (((i) & (_MIB_Obj_Sel_Reg_Packed-1)) * _MIB_Obj_Sel_Reg_Width))
#define     TX_MIB_OBJ_Val(i,val)         (((val) << _TX_MIB_OBJ_Pos) << (((i) & (_MIB_Obj_Sel_Reg_Packed-1)) * _MIB_Obj_Sel_Reg_Width))
#define     DN_MIB_OBJ_Val(i,val)         (((val) << _DN_MIB_OBJ_Pos) << (((i) & (_MIB_Obj_Sel_Reg_Packed-1)) * _MIB_Obj_Sel_Reg_Width))
#define     RX_MIB_OBJ_Mask(i)            ((_RX_MIB_OBJ_Mask << _RX_MIB_OBJ_Pos) << (((i) & (_MIB_Obj_Sel_Reg_Packed-1)) * _MIB_Obj_Sel_Reg_Width))
#define     TX_MIB_OBJ_Mask(i)            ((_TX_MIB_OBJ_Mask << _TX_MIB_OBJ_Pos) << (((i) & (_MIB_Obj_Sel_Reg_Packed-1)) * _MIB_Obj_Sel_Reg_Width))
#define     DN_MIB_OBJ_Mask(i)            ((_DN_MIB_OBJ_Mask << _DN_MIB_OBJ_Pos) << (((i) & (_MIB_Obj_Sel_Reg_Packed-1)) * _MIB_Obj_Sel_Reg_Width))
#define     RX_MIB_OBJ_Pos(i)               ((_RX_MIB_OBJ_Pos) + (((i) & (_MIB_Obj_Sel_Reg_Packed-1)) * _MIB_Obj_Sel_Reg_Width))    /* added by dushb */
#define     TX_MIB_OBJ_Pos(i)               ((_TX_MIB_OBJ_Pos) + (((i) & (_MIB_Obj_Sel_Reg_Packed-1)) * _MIB_Obj_Sel_Reg_Width))
#define     DN_MIB_OBJ_Pos(i)               ((_DN_MIB_OBJ_Pos) + (((i) & (_MIB_Obj_Sel_Reg_Packed-1)) * _MIB_Obj_Sel_Reg_Width))

/*
 * Port MIB Counter 1 Registers (RX Counter)
 */
#define RT8326_MIB_RX_CNT_BASE            0x070D
#define RT8326_MIB_RX_CNT(i)              (RT8326_MIB_RX_CNT_BASE + 0x0001 * i)


/*
 * Port MIB Counter 2 Registers (TX Counter)
 */
#define RT8326_MIB_TX_CNT_BASE            0x0727
#define RT8326_MIB_TX_CNT(i)              (RT8326_MIB_TX_CNT_BASE + 0x0001 * i)


/*
 * Port MIB Counter 1 Registers (Diagnostics Counter)
 */
#define RT8326_MIB_DN_CNT_BASE            0x0741
#define RT8326_MIB_DN_CNT(i)              (RT8326_MIB_DN_CNT_BASE + 0x0001 * i)


/*
 * System Parameter Register (Reserved)
 */
//#define RT8326_SYS_PARAMETER            0xFFFF

/*
 * Bit Operation
 */
 
#define Get_Bit(val, i)                   (((val) & (1 << (i))) >> (i))
extern unsigned int AdvSpeed;
extern unsigned int TrunkDlg_flag;
//extern int SelectedSwitch;

/*
 * GWD added
 */

#define RCP_EEPROM_BASIC_ADDR                 0x400     /* product information basic address */
#define RCP_MAC_BASIC_ADDR                      0x12
#undef IN
#define IN
#undef OUT
#define OUT

typedef enum {
    RCP_SPEED_10M_HALF = 0x1,
    RCP_SPEED_10M_FULL = 0x2,
    RCP_SPEED_100M_HALF = 0x4,
    RCP_SPEED_100M_FULL = 0x8,
}RCP_PORT_SPEED;


typedef enum {
    RCP_NO_LIMIT = 0,
    RCP_128K,
    RCP_256K,
    RCP_512K,
    RCP_1M,
    RCP_2M,
    RCP_4M,
    RCP_8M
}RCP_RX_RATE,RCP_TX_RATE;

typedef enum {
    RCP_CONFIG_2_REGISTER = 1,
    RCP_CONFIG_2_EEPROM   = 2,
    RCP_CONFIG_2_BOTH     = 3
}RCP_FLAG;

#define	RCP_MAC_SIZE					6
#define MAX_RRCP_SWITCH_TO_MANAGE		NUM_PORTS_PER_SYSTEM
#define MAX_RTL_REG_TO_ACCESS			30
#define MAX_RCP_SWITCH_TO_MANAGE		24
#define MAX_RCP_VLAN_NUM                32
#define MAX_RCP_PORT_NUM                32
#define RCP_PKT_MAX_LENGTH				80
#define RCP_RESPONSE_TIMEOUT			500
#define RCP_HELLO_PAYLOAD_OFFSET		(RCP_MAC_SIZE + RCP_MAC_SIZE + 4 + 4)
#define RCP_HELLO_CHIPID_OFFSET			(RCP_HELLO_PAYLOAD_OFFSET + 10)

#define RCP_ALARM_STATUS_REGISTER		0x01
#define RCP_ALARM_STATUS_RE_REGISTER	0x02
#define RCP_ALARM_STATUS_OFFLINE		0x03

/* Define the different device type that may exist in system */
typedef enum
{
    GH1508		= 0,
    GH1516		= 1,
    GH1524  	= 2,  /*for GH1524*/
    GH1532  	= 3,  /*for GH1532*/
    MAX_RCP_DEV_ID
}RCP_DEVICE_ID;

typedef enum
{
	RTL_8208OR8316       	= 0xc,
	RTL_8324_FOR_GH1524		= 0xd,
	RTL_8324_FOR_GH1532     = 0xe,
	UNKNOWN_CHIP_ID
}RCP_CHIP_ID;
/*Define the chip id for rcp device register authentication*/

typedef struct _RCP_DEV RCP_DEV;

typedef int (*FRCP_READ_REG)(
				RCP_DEV*  dev,
				unsigned short regAddr, 
				unsigned short* data);

typedef int (*FRCP_WRITE_REG)(
				RCP_DEV*   dev,
				unsigned short regAddr, 
				unsigned short data);

#if 0
typedef void (*RCP_SEM_CREATE)(
						cyg_sem_t *semId, int countVal);
typedef void (*RCP_SEM_DELETE)(
						cyg_sem_t *semId);
typedef int (*RCP_SEM_TAKE)(
						cyg_sem_t *semId, unsigned long long timOut);
typedef void (*RCP_SEM_GIVE)(
						cyg_sem_t *semId);
#else
typedef void (*RCP_SEM_CREATE)(
						unsigned int  *semId, unsigned char *name, int initVal);
typedef void (*RCP_SEM_DELETE)(
						unsigned int semId);
typedef int (*RCP_SEM_TAKE)(
						unsigned int semId, unsigned long timOut);
typedef void (*RCP_SEM_GIVE)(
						unsigned int semId);
#endif
						
typedef int (*FRCP_GET_PHYPORT)(
				RCP_DEV*   dev,
				unsigned long slot,
				unsigned long lPort,
				unsigned long *unit,
				unsigned long *phyPort);
typedef int (*FRCP_GET_LPORT)(
				RCP_DEV*   dev,
				unsigned long *slot,
				unsigned long *lPort,
				unsigned long unit,
				unsigned long phyPort);
				

struct _RCP_DEV
{
    RCP_DEVICE_ID  	deviceId;

	unsigned char 	switchMac[RCP_MAC_SIZE];
	unsigned char 	previousswitchMac[RCP_MAC_SIZE]; /*mac address of previous switch  */
	unsigned char 	parentMac[RCP_MAC_SIZE];
	unsigned char 	chipID[2];  
	unsigned char 	vendorID[4];
	unsigned char 	modelID[16];

	unsigned char 	existEeprom;
	/* For Port Authentication */
    unsigned char   authenKey[2];

    unsigned char   upLinkPort;
    unsigned long   previousUplinkPort;
    unsigned char   parentPort;
    unsigned char   paSlot;	/* parentPort's slot id */
    unsigned char   paPort; /* parentPort's port id */
    unsigned long   loopStatus;
    unsigned long   loopAndDown;/*port looped and shut down */
	unsigned char   lpbDetIncapableFlag;
    unsigned char   onlineStatus;
    unsigned char   previousOnlineStatus;
    unsigned char   alarmStatus; /*alarm type flag*/
    unsigned char   alarmMask;
    unsigned char   alarmLocalLog; 
    unsigned long   vlanPort[MAX_RCP_VLAN_NUM];
    unsigned short  vlanVid[MAX_RCP_VLAN_NUM];
    unsigned char 	vlanPortIsolate;
    
    unsigned char	devEnabled;
    unsigned char   numOfPorts;
	unsigned char	maxPorts;
    unsigned char	devStorage;
    unsigned char	timeoutCounter;
    unsigned char	timeoutFlag;
    unsigned short  mgtVid;
    unsigned short  phyAddr;
    unsigned short  oamCounter;
	
    unsigned long	ingresslim[RCP_SW_MAX_PORT];
    unsigned long   egresslim[RCP_SW_MAX_PORT];

    FRCP_READ_REG 	frcpReadReg;
    FRCP_WRITE_REG 	frcpWriteReg;

    RCP_SEM_CREATE	semCreate; 	/* create semaphore */
    RCP_SEM_DELETE	semDelete; 	/* delete the semaphore */
    RCP_SEM_TAKE	semTake;	/* try to get a semaphore */
    RCP_SEM_GIVE	semGive;	/* return semaphore */

    FRCP_GET_LPORT 		frcpPort2LPort;
    FRCP_GET_PHYPORT 	frcpLPort2Port;

    void*		appData;

};

typedef struct _RCP_REG
{
	unsigned short address;
	unsigned long value;  /*unsigned long for 32-bit register*/
	unsigned char *note;
	RCP_DEV *dev;
	unsigned short validFlag;
	unsigned int semAccess;
}RCP_REG;

typedef struct _ALARM_FIFO_ENTRY_
{
	unsigned char *MsgBody;				
	unsigned short MsgLen;				
	struct _ALARM_FIFO_ENTRY_ *pNext;	
	struct _ALARM_FIFO_ENTRY_ *pPrevious;	
}ALARM_FIFO_ENTRY;


typedef struct _ALARM_FIFO_
{
	ALARM_FIFO_ENTRY *top;				
	ALARM_FIFO_ENTRY *bottom;				
	unsigned short occupancy;								
	unsigned short lengthLimitMax;								
	unsigned short lengthLimitMin;								
}ALARM_FIFO;

typedef struct _RCP_HELLO_PAYLOAD
{
	unsigned char authKey[2];
	unsigned char downLinkPort;
	unsigned char upLinkPort;
	unsigned char upLinkMac[6];
	unsigned char chipID[2];
	unsigned char vendorID[4];
}RCP_HELLO_PAYLOAD;

typedef struct _RCP_GET_REPLY_PAYLOAD
{
	unsigned char authKey[2];
	unsigned char regAddress[2];
	unsigned char regValue[4];
}RCP_GET_REPLY_PAYLOAD;

typedef struct _RCP_DEV_INFO
{
	unsigned char maxSlot;
	unsigned char maxPortPerSlot;
}RCP_DEV_INFO;

typedef struct _RCP_LPORT_INFO
{
	unsigned char unit;
	unsigned char port;
}RCP_LPORT_INFO;

typedef struct _RCP_LPORT_MAP
{
	RCP_LPORT_INFO L2Pmap[25];
}RCP_LPORT_MAP;

typedef struct _RCP_PPORT_INFO
{
	unsigned char slot;
	unsigned char port;
}RCP_PPORT_INFO;

typedef struct _RCP_PPORT_MAP
{
	RCP_PPORT_INFO P2Lmap[25];
}RCP_PPORT_MAP;

#define RCP_MAX_PORTS_PER_UNIT	24
#define RCP_MAX_PORTS_PER_SLOT	24

#define RCP_ERROR		   		(-1)
#define RCP_OK			   		(0x00)	 /* Operation succeeded                   */
#define RCP_FAIL			   	(0x01)	 /* Operation failed                      */
#define RCP_BAD_VALUE       	(0x02)   /* Illegal value (general)               */
#define RCP_BAD_PARAM       	(0x03)   /* Illegal parameter in function called  */
#define RCP_NOT_FOUND       	(0x04)   /* Item not found                        */
#define RCP_NO_MORE         	(0x05)   /* No more items found                   */
#define RCP_NO_SUCH         	(0x06)   /* No such item                          */
#define RCP_TIMEOUT         	(0x07)   /* Time Out                              */
#define RCP_NOT_SUPPORTED   	(0x08)   /* This request is not support           */
#define RCP_ALREADY_EXIST   	(0x09)   /* Tried to create existing item         */
#define RCP_NOT_INITIALIZED 	(0x0A)   /* Tried to access not intialize item    */
#define RCP_BAD_CONFIGURATION 	(0x0B)   /* Tried to access with error config     */
#define RCP_ILLEGAL_ADDRESS 	(0x0C) 	 /* Tried to access with error address    */
#define RCP_OS_ERROR 			(0x0D) 	 /* OS call return with error             */
#define RCP_FUTURE_USE 			(0x0E) 	 /* Not implement in current version      */
#define RCP_NO_MEM 				(0x0F) 	 /* No memory      */
#define RCP_UNKOWN 				(0x10) 	 /* Unknown status      */
#define RCP_NO_RESOURCE			(0x11) 	 /* No more resource    */

#define RCP_TRANS_FAILD		 	(0x12)	 /* Operation failed                      */
#define RCP_GETREG_FAILD    	(0x13)   /* Illegal value (general)               */
#define RCP_SETREG_FAILD    	(0x14)   /* Illegal parameter in function called  */
#define RCP_GETFIELD_FAILD  	(0x15)   /* Item not found                        */
#define RCP_SETFIELD_FAILD  	(0x16)   /* No more items found                   */
#define RCP_READEE_FAILD    	(0x17)   /* No such item                          */
#define RCP_WRITEE_FAILD    	(0x18)   /* Time Out                              */
#define RCP_ILLEGAL_AUTHKEY 	(0x19) 	 /* Tried to access with error auth_key   */
#define RCP_LAST_STATUS 		(0x1A) 	 /* Last value      */

#define  REALTEK_PROTOCOL_RRCP      0x01   
#define  REALTEK_PROTOCOL_ECHO      0x02
#define  REALTEK_PROTOCOL_LOOP      0x03
#define  REALTEK_RRCP_OPCODE_HELLO  0x00
#define  REALTEK_RRCP_OPCODE_GET    0x01
#define  REALTEK_RRCP_OPCODE_SET    0x02  
#define  REALTEK_RRCP_HELLO_REPLY   0x0180
#define  REALTEK_RRCP_GET_REPLY     0x0181
#define  REALTEK_AUTHEN_KEY         0x2379
#define  REALTEK_EEPROM_WRITE       0x0000
#define  REALTEK_EEPROM_READ        0x0800   

#define  RCP_PORT_MODE_100F        0x8
#define  RCP_PORT_MODE_100H        0x4
#define  RCP_PORT_MODE_10F         0x2
#define  RCP_PORT_MODE_10H         0x1

#define  RCP_CONFIG_REGISTER     	0x01
#define  RCP_CONFIG_EEPROM       	0x02  

#define  ETH_TYPE_RRCP    0x8899      /*added by dushaobo for realtek rrcp */

/*#define GET_RCP_PKT_SHORT( _BUF )   ((((unsigned short)(((unsigned char *)(_BUF))[1])) << 8) | (((unsigned short)(((unsigned char *)(_BUF))[0])) << 0))
#define SET_SHORT_TO_RCP_PKT( _BUF, value)	\
{ \

    unsigned short _ulValue;\
    _ulValue = (unsigned short)(((((value)& 0xFF00)>>8) | (((value) & 0x00FF)<<8)));\
    VOS_MemCpy(_BUF,&_ulValue,2);\
}*/
#define GET_RCP_PKT_SHORT( _BUF )   ((((unsigned short)(((unsigned char *)(_BUF))[1])) << 8) | (((unsigned short)(((unsigned char *)(_BUF))[0])) << 0))
#define SET_SHORT_TO_RCP_PKT( _BUF, value)	\
{ \
    _BUF[1] = (char)(((value)&0xFF00)>>8); \
    _BUF[0] = (char)(((value)&0x00FF)>>0); \
}

#define GET_RCP_PKT_LONG( _BUF ) ((((unsigned long)(((unsigned char *)(_BUF))[0])) << 0) | (((unsigned long)(((unsigned char *)(_BUF))[1])) << 8) | (((unsigned long)(((unsigned char *)(_BUF))[2])) << 16) | (((unsigned long)(((unsigned char *)(_BUF))[3])) << 24))

extern unsigned char rrcpHWPortID[33];
extern unsigned char rrcpSWPortID[33];
#define RCP_LPORT_2_PORT(_lport)       (rrcpHWPortID[_lport])
#define RCP_PORT_2_LPORT(_port)        (rrcpSWPortID[_port])

void gw_rcp_sem_create(unsigned int * semid, unsigned char * name ,unsigned int initval);
void gw_rcp_sem_delete(unsigned int semid);
int gw_rcp_sem_take(unsigned int semid, unsigned int timeout);
void gw_rcp_sem_give(unsigned int semid);


//#define RCP_GET_MGTPORT_VLAN_TAG(_dev, _ulTagged) IFM_VlanPortRelationApi( IFM_ETH_CREATE_INDEX(_dev->paSlot,_dev->parentPort), _dev->mgtVid, &_ulTagged)

int RCP_Init(void);
int RCP_DevList_Update(unsigned long parentPort, char *pkt);
int RCP_RegList_Update(unsigned long parentPort, char *pkt);
int RCP_RegList_Insert(RCP_REG *reg);
RCP_REG *RCP_RegList_Search(RCP_DEV *dev, unsigned short regAddr);
int RCP_RegList_Delete(RCP_REG *reg);
int RCP_Read_Reg(RCP_DEV *dev, unsigned short regAddr, unsigned short *data);
int RCP_Read_32bit_Reg(RCP_DEV *dev, unsigned short regAddr, unsigned long *data);
int RCP_Write_Reg(RCP_DEV *dev, unsigned short regAddr, unsigned short data);
int RCP_Say_Hello(int parentPort, unsigned short broadcastVid);
int RCP_Dev_Is_Exist(unsigned long parentPort);
int RCP_Dev_Is_Valid(unsigned long parentPort);
int RCP_Dev_Status_Set(unsigned long parentPort, unsigned char up);
RCP_DEV *RCP_Get_Dev_Ptr(unsigned long parentPort);
int Rcp_ChipId_AuthCheck(char *packet);
int RCP_GetEeaddrFromReg(unsigned short regAddr, unsigned short fieldOffset, unsigned short fieldLength, unsigned short *eeAddr, unsigned char *eeOffset, unsigned char *eeLength);
int RCP_GetRegField(RCP_DEV  *rrcp_dev, unsigned short  regAddr, unsigned short  fieldOffset, unsigned short  fieldLength, unsigned short  *data);
int RCP_SetRegisterField(RCP_DEV  *rrcp_dev, unsigned short  regAddr, unsigned short  fieldOffset, unsigned short  fieldLength, unsigned short  data);
int RCP_GetEepromValue(RCP_DEV  *rrcp_dev, unsigned short  eeAddr, unsigned char  *value);
int RCP_SetEepromValue(RCP_DEV  *rrcp_dev, unsigned short  eeAddr, unsigned char  value);
int RCP_GetEepromField(RCP_DEV  *rrcp_dev, unsigned short  eeAddr, unsigned char  fieldOffset, unsigned char  fieldLen, unsigned char  *value);
int RCP_SetEepromField(RCP_DEV  *rrcp_dev, unsigned short  eeAddr, unsigned char  fieldOffset, unsigned char  fieldLen, unsigned char  value);
int RCP_SetRegField(RCP_DEV  *rrcp_dev, unsigned short  regAddr, unsigned short fieldOffset, unsigned short fieldLength, unsigned short data, RCP_FLAG flag);
int RCP_SysSWReset(RCP_DEV  *rrcp_dev);
int RCP_SysHWReset(RCP_DEV  *rrcp_dev);
int RCP_GetSysEeprom(RCP_DEV  *rrcp_dev, unsigned short *data);
int RCP_GetAuthenkey(RCP_DEV  *rrcp_dev, unsigned short *data);
int RCP_SetAuthenKey(RCP_DEV  *rrcp_dev,unsigned short data);
int RCP_GetMaxPktLength(RCP_DEV  *rrcp_dev,  unsigned short  *data);
int RCP_SetMaxPktLength(RCP_DEV  *rrcp_dev,  unsigned short data);
int RCP_GetMaxPauseCnt(RCP_DEV * rrcp_dev,  unsigned short * data);
int RCP_GetPortLink(RCP_DEV  *rrcp_dev, unsigned long  slot,  unsigned long  port, unsigned short  *data);
int RCP_GetPortSpeed(RCP_DEV  *rrcp_dev, unsigned long  slot,  unsigned long  port, unsigned short  *data);
int RCP_GetPortDuplex(RCP_DEV  *rrcp_dev, unsigned long  slot,  unsigned long  port, unsigned short  *data);
int RCP_GetPortAutoNego(RCP_DEV  *rrcp_dev, unsigned long  slot,  unsigned long  port, unsigned short  *data);
int RCP_GetPortFC(RCP_DEV  *rrcp_dev, unsigned long  slot,  unsigned long  port, unsigned short  *data);
int RCP_GetPortLoopDetect(RCP_DEV  *rrcp_dev, unsigned long  slot,  unsigned long  port, unsigned short  *data);
int RCP_GetPortTruckFault(RCP_DEV  *rrcp_dev, unsigned long  slot,  unsigned long  port, unsigned short  *data);
int RCP_GetPortRouterPort(RCP_DEV  *rrcp_dev, unsigned long  slot,  unsigned long  port, unsigned short  *data);
int RCP_GetFullDuplexFC(RCP_DEV * rrcp_dev, unsigned short * data);
int RCP_SetFullDuplexFC(RCP_DEV * rrcp_dev, unsigned short data, RCP_FLAG falg);
int RCP_GetHalfDuplexFC(RCP_DEV * rrcp_dev, unsigned short * data);
int RCP_SetHalfDuplexFC(RCP_DEV * rrcp_dev,  unsigned short data,  RCP_FLAG falg);
int RCP_GetBroadcastFC(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetBroadcastFC(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetMulticastFC(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetMulticastFC(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetBroadcastStormFilter(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetBroadcastStormFilter(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetLoopDetect(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetLoopDetect(RCP_DEV  *rrcp_dev, unsigned short  data,  RCP_FLAG  flag);
int RCP_GetIGMPSnooping(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetIGMPSnooping(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetMIBRxObject(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short  *data);
int RCP_SetMIBRxObject(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short data,  RCP_FLAG  flag);
int RCP_GetMIBTxObject(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short  *data);
int RCP_SetMIBTxObject(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short data,  RCP_FLAG  flag);
int RCP_GetMIBDnObject(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short  *data);
int RCP_SetMIBDnObject(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short data,  RCP_FLAG  flag);
int RCP_GetMIBRxCounter(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port, unsigned long  *data);
int RCP_GetMIBTxCounter(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port, unsigned long  *data);
int RCP_GetMIBDiagCounter(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port, unsigned long  *data);
int RCP_GetPortEnable(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port, unsigned short *data);
int RCP_SetPortEnable(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short data,  RCP_FLAG flag);
int RCP_GetPortSpeedConfig(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port, unsigned short *data);
int RCP_SetPortSpeedConfig(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port, unsigned short  data,  RCP_FLAG flag);
int RCP_GetPortAutoNegoEnable(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port, unsigned short *data);
int RCP_SetPortAutoNegoEnable(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short data,  RCP_FLAG flag);
int RCP_GetPortPauseFC(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short *data);
int RCP_SetPortPauseFC(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short   data,  RCP_FLAG flag);
int RCP_GetPortRxBandwidth(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  RCP_RX_RATE *data);
int RCP_SetPortRxBandwidth(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  RCP_RX_RATE data,  RCP_FLAG flag);
int RCP_GetPortTxBandwidth(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  RCP_TX_RATE *data);
int RCP_SetPortTxBandwidth(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  RCP_TX_RATE   data,  RCP_FLAG flag);
int RCP_GetPortTrucking(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short *data);
int RCP_SetPortTrucking(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetQosDiffPriority(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetQosDiffPriority(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetQosVlanPriority(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetQosVlanPriority(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetQosMapMode(RCP_DEV  *rrcp_dev,  unsigned short *data);
int RCP_GetQosAdapterFC(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetQosAdapterFC(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetQosPrioRatio(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetQosPrioRatio(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetQosPortPriority(RCP_DEV  *rrcp_dev,  unsigned long slot, unsigned long port, unsigned short  *data);
int RCP_SetQosPortPriority(RCP_DEV  *rrcp_dev,  unsigned long slot, unsigned long port,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetQosPriorityCfg(RCP_DEV  *rrcp_dev,  unsigned long  *data);
int RCP_GetVlanEnable(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetVlanEnable(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetVlanUnicastLeaky(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetVlanUnicastLeaky(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetVlanMulticastLeaky(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetVlanMulticastLeaky(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetVlanArpLeaky(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetVlanArpLeaky(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetVlan8021Q(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetVlan8021Q(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetVlanPortIsolate(RCP_DEV  *rrcp_dev,  unsigned char  *data);
int RCP_SetVlanPortIsolate(RCP_DEV  *rrcp_dev,  unsigned char  data);
int RCP_GetVlan8021QTagOnly(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetVlan8021QTagOnly(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetVlanIRMember(RCP_DEV  *rrcp_dev, unsigned short *data);
int RCP_SetVlanIRMember(RCP_DEV * rrcp_dev, unsigned short data,  RCP_FLAG flag);
int RCP_GetVlanIngressFilter(RCP_DEV  *rrcp_dev, unsigned short  *data);
int RCP_SetVlanIngressFilter(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetVlanPortIDAssignment(RCP_DEV  *rrcp_dev,  unsigned long slot, unsigned long port,  unsigned short  *data);
int RCP_SetVlanPortIDAssignment(RCP_DEV  *rrcp_dev,  unsigned long slot, unsigned long port,  unsigned short data,  RCP_FLAG  flag);
int RCP_GetVlanOutPortTag(RCP_DEV  *rrcp_dev,  unsigned long slot, unsigned long port,  unsigned short  *data);
int RCP_SetVlanOutPortTag(RCP_DEV  *rrcp_dev,  unsigned long slot, unsigned long port,  unsigned short  data,  RCP_FLAG  flag);
int RCP_GetVlanPort(RCP_DEV  *rrcp_dev,  unsigned short vlanum, unsigned long *data);
int RCP_SetVlanPort(RCP_DEV  *rrcp_dev, unsigned short vlanum, unsigned long data, RCP_FLAG flag);
int RCP_GetVlanPortExist(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short vlanum, unsigned short *data);
int RCP_SetVlanPortAdd(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short vlanum,  RCP_FLAG flag);
int RCP_SetVlanPortDelete(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short vlanum,  RCP_FLAG flag);
int RCP_GetVlanVID(RCP_DEV  *rrcp_dev,  unsigned short  vlanum,  unsigned short  *data);
int RCP_SetVlanVID(RCP_DEV  *rrcp_dev,  unsigned short  vlanum,  unsigned short  data,  RCP_FLAG  flag);
int RCP_GetVlanPVID(RCP_DEV  *rrcp_dev,   unsigned long slot,  unsigned long port, unsigned short  *data);
int RCP_SetVlanPVID(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short  data,  RCP_FLAG  flag);
int RCP_GetVlanInsertPVID(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short *data);
int RCP_SetVlanInsertPVID(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short data,  RCP_FLAG flag);
int RCP_SetVlanInsert(RCP_DEV * rrcp_dev, unsigned short data, RCP_FLAG flag);
int RCP_GetVlanIdleNum(RCP_DEV *rrcp_dev,  unsigned short *idlenum);
int RCP_GetVlanExist(RCP_DEV *rrcp_dev,  unsigned short vid,   unsigned short *isExist);
int RCP_GetVlanExistNum(RCP_DEV  *rrcp_dev,  unsigned short *totalnum);
int RCP_SetVlanMgtPortIsolate(RCP_DEV *pRcpDev, unsigned long mgtPort, RCP_FLAG flag);
int RCP_GetVlanNumFromVID(RCP_DEV *rrcp_dev,   unsigned short vid,   unsigned short *vlanum);
int RCP_GetMirrorPort(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short  *data);
int RCP_SetMirrorPort(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetMirrorDestPort(RCP_DEV  *rrcp_dev, unsigned long *data);
int RCP_SetMirrorDestPort(RCP_DEV  *rrcp_dev,unsigned long   data);
int RCP_GetMirrorRxPort(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port, unsigned short  *data);
int RCP_SetMirrorRxPort(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetMirrorRxSrcPort(RCP_DEV  *rrcp_dev, unsigned long *data);
int RCP_GetMirrorTxPort(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port, unsigned short  *data);
int RCP_SetMirrorTxPort(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetMirrorTxSrcPort(RCP_DEV  *rrcp_dev, unsigned long *data);
int RCP_GetPhyRegValue(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short phyReg,  unsigned short *data);
int RCP_SetPhyRegValue(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short phyReg,  unsigned short data);
int RCP_GetPhyRegField(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short phyReg,  unsigned char offset,  unsigned char length,  unsigned short *data);
int RCP_SetPhyRegField(RCP_DEV  *rrcp_dev,  unsigned long  slot,  unsigned long  port,  unsigned short phyReg,  unsigned char offset,  unsigned char length,  unsigned short data);
int RCP_GetPhySpeed(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short *data);
int RCP_SetPhySpeed(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short data);
int RCP_GetPhyAutoNegoEnable(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short *data);
int RCP_SetPhyAutoNegoEnable(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short data);
int RCP_SetPhyReset(RCP_DEV *rrcp_dev,  unsigned long slot,  unsigned long port);
int RCP_SetPhyReAutoNego(RCP_DEV *rrcp_dev,  unsigned long slot,  unsigned long port);
int RCP_GetPhyPowerDown(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short *data);
int RCP_SetPhyPowerDown(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short data);
int RCP_GetPhyDuplex(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short *data);
int RCP_SetPhyDuplex(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short data);
int RCP_GetPhyFC(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short *data);
int RCP_SetPhyFC(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short data);
int RCP_GetPhy10H(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short *data);
int RCP_SetPhy10H(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short data);
int RCP_GetPhy10F(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short *data);
int RCP_SetPhy10F(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short data);
int RCP_GetPhy100H(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short *data);
int RCP_SetPhy100H(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short data);
int RCP_GetPhy100F(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short *data);
int RCP_SetPhy100F(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long port,  unsigned short data);
int RCP_GetAtuLearning(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long  port,  unsigned short *data);
int RCP_GetAtuLearningList(RCP_DEV  *rrcp_dev,  unsigned short *data);
int RCP_SetAtuLearning(RCP_DEV  *rrcp_dev,  unsigned long slot,  unsigned long  port,  unsigned short data, RCP_FLAG  flag);
int RCP_GetAtuAgingEnable(RCP_DEV  *rrcp_dev,  unsigned short *data);
int RCP_SetAtuAgingEnable(RCP_DEV  *rrcp_dev,  unsigned short data,  RCP_FLAG flag);
int RCP_GetAtuFastAgingEnable(RCP_DEV  *rrcp_dev,  unsigned short *data);
int RCP_SetAtuFastAgingEnable(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG flag);
int RCP_GetAtuCtrlFrameFilter(RCP_DEV  *rrcp_dev,  unsigned short  *data);
int RCP_SetAtuCtrlFrameFilter(RCP_DEV  *rrcp_dev,  unsigned short  data,  RCP_FLAG  flag);
void convertOidToChar(oid * oidMac,unsigned char *charMac);
void convertMacToOid(oid * oidMac,unsigned char *charMac);
int RCP_GetDevIndexByMac(oid *mac, unsigned long *devId);
int RCP_GetValidIndex(oid *mac, unsigned long *index);
int RCP_GetFirstIndex(unsigned long *index);
int RCP_GetNextIndex(unsigned long index, unsigned long *nextIndex);
int RCP_GetPortListSNMP(RCP_DEV * rcpDev,unsigned short portList, unsigned short *newList);
int RCP_PortListToSNMP(RCP_DEV * rcpDev,unsigned long portList, unsigned char *newList);
int RCP_SNMPToPortList(RCP_DEV * rcpDev,unsigned long *portList, unsigned char *newList);
int RCP_SetPortListSNMP(RCP_DEV * rcpDev,unsigned short portList, unsigned short *newList);
int RCP_GetPortList16SNMP(unsigned short portList, unsigned short *newList);
int RCP_GetPortList8SNMP(unsigned char portList, unsigned char *newList);
int RCP_SetVlanPortIsolateEn(RCP_DEV * rcpDev, unsigned short data, unsigned long mgtPort, RCP_FLAG flag);
int RCP_GetVlanUntagPortList(RCP_DEV  *rrcp_dev,  unsigned long *portList);
int RCP_SetVlanUntagPortList(RCP_DEV  *rcpDev,  unsigned short vlanum,  unsigned long   *portList,  unsigned long  port_len,  RCP_FLAG flag);
int RCP_SetVlanTaggedPortList(RCP_DEV  *rcpDev, unsigned short vlanum,  unsigned long *portList,  unsigned long  port_len,  RCP_FLAG flag);
int RCP_SetVlanPortListDelete(RCP_DEV  *rcpDev, unsigned short vlanum, unsigned long  *portList, unsigned long  port_len, RCP_FLAG flag);
int RCP_SetVlanSave(RCP_DEV *rcpDev, unsigned short vlanum);
int RCP_SetVlanEnableSave(RCP_DEV *rcpDev, unsigned short enable);
int RCP_SetVlanDelete(RCP_DEV  *rcpDev,unsigned short vid,RCP_FLAG flag);
int RCP_GetQosPriPort(RCP_DEV  *rcpDev,unsigned long *data);
int RCP_SetConfigClear(RCP_DEV  *rcpDev);
int RCP_GetPortMauType(RCP_DEV  *rcpDev,unsigned long slot, unsigned long port,oid *mau,unsigned long *mau_len);
int RCP_SetPortMauType(RCP_DEV *dev,unsigned long slot,unsigned long port,unsigned short mau,unsigned char data,RCP_FLAG flag);
int RCP_SetVlan8021QEnable(RCP_DEV  *rcpDev,unsigned short enable,RCP_FLAG flag);
int Rcp_Get_MGT_Port(RCP_DEV *dev, unsigned long *slot, unsigned long *port);
int sendOamRcpLpbDetectNotifyMsg(unsigned char onuPort, unsigned char rcpPort, unsigned char state, unsigned short uvid,unsigned char *session);
int pushOneSwitchStatusChgMsg(unsigned char onuPort, unsigned char* switchMac, unsigned char msgType, unsigned char reason);
int popAllSwitchStatusChgMsg(void);
int saveAllRcpDevCfgToFlash(void);
int saveOneRcpDevCfgToFlash(RCP_DEV  *rcpDev);
int restoreRcpDevCfgFromFlash(RCP_DEV  *rcpDev);
int ereaseRcpDevCfgInFlash(RCP_DEV  *rcpDev, int allDev);
int IS_Flash_Data_Valid(RCP_DEV  *rcpDev);
int getRegValueInFlash(RCP_DEV  *rcpDev, unsigned short usRegAddr, unsigned short *pRegValue);
int RCP_GetVlanPortIsolate_InFlash(RCP_DEV  *rcpDev, unsigned char *data);
int RCP_GetVlanPortIsolate_InEeprom(RCP_DEV  *rrcp_dev, unsigned char *data);
int RCP_GetDeviceID_InFlash(RCP_DEV  *rcpDev, unsigned short *data);
RCP_DEV *RCP_Get_Dev_Ptr_For_Flash(unsigned long parentPort);
int rrcp_packet_handler(unsigned int srcPort, unsigned int len, unsigned char *packet);
int RcpFrameRevHandle(gw_uint32  portid ,gw_uint32  len, gw_uint8  *frame);
int gw_Onu_Rcp_Detect_Set_FDB(unsigned char  opr);

int device_conf_write_switch_conf_to_flash(char * buf, long int length);
int device_conf_read_switch_conf_from_flash(char *buf, long int *length);

#endif /* _RRCP_H_ */
#endif

