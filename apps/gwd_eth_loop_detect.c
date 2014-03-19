#if 0
#include "iros_config.h"
#include <network.h>
#include <cyg/kernel/kapi.h>
#include <pkgconf/io_fileio.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_io.h>
#include <pkgconf/hal.h>
#include <pkgconf/system.h>
#include <pkgconf/memalloc.h>
#include <pkgconf/isoinfra.h>
#include <sys/socket.h>
//#include <netinet/tcpip.h>
//#include <socketvar.h>
//#include <sys/sockio.h>
#include <netinet/tcp.h>
//#include <netinet/tcp_fsm.h>
//#include <netinet/tcp_seq.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <netinet/tcp.h>



#include <netinet/tcp_timer.h>
#include <netinet/tcp_var.h>
//#include <netinet/tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <immenstar.h>
#include "i2c.h"  // UART driver is based on I2C, Hardware is I2C <=> exar UART
#include "gpio.h" // UART interrupt use GPIO pins 
#include "uart.h"
#include "onu_sync_api.h"
#include "onuAalInt.h"

#include "oam.h"

#include "marvell/onu_marvell_sample_hwcfg.h"
#include "onu_switch_if.h"
#include "onu_sw_api.h"
#include "onu_sync_api.h"
#include "frame.h"
#include "if_eth_drv.h"

#include "gwd_eth_loop_detect.h"

#else

#include "../include/gw_os_api_core.h"
#include "gwdonuif_interval.h"
#include "gw_log.h"
#include "oam.h"
#include "gwd_eth_loop_detect.h"
#include "pkt_main.h"

#endif

#define EPON_ETHERTYPE_DOT1Q 0x8100
#define LPB_OAM_SEND_INTERVAL 1
#define LOOP_DETECT_THREAD_STACKSIZE     (8 * 1024)

LPB_CTRL_LIST *g_lpb_detect_ctrl_head = NULL, *g_lpb_detect_ctrl_tail = NULL;

gw_int32 gwd_loop_thread_id;
unsigned long timeCounter = 0;
unsigned long gulLoopDetectMode = 1;
unsigned long gulLoopDetectFrameHandleRegister = 0;
OAM_ONU_LPB_DETECT_FRAME local_onu_lpb_detect_frame = {0, 0, 0, 0, {0, 0, 0, 0, 0, 0},30, 1, 3, 3};

unsigned long gulPortDownWhenLpbFound[NUM_PORTS_PER_SYSTEM+1] = { 0 };
unsigned long gulPortWakeupCounter[NUM_PORTS_PER_SYSTEM+1] = { 0 };
unsigned long gulLoopRecFlag[NUM_PORTS_PER_SYSTEM+1] = { 0 };
unsigned long gulLoopIgnorePortDefault[NUM_PORTS_PER_SYSTEM+1] = {0};
unsigned long gulLoopIgnorePort[NUM_PORTS_PER_SYSTEM+1] = { 0 };

unsigned long gw_lpb_sem;

#if 0
unsigned char loop_detect_thread_stack[LOOP_DETECT_THREAD_STACKSIZE];
cyg_handle_t  loop_detect_thread_handle;
cyg_thread    loop_detect_thread_obj;
#endif

unsigned long   gulDebugLoopBackDetect = 0;
#define LOOPBACK_DETECT_DEBUG(str) if( gulDebugLoopBackDetect ){ gw_printf str ;}
#ifdef CYG_LINUX
#define DUMPGWDPKT(c, p, b, l)      if(gulDebugLoopBackDetect) dumpPkt(c, p, b, l)
#else
#define DUMPGWDPKT(c, p, b, l)      if(gulDebugLoopBackDetect) \
{ \
	gw_printf("\r\n%s    (%d)\r\n", c, p); \
	gw_dump_buffer(b, l); \
}
#endif

unsigned long gulNumOfPortsPerSystem = NUM_PORTS_PER_SYSTEM;

ALARM_LOOP sendLoopAlarmOam;

unsigned char loop_detect_mac[6] = {0x00, 0x0F, 0xE9, 0x04, 0x8E, 0xDF};


char port_loop_back_session[8]="";

char olt_type_string[][16] = {
	"UNKNOWN",
	"GFA6100",
	"GFA6700",
	"GFA6900",
	"UNKNOWN"
};

extern OAM_ONU_LPB_DETECT_FRAME oam_onu_lpb_detect_frame, tframe;
long ethLoopBackDetectActionCall( int enable, char * oamSession);
int sendOamLpbDetectNotifyMsg(unsigned char port, unsigned char state, unsigned short uvid,unsigned char *session, ALARM_LOOP *pAlarmInfo);
long ethLoopBackDetectActionCall( int enable, char * oamSession);

int gw_loopbackFrameParser( gw_uint8  *frame, gw_uint32  len);
int loopbackFrameRevHandle(gw_uint32  portid ,gw_uint32  len, gw_uint8  *frame);
int gw_loopbackFrameRevHandle(gw_uint8  *frame, gw_uint32  len, gw_uint32  portid );
//extern gw_return_code_t epon_request_ctc_onu_phy_admin_state_read (gw_uint8 port, gw_uint32 *state);
extern int CommOnuMsgSend(unsigned char GwOpcode, unsigned int SendSerNo, unsigned char *pSentData,const unsigned short SendDataSize, unsigned char  *pSessionIdfield);
extern int GwGetOltType(unsigned char *mac, GWD_OLT_TYPE *type);
extern int GwGetPonSlotPort(unsigned char *mac, GWD_OLT_TYPE type, unsigned long *slot, unsigned long *port);
//extern epon_port_id_t ifm_port_id_make(epon_physical_port_id_t phy_id, epon_logical_link_id_t llid, epon_port_type_t port_type);

static int FoundWakeupPortFlag = WAKEUP_DISABLE;
int Gwd_loop_port_wakeup_set(unsigned int wakestat)
{
    FoundWakeupPortFlag = wakestat;
    return GW_OK;
}

int Gwd_loop_port_wakeup_get(unsigned int* wakestat)
{
    *wakestat = FoundWakeupPortFlag;
    return GW_OK;
}
int gw_lpb_detect_init(void)
{
	return gw_semaphore_init(&gw_lpb_sem, "lpb_sem", 1, 0);
}

int gw_lpb_sem_take()
{
	return gw_semaphore_wait(gw_lpb_sem, GW_OSAL_WAIT_FOREVER);
}

int gw_lpb_sem_give( )
{
	return gw_semaphore_post(gw_lpb_sem);
}
#if 0
int boards_logical_to_physical(unsigned long lport, unsigned long *unit, unsigned long *pport)
{
    int lport_0 = (lport-1);

    /* check whether logical port number is legal or not */
    if ((lport_0 < 0) || (lport_0 >= NUM_PORTS_PER_SYSTEM)) {
        return 0;
    }
    *unit = log_phy_map[lport_0].unit;
    *pport = log_phy_map[lport_0].physical_port;

    return 1;
}

int boards_physical_to_logical(unsigned long unit, unsigned long pport, unsigned long *lport)
{
    /* check whether logical port number is legal or not */
    if ((unit >= NUM_UNITS_PER_SYSTEM) || (pport > PHY_PORT_MAX)) {
        return 0;
    }

    *lport = phy_log_map[unit][pport];
    if ( *lport == 0xFF ) {
        return 0;
    }

    return 1;
}
#endif

#if 0
int gtGetSrcPortForMac(char *mac, unsigned short vid, unsigned long *pLogicPort)
{
	int gtRet = GWD_RETURN_ERR;
	epon_sw_fdb_entry_t gtAtuEntry;
	gw_boolean gtFound;
	int     i, numOfUniPorts;
	epon_sw_vlan_config_t vtuEntry;
	unsigned int uiPortVect;

	if(NULL == mac)
		return GWD_RETURN_ERR;
		
	memset(&gtAtuEntry, 0, sizeof (epon_sw_fdb_entry_t));

	for (i = 0; i < GW_MACADDR_LEN ; i++)
		gtAtuEntry.addr[i] = mac[i];
	//gtAtuEntry.DBNum = vid;
	memset(&vtuEntry, 0, sizeof (epon_sw_vlan_config_t));
	//vtuEntry.DBNum = vid;
	vtuEntry.vlan = vid;

	*pLogicPort = 0xFF;
	if(GW_RETURN_SUCCESS == (gtRet = epon_onu_sw_search_vlan_entry(&vtuEntry, &gtFound)))
	{
		gtRet = epon_onu_sw_search_fdb_entry(&gtAtuEntry, &gtFound);
		if (GW_RETURN_SUCCESS == gtRet)
		{
			if (GW_TRUE == gtFound)
			{
				LOOPBACK_DETECT_DEBUG(("\r\nMac found in vlan %d/0x%x", vid, gtAtuEntry.egress_portmap));
				numOfUniPorts = gw_onu_read_port_num();
				for (i = 0; i < numOfUniPorts; i++)
				{
					uiPortVect = 0x1 << i;
					if (gtAtuEntry.egress_portmap & uiPortVect)
					{
						if((0 == (vtuEntry.tagged_portmap & uiPortVect)) &&
						   (0 == (vtuEntry.untagged_portmap & uiPortVect)) &&
						   (0 == (vtuEntry.unmodified_portmap & uiPortVect)))
						{
							LOOPBACK_DETECT_DEBUG(("\r\nMac found in wrong vlan %d", vid));
							*pLogicPort = 0xFF;
							gtRet = GWD_RETURN_ERR;
						}
						else
						{
							LOOPBACK_DETECT_DEBUG(("\r\nMac found in the right vlan"));
							boards_physical_to_logical(0, i, pLogicPort);
							gtRet = GWD_RETURN_OK;
						}
					}
				}
			}
			else
			{
				LOOPBACK_DETECT_DEBUG(("\r\nMac NOT found in vlan %d", vid));
				gtRet = GWD_RETURN_ERR;
			}
		}
		else
		{
			LOOPBACK_DETECT_DEBUG(("\r\nepon_onu_sw_search_fdb_entry failed %d", gtRet));
		}
	}
	else
	{
		LOOPBACK_DETECT_DEBUG(("\r\epon_onu_sw_search_vlan_entry failed %d", gtRet));
	}
	

	return gtRet;
}
#else
int gtGetSrcPortForMac(char *mac, unsigned short vid, unsigned long *pLogicPort)
{
	int gtRet = GWD_RETURN_ERR;

	int     i, numOfUniPorts;

	unsigned int uiPortVect, eg_ports, tag_ports, untag_ports;

	if(NULL == mac)
		return GWD_RETURN_ERR;

	*pLogicPort = 0xFF;
	if(GW_RETURN_SUCCESS == (gtRet = call_gwdonu_if_api(LIB_IF_VLAN_ENTRY_GET, 3, vid, &tag_ports, &untag_ports)))
	{
		gtRet = call_gwdonu_if_api(LIB_IF_FDB_ENTRY_GET, 3,  vid, mac, &eg_ports);
		if (GW_RETURN_SUCCESS == gtRet)
		{

			LOOPBACK_DETECT_DEBUG(("\r\nMac found in vlan %d/0x%x", vid, eg_ports));
			numOfUniPorts = gw_onu_read_port_num();
			for (i = 0; i < numOfUniPorts; i++)
			{
				uiPortVect = 0x1 << i;
				if (eg_ports & uiPortVect)
				{
					if((0 == (tag_ports & uiPortVect)) &&
					   (0 == (untag_ports & uiPortVect)) )
					{
						LOOPBACK_DETECT_DEBUG(("\r\nMac found in wrong vlan %d", vid));
						*pLogicPort = 0xFF;
						gtRet = GWD_RETURN_ERR;
					}
					else
					{
						LOOPBACK_DETECT_DEBUG(("\r\nMac found in the right vlan"));
						boards_physical_to_logical(0, i, pLogicPort);
						gtRet = GWD_RETURN_OK;
					}
				}
			}
		}
		else
		{
			LOOPBACK_DETECT_DEBUG(("\r\nMac NOT found in vlan %d", vid));
		}
	}
	else
	{
		LOOPBACK_DETECT_DEBUG(("\r\nget vlan entry fail! %d", gtRet));
	}
	

	return gtRet;
}

#endif

int boards_port_is_uni(unsigned long lport)
{
	return (lport < gulNumOfPortsPerSystem)? 1 : 0;
}

unsigned long IFM_ETH_GET_SLOT( unsigned long ulIfIndex )
{
    IFM_ETH_IF_INDEX_U unIfIndex;
    unsigned long ulSlot = 0;

    unIfIndex.ulPhyIfIndex = ulIfIndex;
    ulSlot = unIfIndex.phy_slot_port.slot;

    return ulSlot;
}

unsigned long IFM_ETH_GET_PORT( unsigned long ulIfIndex )
{
    IFM_ETH_IF_INDEX_U unIfIndex;
    unsigned long ulPort = 0;

    unIfIndex.ulPhyIfIndex = ulIfIndex;
    ulPort = unIfIndex.phy_slot_port.port;

    return ulPort;
}

unsigned long IFM_ETH_CREATE_INDEX( unsigned long ulSlot, unsigned long ulPort )
{
    IFM_ETH_IF_INDEX_U unIfIndx;

    /* this is a union, so the first line is used to clear the structure */
    unIfIndx.ulPhyIfIndex = 0;
    unIfIndx.phy_slot_port.type = IFM_ETH_TYPE;
    unIfIndx.phy_slot_port.slot = ulSlot;
    unIfIndx.phy_slot_port.port = ulPort;
    unIfIndx.phy_slot_port.subif = 0;

    return unIfIndx.ulPhyIfIndex;
}

#if 0
int IFM_GET_FIRST_PORTONVLAN(unsigned long *ulport, unsigned short vid)
{
	epon_sw_vlan_config_t vlan_entry;
    gw_boolean found;
    gw_return_code_t ret = 1;
	int i;
	unsigned long lport;
    
    vlan_entry.vlan =vid;
    ret =epon_onu_sw_search_vlan_entry(&vlan_entry, &found);
    if(found)
	{
		for(i=0; i<PHY_PORT_MAX; i++)
		{
			if((vlan_entry.tagged_portmap & (1<<i)) || (vlan_entry.untagged_portmap & (1<<i)))
			{
				if(boards_physical_to_logical(0, i, &lport))
				{
					*ulport = lport;
					return GWD_RETURN_OK;
				}
			}
		}
    }
	return GWD_RETURN_ERR;
}
#else
int IFM_GET_FIRST_PORTONVLAN(unsigned long *ulport, unsigned short vid)
{
    gw_return_code_t ret = 1;
	int i;
	unsigned long lport, tagged_portmap, untagged_portmap;
    
    ret = call_gwdonu_if_api(LIB_IF_VLAN_ENTRY_GET, 3, vid, &tagged_portmap, &untagged_portmap);
    if(ret == GW_RETURN_SUCCESS)
	{
		for(i=0; i<PHY_PORT_MAX; i++)
		{
			if((tagged_portmap & (1<<i)) || (untagged_portmap & (1<<i)))
			{
				if(boards_physical_to_logical(0, i, &lport))
				{
					*ulport = lport;
					return GWD_RETURN_OK;
				}
			}
		}
    }
	return GWD_RETURN_ERR;
}

#endif

char *getRealProductType(unsigned short int productID)
{
	unsigned char st;

	if(DEVICE_TYPE_GT873_A == productID)
	{
		if(call_gwdonu_if_api(LIB_IF_REAL_PRODUCT_TYPE_GET, 1,&st) != GWD_RETURN_OK)
		{
			gw_printf("Get Real Name fail!\r\n");
		}
		if(DEVICE_TYPE_GT873_A == st)
			return "GT873_A";
		else if(DEVICE_TYPE_GT872_A == st)
			return "GT872_A";
	}
}

char* onu_product_name_get(unsigned short int productID)
{
	switch(productID)
	{
		case DEVICE_TYPE_GT821:
			return "GT821";

		case DEVICE_TYPE_GT831:
			return "GT831";

		case DEVICE_TYPE_GT831_B:
		case DEVICE_TYPE_GT831_B_CATV:
			return "GT831_B";
			
		case DEVICE_TYPE_GT810:
			return "GT810";

		case DEVICE_TYPE_GT816:
			return "GT816";

		case DEVICE_TYPE_GT811:
			return "GT811";
		case DEVICE_TYPE_GT811_A:
			return "GT811_A";

		case DEVICE_TYPE_GT812:
			return "GT812";
		case DEVICE_TYPE_GT812_A:
			return "GT812_A";

		case DEVICE_TYPE_GT813:
			return "GT813";

		case DEVICE_TYPE_GT865:
			return "GT865";

		case DEVICE_TYPE_GT861:
			return "GT861";

		case DEVICE_TYPE_GT815:
			return "GT815";

		case DEVICE_TYPE_GT812PB:
			return "GT812_B";

		case DEVICE_TYPE_GT866:
			return "GT866";

		case DEVICE_TYPE_GT863:
			return "GT863";
		case DEVICE_TYPE_GT871B:
		    return "GT871B";
		case DEVICE_TYPE_GT871R:
		    return "GT871R";
		case DEVICE_TYPE_GT872:
		    return "GT872";
		case DEVICE_TYPE_GT873:
		    return "GT873";

		case DEVICE_TYPE_GT813_C:
			return "GT813_C";
		case DEVICE_TYPE_GT811_C:
			return "GT811_C";
		case DEVICE_TYPE_GT812_C:
			return "GT812_C";
		case DEVICE_TYPE_GT815_C:
			return "GT815_C";

		case DEVICE_TYPE_GT872_A:
			return "GT872_A";
		case DEVICE_TYPE_GT873_A:
			return getRealProductType(DEVICE_TYPE_GT873_A);
        case DEVICE_TYPE_GT813C_B:
            return "GT813C_B";
        case DEVICE_TYPE_GT815C_B:
            return "GT815C_B";
                

		default:
			return "UNKNOWN";
	}
}

void EthPortLoopBackDetectTask(void * data)
{
    unsigned short intervalTime;
        
	while(oam_onu_lpb_detect_frame.enable && gulLoopDetectMode)
	{
        LOOPBACK_DETECT_DEBUG(("\r\nEthPortLoopBackDetectTask called"));
	    if(timeCounter == LPB_OAM_SEND_INTERVAL)
        	timeCounter = 0;
    	else
        	timeCounter++;
    	if(LOOP_DETECT_LOCAL_DFT == local_onu_lpb_detect_frame.enable)
        	intervalTime = oam_onu_lpb_detect_frame.interval;
        else
        	intervalTime = local_onu_lpb_detect_frame.interval;
        ethLoopBackDetectActionCall(oam_onu_lpb_detect_frame.enable, port_loop_back_session);
        //VOS_TaskDelay((intervalTime)*VOS_TICK_SECOND);
//		cyg_thread_delay((intervalTime)*IROS_TICK_PER_SECOND);
        gw_thread_delay((intervalTime)*1000);
	}
	
    LOOPBACK_DETECT_DEBUG(("\r\nEthPortLoopBackDetectTask exit!!!"));
	return;
}

#if 0
/*jiangxt added, 20111008.*/
unsigned int Onu_Loop_Detect_Set_FDB(gw_boolean  opr)
{
        int iRet; 
        epon_sw_fdb_entry_t fdb_entry;
        epon_switch_chiptype_t sw_chiptype = epon_onu_get_swith_chiptype();
        if (sw_chiptype && (sw_chiptype != 1))
        {
               return 1;
        }
        LOOPBACK_DETECT_DEBUG(("\r\nOnu_Loop_Detect_Set_FDB func sw_chiptype is : %x", sw_chiptype));
     
        if (opr)
        {
            /*add fdb*/
            memset(&fdb_entry, 0, sizeof(fdb_entry));
            memcpy(fdb_entry.addr, loop_detect_mac, 6);
            fdb_entry.status = FDB_ENTRY_MGMT;
            
            if (sw_chiptype == EPON_SWITCH_6045)
                fdb_entry.egress_portmap = 0x10;
            
            else if (sw_chiptype == EPON_SWITCH_6046)
                fdb_entry.egress_portmap = 0x400;
            
            iRet = epon_onu_sw_add_fdb_entry(&fdb_entry);
            if (iRet != GW_RETURN_SUCCESS)
            {
                LOOPBACK_DETECT_DEBUG(("\r\nOnu_Loop_Detect_Set_FDB func fdb add error."));
                return 1;
            }
        }

        return 0;
}
#endif

long EthLoopbackDetectControl(unsigned long oamEnable, unsigned long localEnable)
{
	//unsigned char   szName[32];
      unsigned long 	enable;
//      int iRet;  /*jiangxt added, 20111008.*/

    enable = oamEnable && localEnable;
	if (enable)
	{
        LOOPBACK_DETECT_DEBUG(("\r\nEthLoopbackDetectControl : enable = %lu", enable));
		/* Enable */
		if (oam_onu_lpb_detect_frame.enable && gulLoopDetectMode)
			return GWD_RETURN_OK;

		/* Enable */
		oam_onu_lpb_detect_frame.enable = 1;
        gulLoopDetectMode = LOOP_DETECT_MODE_OLT;

        if(!gulLoopDetectFrameHandleRegister)
        {
#if 0			
            /*jiangxt added, 20111008.*/
           #ifdef HAVE_EXT_SW_DRIVER
           iRet = epon_onu_sw_register_frame_handle(loopbackFrameRevHandle);
           #else
           iRet = epon_onu_register_special_frame_handle(loopbackFrameRevHandle);
           #endif

           if (iRet == GW_RETURN_FAIL)
           {
                 LOOPBACK_DETECT_DEBUG(("\r\nepon_onu_register_special_frame_handle failed!"));
           }
           else if (iRet == GW_RETURN_SUCCESS)
           {
                 LOOPBACK_DETECT_DEBUG(("\r\nepon_onu_register_special_frame_handle success!"));
           }
#else		
		gw_reg_pkt_parse(GW_PKT_LPB, gw_loopbackFrameParser);
		gw_reg_pkt_handler(GW_PKT_LPB, gw_loopbackFrameRevHandle);
#endif
/*           iRet = Onu_Loop_Detect_Set_FDB(1);
           if (iRet == 0)
           {
                 LOOPBACK_DETECT_DEBUG(("\r\nonu_loop_detect_set success!"));
           }
           else
           {
                 LOOPBACK_DETECT_DEBUG(("\r\nonu_loop_detect_set failed!"));
           }
*/	
           /*added end, jiangxt.*/

	if(call_gwdonu_if_api(LIB_IF_FDB_MGT_MAC_SET, 1, loop_detect_mac) == GW_OK)
	{
                 LOOPBACK_DETECT_DEBUG(("\r\nonu_loop_detect_set success!"));	
	}
	else
	{
		LOOPBACK_DETECT_DEBUG(("\r\nonu_loop_detect_set failed!"));
	}
               
           	gulLoopDetectFrameHandleRegister = 1;

			/* Tx and lookup Task */
			//sprintf(szName, "tLoopDetect");
			//VOS_TaskCreate(szName, TASK_PRIORITY_LOWEST, EthPortLoopBackDetectTask, lTaskArg);
	        // create ONU application thread

           	if(gw_thread_create(&gwd_loop_thread_id,
           			"gwdloopdetect",
           			EthPortLoopBackDetectTask,
           			NULL,
           			LOOP_DETECT_THREAD_STACKSIZE,
           			TASK_PRIORITY_LOWEST,
           			0) != GW_OK)
           		{
           			gw_log(GW_LOG_LEVEL_DEBUG, "\r\nloop_detect_thread create fail!");
           		}
           	else
           		gw_log(GW_LOG_LEVEL_DEBUG, "\r\nloop_detect_thread created");

           	/*
	        cyg_thread_create(TASK_PRIORITY_LOWEST,
	                          EthPortLoopBackDetectTask,
	                          0,
	                          "tLoopDetect",
	                          &loop_detect_thread_stack,
	                          LOOP_DETECT_THREAD_STACKSIZE,
	                          &loop_detect_thread_handle,
	                          &loop_detect_thread_obj);
	        printf("\r\nloop_detect_thread created");
         	cyg_thread_resume(loop_detect_thread_handle);
         	*/
       }
	}
	else
	{
        LOOPBACK_DETECT_DEBUG(("\r\nEthLoopbackDetectControl : enable = %lu", enable));
		/* Disable */
		/*if (0 == oam_onu_lpb_detect_frame.enable)
			return GWD_RETURN_OK;*/
		/* Stop */
		if(oamEnable == 0)
            oam_onu_lpb_detect_frame.enable = 0;
        else
            oam_onu_lpb_detect_frame.enable = 1;
        if(localEnable == 0)
            gulLoopDetectMode = LOOP_DETECT_MODE_DISABLE;
        else
            gulLoopDetectMode = LOOP_DETECT_MODE_OLT;

		gw_thread_delete(gwd_loop_thread_id);
		
        gulLoopDetectFrameHandleRegister = 0;
	}

	return GWD_RETURN_OK;
}

OAM_ONU_LPB_DETECT_CTRL *getVlanLpbStasNode(unsigned short vid)
{
	LPB_CTRL_LIST *pNode = g_lpb_detect_ctrl_head;
	
	while(pNode != NULL)
	{
		if(pNode->ctrlnode->vid == vid)
			break;
		pNode = pNode->next;
	}

	return ((pNode!=NULL)?pNode->ctrlnode:NULL);
}

void deleteLpbStatsNode( unsigned short vid )
{
	LPB_CTRL_LIST *pNode = g_lpb_detect_ctrl_head, *ppre = NULL;

	while(pNode != NULL)
	{
		if(pNode->ctrlnode->vid == vid)
		{
			if(pNode == g_lpb_detect_ctrl_head)
			{
				g_lpb_detect_ctrl_head = g_lpb_detect_ctrl_head->next;
				if(g_lpb_detect_ctrl_head == NULL)
					g_lpb_detect_ctrl_tail = NULL;
			}
			else if(pNode == g_lpb_detect_ctrl_tail)
			{
				g_lpb_detect_ctrl_tail = ppre;
				ppre->next = NULL;
			}
			else
			{
				ppre->next = pNode->next;
			}

			free(pNode->ctrlnode);
			free(pNode);
			break;
			
		}
		else
		{
			ppre = pNode;
			pNode = pNode->next;
		}
	}
}

static void reportPortsLpbStatus( unsigned short vid, char *session )
{
	int i=0, almstats = 0;
	LPB_CTRL_LIST *pNode = NULL;

	OAM_ONU_LPB_DETECT_CTRL *pCtrl = NULL;

	gw_lpb_sem_take();

        pNode = g_lpb_detect_ctrl_head;
	
	while(pNode != NULL)
	{
		if(pNode->ctrlnode->vid == vid)
			break;
		pNode = pNode->next;
	}

    if(!pNode)
    {
		gw_lpb_sem_give();
		return;
    }

	pCtrl = pNode->ctrlnode;
	
	for(i=1; i<gulNumOfPortsPerSystem; i++)
	{
		if(pCtrl->lpbportdown[i])
		{

			gwd_port_admin_t st = PORT_ADMIN_DOWN;
			if((call_gwdonu_if_api(LIB_IF_PORT_ADMIN_GET, 2, i, &st) == GWD_RETURN_OK) && (PORT_ADMIN_UP == st))
			{
				pCtrl->lpbportdown[i] = 0;
            	LOOPBACK_DETECT_DEBUG(("\r\nSet vlan(%d)lpbportdown[%d] = 0,cause port %d's admin is UP", vid, i,i));
			}
		}

		if( pCtrl->lpbmask[i]&&(pCtrl->lpbClearCnt[i] == 0))
		{
			//IFM_config( ethIfIdx, IFM_CONFIG_ETH_ALARM_STATUS_SET, &loopstatus, NULL );
            if(timeCounter == LPB_OAM_SEND_INTERVAL)
            {
            	sendOamLpbDetectNotifyMsg(i, 1, vid, session, &(pCtrl->alarmInfo[i]));
                LOOPBACK_DETECT_DEBUG(("\r\nReport vlan(%d)port(%d)Looped", vid, i));
            }
		}
		if(pCtrl->lpbmask[i])
        	almstats ++;

	}

	if(almstats == 0)//added by wangxiaoyu 2009-05-08
	{ /*all ports' lpbmask cleared,then delete the node */
		deleteLpbStatsNode(pCtrl->vid);
		LOOPBACK_DETECT_DEBUG(("\r\nDelete lpb stat info node in vlan %d", pCtrl->vid));
	}

	gw_lpb_sem_give();
	
}
void freeLpbStatusList(void)
{
	LPB_CTRL_LIST *pList = NULL;

	gw_lpb_sem_take();
	
	while(g_lpb_detect_ctrl_head)
	{
		pList = g_lpb_detect_ctrl_head;
		free(pList->ctrlnode);
		g_lpb_detect_ctrl_head = pList->next;
		free(pList);
	}
	g_lpb_detect_ctrl_tail = NULL;

	gw_lpb_sem_give();
}

int clsPortLpbStatus(const unsigned short vid, const char *ss)
{ /* no loop found,port with lpbmask and not lpbportdown then lpbClearCnt++*/
    unsigned long i;
    OAM_ONU_LPB_DETECT_CTRL *pCtrl = NULL;

    gw_lpb_sem_take();

    pCtrl = getVlanLpbStasNode(vid);
    if(pCtrl)
    {
//        printf("need to clr status\r\n");
    	for(i = 1; i < gulNumOfPortsPerSystem; i++)
        {
            if(pCtrl->lpbmask[i] == 1 && pCtrl->lpbportdown[i] != 1)
            {
    			if(0 == gulLoopRecFlag[i]) /* Loop alarm oam has been sent */
    			{
    	            pCtrl->lpbClearCnt[i]++;
    	            LOOPBACK_DETECT_DEBUG(("\r\nclsPortLpbStatus calls,lpbClearCnt[%lu]++=%d", i,pCtrl->lpbClearCnt[i]));
    			}

             }
        }
    }

	gw_lpb_sem_give();
	return 0;
}

int setPortLpbStatus(const unsigned short vid, const int lport, const int shutdown, const char * ss, const void *pInfo)
{
	int ret = GWD_RETURN_ERR;
	
	OAM_ONU_LPB_DETECT_CTRL *pCtrl = NULL;

	gw_lpb_sem_take();

	pCtrl = getVlanLpbStasNode(vid);
	
	if(pCtrl != NULL)
	{
		//pCtrl->lpbStateChg[lport] = (pCtrl->lpbmask[lport])?0:1; //added by wangxiaoyu 2009-03-17

		if(!pCtrl->lpbmask[lport])
		{
			if(shutdown == 0)
   	                      LOOPBACK_DETECT_DEBUG(("\r\nNot shutdown port , lpbmask[%lu] : %d", lport, pCtrl->lpbmask[lport]));
   	                 gw_log(GW_LOG_LEVEL_MAJOR, "Interface  eth%d/%lu marked loopback in vlan %u\n", 1, lport, vid);
		}
		pCtrl->lpbClearCnt[lport] = 0;
        	pCtrl->lpbmask[lport] = 1;
		LOOPBACK_DETECT_DEBUG(("\r\nVlan(%d) found (&pCtrl = %p) and set lpbmask[%d]=%d(lport = %d)", vid, pCtrl, lport, pCtrl->lpbmask[lport], lport));
		
		if(shutdown == 0)
			pCtrl->lpbportdown[lport] = 0;
		else
		{

			if(((!local_onu_lpb_detect_frame.enable)&&(pCtrl->lpbportwakeupcounter[lport] < oam_onu_lpb_detect_frame.maxwakeup))||
            	((local_onu_lpb_detect_frame.enable)&&(pCtrl->lpbportwakeupcounter[lport] < local_onu_lpb_detect_frame.maxwakeup)))
			{
				pCtrl->slpcounter[lport] = 0;
                gulPortWakeupCounter[lport] = pCtrl->lpbportwakeupcounter[lport] ;
				LOOPBACK_DETECT_DEBUG(("\r\nrepeated wakeup -- %d in vlan %d", pCtrl->lpbportwakeupcounter[lport], pCtrl->vid));
				LOOPBACK_DETECT_DEBUG(("\r\nmaximum wakeup reapeater is -- %d", oam_onu_lpb_detect_frame.maxwakeup));
			}
			else if(pCtrl->lpbportwakeupcounter[lport] == 255)
			{
				pCtrl->lpbportwakeupcounter[lport] = 0;
                gulPortWakeupCounter[lport] = pCtrl->lpbportwakeupcounter[lport];
				pCtrl->slpcounter[lport] = 0;
				LOOPBACK_DETECT_DEBUG(("\r\nwakeup task start in vlan %d", pCtrl->vid));
			}
			else
			{
				pCtrl->lpbportwakeupcounter[lport] = 255;
                gulPortWakeupCounter[lport] = pCtrl->lpbportwakeupcounter[lport];
				LOOPBACK_DETECT_DEBUG(("\r\nwakeup task stop in vlan %d", pCtrl->vid));
			}
			
			pCtrl->lpbportdown[lport] = 1;
			if(shutdown)
				gw_log(GW_LOG_LEVEL_MAJOR, "Interface eth%d/%lu shut down for loopback\r\n",1,lport);			
		}

		if(NULL != pInfo)
		{
			memcpy(&(pCtrl->alarmInfo[lport]), pInfo, sizeof(ALARM_LOOP));
		}
		
		ret = GWD_RETURN_OK;
	}
	else
	{
		pCtrl = (OAM_ONU_LPB_DETECT_CTRL*)malloc( sizeof(OAM_ONU_LPB_DETECT_CTRL));
		if(pCtrl != NULL)
		{			
			LPB_CTRL_LIST *pList = (LPB_CTRL_LIST *)malloc( sizeof(LPB_CTRL_LIST));
			memset(pCtrl, 0, sizeof(OAM_ONU_LPB_DETECT_CTRL));
			if(pList == NULL)
			{
				free(pCtrl);
			}
			else
			{
				memset(pList, 0, sizeof(LPB_CTRL_LIST));
				
				pCtrl->vid = vid;
				pCtrl->lpbmask[lport] = 1;
				pCtrl->lpbStateChg[lport] = 1;
				LOOPBACK_DETECT_DEBUG(("\r\nNew vlan(%d) (&pCtrl = %p) and set lpbmask[%d]: %d", vid, pCtrl, lport, pCtrl->lpbmask[lport]));
				gw_log(GW_LOG_LEVEL_MAJOR, "Interface  eth%d/%lu marked loopback in vlan %u\n", 1, lport, vid);
				if(shutdown)
					gw_log(GW_LOG_LEVEL_MAJOR, "Interface eth%d/%lu shut down for loopback\r\n",1,lport);
				
				if(shutdown == 0)
					pCtrl->lpbportdown[lport] = 0;
				else
				{
					pCtrl->lpbportwakeupcounter[lport] = 0;
                    gulPortWakeupCounter[lport] = pCtrl->lpbportwakeupcounter[lport];
					LOOPBACK_DETECT_DEBUG(("\r\nInit wakeup counter in vlan %d", pCtrl->vid));
					pCtrl->lpbportdown[lport] = 1;
				}

				pList->ctrlnode = pCtrl;
				pList->next = NULL;

				if(g_lpb_detect_ctrl_head == NULL && g_lpb_detect_ctrl_tail == NULL)
				{
					g_lpb_detect_ctrl_head = pList;
					g_lpb_detect_ctrl_tail = pList;
					LOOPBACK_DETECT_DEBUG(("\r\nNew lpb control list"));
				}
				else
				{
					g_lpb_detect_ctrl_tail->next = pList;
					g_lpb_detect_ctrl_tail = g_lpb_detect_ctrl_tail->next;
					LOOPBACK_DETECT_DEBUG(("\r\nNew lpb control node"));
				}

				if(NULL != pInfo)
				{
					memcpy(&(pCtrl->alarmInfo[lport]), pInfo, sizeof(ALARM_LOOP));
				}
				ret = GWD_RETURN_OK;
			}
		}
	}

	gw_lpb_sem_give();

	return ret;

}

/*begin:
modified by wangxiaoyu,ï¿½Þ¸ï¿½ï¿½Ð¶ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½forÑ­ï¿½ï¿½ï¿½ï¿½1ï¿½ï¿½Ê¼*/
static int resetLpbPort( int force, unsigned char *session )
{	
	LPB_CTRL_LIST *pList = NULL;
	unsigned long lport;
	int ret;

	gw_lpb_sem_take();

	pList = g_lpb_detect_ctrl_head;
	
	while(pList)
	{
		//ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ç²ï¿½ï¿½Ø±Õ¶Ë¿Ú£ï¿½ï¿½ï¿½Ô­ï¿½ï¿½ï¿½Ø±ÕµÄ¶Ë¿ï¿½ï¿½ï¿½ï¿½Â´ï¿½
		if((force || (!oam_onu_lpb_detect_frame.policy&0x0001))||
        	(force || (!local_onu_lpb_detect_frame.policy&0x0001)))
		{
			for (lport = 1; lport < gulNumOfPortsPerSystem; lport++)
			{
				if(1 == pList->ctrlnode->lpbportdown[lport])
				{
					call_gwdonu_if_api(LIB_IF_PORT_ADMIN_SET, 2, lport, PORT_ADMIN_UP);
					if(GWD_RETURN_OK == (ret = sendOamLpbDetectNotifyMsg(lport, 2, pList->ctrlnode->vid, session, &(pList->ctrlnode->alarmInfo[lport]))))
					{
						pList->ctrlnode->lpbportdown[lport] = 0;
						pList->ctrlnode->lpbmask[lport] = 0;
						pList->ctrlnode->lpbClearCnt[lport] = 0;
					}
                    LOOPBACK_DETECT_DEBUG(("\r\nsendOamLpbDetectNotifyMsg2,%lu(ret=%d)", lport, ret));
					//IFM_config( ulIfIndex, IFM_CONFIG_ETH_ALARM_STATUS_CLEAR, &loopstatus, NULL );
				}
			}
		}
		else
		{
			LOOPBACK_DETECT_DEBUG(("\r\nresetport deny:"));
			/*LOOPBACK_DETECT_DEBUG(("\r\nlpbPortMask: %d", pList->ctrlnode->lpbmask));*/
		}
		
		pList = pList->next;
	}

	gw_lpb_sem_give();

	return GWD_RETURN_OK;
}

/*begin:
modified by wangxiaoyu 2008-05-14
add uvid
*/
int sendOamLpbDetectNotifyMsg(unsigned char port, unsigned char state, unsigned short uvid,unsigned char *session, ALARM_LOOP *pAlarmInfo)
{
	int ret; 
	/*modified by wangxiaoyu 2008-06-06
	ï¿½ï¿½ï¿½ï¿½ï¿½Õ»ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½temp[4]-->temp[16]
	*/
    unsigned char ucSlot, ucPort;
    ALARM_LOOP *alarmLoop;
    unsigned char temp[40]= { 0 };

	if((NULL == session) || (NULL == pAlarmInfo)) 
	{
		return GWD_RETURN_ERR;
	}

    ucSlot = 1;
    ucPort = port;

    alarmLoop = (ALARM_LOOP *)temp;
    alarmLoop->alarmFlag = ONU_PORT_LOOP_ALARM;
    alarmLoop->portNum[2] = ucSlot;
    alarmLoop->portNum[3] = ucPort;
    alarmLoop->loopstate = state;
    alarmLoop->vlanid = htons(uvid);
    if(1 /*gulLoopRecFlag[port]*/)
    {   
        if(pAlarmInfo->onuLocation[1] >= 1 && pAlarmInfo->onuLocation[1] <= MAX_GWD_OLT_SLOT
                    && pAlarmInfo->onuLocation[2] >= 1 && pAlarmInfo->onuLocation[2] <= MAX_GWD_OLT_PORT
                    && pAlarmInfo->oltType > GWD_OLT_NONE && pAlarmInfo->oltType < GWD_OLT_NOMATCH)
        {
            alarmLoop->externFlag = 1;
            alarmLoop->oltType = pAlarmInfo->oltType;
            memcpy(alarmLoop->oltMac, pAlarmInfo->oltMac, 6);
            memcpy(alarmLoop->onuLocation, pAlarmInfo->onuLocation, 4);
            alarmLoop->onuType = pAlarmInfo->onuType;
            memcpy(alarmLoop->onuMac, pAlarmInfo->onuMac, 6);
            memcpy(alarmLoop->onuPort, pAlarmInfo->onuPort, 4);
        }
    }

	ret = CommOnuMsgSend(ALARM_REQ,0,(unsigned char*)alarmLoop,sizeof(ALARM_LOOP), session);
	LOOPBACK_DETECT_DEBUG(("\r\nSend ALARM_REQ for lpb detection %s! state is %d    port %d/%d", (GWD_RETURN_OK != ret) ? "ERROR" : "SUCCESS", state, 1, port));
	/*if(state == 1)
		VOS_SysLog(LOG_TYPE_ETH, LOG_WARNING, "send eth %d/%d marked loopback status.", PORTNO_TO_ETH_SLOT(port), PORTNO_TO_ETH_PORTID(port));
	else
		VOS_SysLog(LOG_TYPE_ETH, LOG_WARNING, "send eth %d/%d clear loopback status.", PORTNO_TO_ETH_SLOT(port), PORTNO_TO_ETH_PORTID(port));*/
    gulLoopRecFlag[port] = 0;
	
	return ret;	
}
void lpbDetectWakeupPorts(unsigned short usVid)
{
    int portnum, ret;
	OAM_ONU_LPB_DETECT_CTRL *pCtrl = NULL;

	gw_lpb_sem_take();

	pCtrl = getVlanLpbStasNode(usVid);
	
        if(pCtrl != NULL)
        {
            for (portnum = 1; portnum < gulNumOfPortsPerSystem; portnum++)
            {
    			/*ethIfIndex = IFM_ETH_CREATE_INDEX(PORTNO_TO_ETH_SLOT(portnum), PORTNO_TO_ETH_PORTID(portnum));*/
                if((1== pCtrl->lpbmask[portnum])&&(10<=pCtrl->lpbClearCnt[portnum]))
                {
    				if(GWD_RETURN_OK != (ret = sendOamLpbDetectNotifyMsg(portnum, 2, usVid, port_loop_back_session, &(pCtrl->alarmInfo[portnum]))))
    				{
    					gw_log(GW_LOG_LEVEL_DEBUG, "sendOamLpbDetectNotifyMsg failed!\n");
    				}
    				pCtrl->lpbmask[portnum] = 0;
    				pCtrl->lpbportwakeupcounter[portnum] = 0;
    				pCtrl->slpcounter[portnum] = 0;
    				pCtrl->lpbClearCnt[portnum] = 0;
    				/*IFM_config( ethIfIndex, IFM_CONFIG_ETH_ALARM_STATUS_CLEAR, &loopstatus, NULL );
    				VOS_SysLog(LOG_TYPE_TRAP, LOG_INFO,"Interface  eth%d/%d no loop found in vlan %d for 10 intervals,clear status", PORTNO_TO_ETH_SLOT(portnum), PORTNO_TO_ETH_PORTID(portnum), usVid);*/

//				LOOPBACK_DETECT_DEBUG(("\r\nVlan(%d)port(%d)no loop in 10 intervals,clear status(%d)",usVid, portnum, ret));
    			gw_log(GW_LOG_LEVEL_MAJOR, "\r\nInterface eth1/%d no loop found in vlan %d for 10 intervals,clear status", portnum, usVid );
                LOOPBACK_DETECT_DEBUG(("\r\nsendOamLpbDetectNotifyMsg2,%d(ret=%d)", portnum, ret));
            }
            if((1== pCtrl->lpbmask[portnum])&&(10>=pCtrl->lpbClearCnt[portnum]&&(1==pCtrl->lpbportdown[portnum])))
            {
                LOOPBACK_DETECT_DEBUG(("\r\nVlan %d's lpbportwakeupcounter[%d] : %d, slpcounter[%d] : %d",usVid, portnum, pCtrl->lpbportwakeupcounter[portnum], portnum, pCtrl->slpcounter[portnum]));
                if(((!local_onu_lpb_detect_frame.enable)&&( pCtrl->lpbportwakeupcounter[portnum] < oam_onu_lpb_detect_frame.maxwakeup ) && 
                    ((++(pCtrl->slpcounter[portnum])) >= oam_onu_lpb_detect_frame.waitforwakeup))
                    ||((local_onu_lpb_detect_frame.enable)&&(pCtrl->lpbportwakeupcounter[portnum] < local_onu_lpb_detect_frame.maxwakeup)&&
                        ((++(pCtrl->slpcounter[portnum])) >= local_onu_lpb_detect_frame.waitforwakeup))
                    || ((-1 == oam_onu_lpb_detect_frame.maxwakeup)&&(local_onu_lpb_detect_frame.enable)&&((++(pCtrl->slpcounter[portnum])) >= local_onu_lpb_detect_frame.waitforwakeup)))
                {
					//IFM_admin_up(ethIfIndex, NULL, NULL);
					call_gwdonu_if_api(LIB_IF_PORT_ADMIN_SET, 2, portnum, PORT_ADMIN_UP);
                    Gwd_loop_port_wakeup_set(WAKEUP_ENABLE);
					if(GWD_RETURN_OK != (ret = sendOamLpbDetectNotifyMsg(portnum, 2, usVid, port_loop_back_session, &(pCtrl->alarmInfo[portnum]))))
					{
						LOOPBACK_DETECT_DEBUG(("sendOamLpbDetectNotifyMsg failed!"));		
					}
				    pCtrl->lpbportdown[portnum] = 0;
	                pCtrl->slpcounter[portnum] = 0;
	                pCtrl->lpbportwakeupcounter[portnum]++;
	                gulPortDownWhenLpbFound[portnum] = 0;
	                gw_log(GW_LOG_LEVEL_MAJOR, "Interface  eth%d/%d wakeup(%d) in vlan %d for loopback\n", 1, portnum, pCtrl->lpbportwakeupcounter[portnum], usVid);
                    LOOPBACK_DETECT_DEBUG(("\r\nVlan %d's port %d admin up and wakeupcouter++(%d)", usVid, portnum, ret));
                	LOOPBACK_DETECT_DEBUG(("\r\nsendOamLpbDetectNotifyMsg2,%d(ret=%d)", portnum, ret));
                }
            }
        }
    }

    gw_lpb_sem_give();
	
}

int gw_port_is_lpb_mark(unsigned short vid, unsigned short port)
{
	int ret = 0;
	OAM_ONU_LPB_DETECT_CTRL *pCtrl = NULL;

	if(port < gulNumOfPortsPerSystem )
	{
		gw_lpb_sem_take();
		
		pCtrl = getVlanLpbStasNode(vid);

		if(pCtrl)
		{
			if(pCtrl->lpbmask[port])
				ret = 1;
		}

		gw_lpb_sem_give();
	}

	return ret;
	
}

#if 0
extern gw_macaddr_t olt_mac_addr;
#endif

int lpbDetectTransFrames(unsigned short usVid)
#if 1
{
    int i, iRet;
    GWD_OLT_TYPE type;
    unsigned long ulSlot, ulPon;
    
    char sysMac[6] = {0}, olt_mac_addr[6]={0};
    char loopVid[2] = { 0 };
    unsigned char   LoopBackDetectPktBuffer[64], DevId;
    
    LOOP_DETECT_FRAME *packet_head = NULL;
	unsigned int OnuLlid;

//	unsigned int dport;
    gwd_port_oper_status_t lb_port_opr_status;

//	dport = ifm_port_id_make(ONU_GE_PORT, 0, EPON_PORT_UNI);
    
    call_gwdonu_if_api(LIB_IF_OLT_MAC_GET, 1, olt_mac_addr);

    gw_onu_get_local_mac(sysMac);    
    GwGetOltType(olt_mac_addr, &type);
    GwGetPonSlotPort(olt_mac_addr, type, &ulSlot, &ulPon);
    loopVid[0] = (unsigned char)((usVid >> 8) | 0xe0);
    loopVid[1] = (unsigned char)(0xff & usVid);

    memset(LoopBackDetectPktBuffer, 0, sizeof(LoopBackDetectPktBuffer));
    packet_head = (LOOP_DETECT_FRAME*)LoopBackDetectPktBuffer;
    
    memcpy(packet_head->Destmac, loop_detect_mac, 6);
    memcpy(packet_head->Srcmac, oam_onu_lpb_detect_frame.smac, 6);
    packet_head->Tpid = htons(0x8100);
    memcpy(packet_head->Vid, loopVid, 2);
    packet_head->Ethtype = htons(ETH_TYPE_LOOP_DETECT);
    packet_head->LoopFlag = htons(LOOP_DETECT_CHECK);
    packet_head->OltType = type;
    packet_head->OnuType = (unsigned char)DEVICE_TYPE_GT813_C;
    memset(packet_head->OnuLocation, 0, 4);
    memset(&packet_head->OnuLocation[1], (unsigned char)ulSlot, 1);
    memset(&packet_head->OnuLocation[2], (unsigned char)ulPon, 1);
//    epon_onu_llid_read(&OnuLlid);
    call_gwdonu_if_api(LIB_IF_ONU_LLID_GET, 1, &OnuLlid);
    memset(&packet_head->OnuLocation[3], (unsigned char)OnuLlid, 1);
    memcpy(packet_head->Onumac, sysMac, 6);
    packet_head->OnuVid =htons(usVid);
    packet_head->Onuifindex = 0;
    for (i = 0; i < 64 - 38; i++)
    {
	    LoopBackDetectPktBuffer[38 + i] = (unsigned char) i;
    }

    DevId = 1;
    for(i=0; i<gw_onu_read_port_num(); i++)
    {
        call_gwdonu_if_api(LIB_IF_PORT_OPER_STATUS_GET, 2, i+1, &lb_port_opr_status);
        if (lb_port_opr_status == PORT_OPER_STATUS_UP)			
        {        
		/*LoopBackDetectPktBuffer[12] = 0xC0 + DevId;*/			/* Use Forward DSA tag: forwarding tag/untag based on switch rule */
		/*LoopBackDetectPktBuffer[12] = 0x40 + DevId;*/		/* Use FromCPU DSA tag: forwarding tag/untag based on CPU control */
		/*LoopBackDetectPktBuffer[13] = (char)(i<<3);*/
		/*LoopBackDetectPktBuffer[12] = 0xFF;
		LoopBackDetectPktBuffer[13] = 0xFE;*/
		DUMPGWDPKT("\r\nLoopDetectPkt : ", i+1, LoopBackDetectPktBuffer, sizeof(LoopBackDetectPktBuffer));
//             iRet = epon_onu_sw_send_frame(i+1, LoopBackDetectPktBuffer, 64);
		iRet = call_gwdonu_if_api(LIB_IF_PORTSEND, 4, i+1, LoopBackDetectPktBuffer, 64);
		LOOPBACK_DETECT_DEBUG(("\r\nepon_onu_sw_send_frame(v%d,p%d) return %d.", usVid, i+1, iRet));
        }
        else
        {
			LOOPBACK_DETECT_DEBUG(("\r\nPort %d no need send for not up.", i+1));
        }
    }

    return GWD_RETURN_OK;
}
#else
{
        gw_ether_header_lb_t frame;
        gw_ether_header_lb_vlan_t frame1;
        gw_boolean opr = 0;
        epon_macaddr_t mac;
        gwd_port_oper_status_t lb_port_opr_status;
        epon_loop_detect_status tmp_status;
        unsigned char i = 0;
#ifdef HAVE_EXT_SW_DRIVER
        gw_return_code_t rc = GW_RETURN_SUCCESS;
        //gwd_port_admin_t lb_port_admin_status;
        epon_sw_vlan_config_t config;
        gw_boolean        result;
#endif
        epon_onu_port_status_t opr_cfg;
        unsigned int status;
        gwd_port_admin_t admin_status = ifm_port_admin_status_get(0x30400000);
        
        if(ifm_port_admin_status_get(0x30400000) != PORT_ADMIN_UP)
        {
			LOOPBACK_DETECT_DEBUG(("\r\nUNI port link down, can't send loop frame."));
            return GWD_RETURN_ERR;
        }
        
        epon_onu_ifm_port_status_read(0x30400000, &opr_cfg);
        lb_port_opr_status = opr_cfg.oper;
        if(lb_port_opr_status != PORT_OPER_STATUS_UP)
        {
			LOOPBACK_DETECT_DEBUG(("\r\nUNI port opr status down, can't send loop frame."));
            return GWD_RETURN_ERR;
        }
        
        //build frame
        memset(&frame, 0, sizeof(frame));
        memset(&frame1, 0, sizeof(frame1));
        memcpy(frame.dst, loop_detect_mac, EPON_MACADDR_LEN);
        memcpy(frame.src, onu_node.macaddr, EPON_MACADDR_LEN);
        frame.ethertype = htons(EPON_ETHERTYPE_MGMT);

        //set marvell port id in frame
        //send frame on all 4 marvell ports
        for(i=0; i<gw_onu_read_port_num(); i++)
        {
            //call_gwdonu_if_api(LIB_IF_PORT_ADMIN_GET, 2, i+1, &lb_port_admin_status);
            call_gwdonu_if_api(LIB_IF_PORT_OPER_STATUS_GET, 2, i+1, &lb_port_opr_status);
            //IROS_LOG_CRI(IROS_MODULE_ID_STP,"port %d opr %d\n", i+1, lb_port_opr_status);
            //IROS_LOG_CRI(IROS_MODULE_ID_STP, "oper status = %d , loopstatus = %d \n",lb_port_opr_status,loop_detect_status[i]);
            if (lb_port_opr_status == PORT_OPER_STATUS_UP)
            {        
                frame.lb_port = htonl(i+1);
                rc = epon_onu_sw_send_frame(i+1, (gw_uint8 *)&frame, sizeof(frame));
                //IROS_LOG_CRI(IROS_MODULE_ID_STP,"Send untag LD frame to port %d rc = %d\n", i+1, rc);
            }
        }
        memcpy(frame1.dst, mac, EPON_MACADDR_LEN);
        memcpy(frame1.src, onu_node.macaddr, EPON_MACADDR_LEN);
        frame1.ethertype = htons(EPON_ETHERTYPE_MGMT);
        frame1.tpid = htons(0x8100);
        frame1.lb_port = htonl(i+1);
        result = OK;
        memset(&config, 0, sizeof(config));
        while(1)
        {
            if(GW_RETURN_SUCCESS != epon_onu_sw_get_next_vlan_entry(0, &config, &result))
                    break;
            if(result == GW_FALSE)
                    break;
            frame1.vlan = htons(config.vlan);
            //IROS_LOG_CRI(IROS_MODULE_ID_STP,"Get vlan entry id %d portmap 0x%x result %d\n", config.vlan, config.tagged_portmap, result);
            for(i=0; i<gw_onu_read_port_num(); i++)
            {
                if(config.tagged_portmap & (1 << i))
                    continue;

                call_gwdonu_if_api(LIB_IF_PORT_OPER_STATUS_GET, 2, i+1, &lb_port_opr_status);
                //IROS_LOG_CRI(IROS_MODULE_ID_STP,"port %d opr %d\n", i+1, lb_port_opr_status);
                if (lb_port_opr_status == PORT_OPER_STATUS_UP) {
                    frame1.lb_port = htonl(i+1);
                    DUMPLDPKT("LD send", (gw_uint8 *)&frame1, sizeof(frame1));
                    rc = epon_onu_sw_send_frame(i+1, (gw_uint8 *)&frame1, sizeof(frame1));
                    //IROS_LOG_CRI(IROS_MODULE_ID_STP,"Send tag %d LD frame to port %d rc = %d\n", config.vlan, i+1, rc);
                }
            }
        }
}
#endif

unsigned long gulTmpForOnuPort; 	
void lpbDetectCheckMacTable(unsigned short usVid, char * oamSession)
{
//    OAM_ONU_LPB_DETECT_CTRL *pCtrl = getVlanLpbStasNode(usVid);
    int gtRet;
    unsigned char  *mac = oam_onu_lpb_detect_frame.smac;
    unsigned long   lport = 0;

	if(GW_RETURN_SUCCESS == (gtRet = gtGetSrcPortForMac(mac, usVid, &lport)))
	{
	    if (boards_port_is_uni(lport))
	    { /*find lport looped */
			LOOPBACK_DETECT_DEBUG(("\r\nLoopback detected on port %d/%lu in vlan %d.", 1, lport, usVid));
            LOOPBACK_DETECT_DEBUG(("\r\nSet lpbClear[%lu] = 0, lpbMask[%lu] = 1", lport, lport));
            if(gulLoopIgnorePort[lport])
            {
                LOOPBACK_DETECT_DEBUG(("\r\nPort %d/%lu ignore loopback detect", 1, lport));
            }
            else
            {
   /******************************************************************
   drop oam_onu_lpb_detect_frame.policy api;
   because,the judgment,can only be reported after the open loop contrl can make the loop
   *******************************************************************/
//#ifdef __LOOP_DEBUG__
#if 1
		    if(((!local_onu_lpb_detect_frame.enable)&&(oam_onu_lpb_detect_frame.policy&0x0001))||
		    	((local_onu_lpb_detect_frame.enable)&&(local_onu_lpb_detect_frame.policy&0x0001)))
#else
		    if((!local_onu_lpb_detect_frame.enable)||
		    	((local_onu_lpb_detect_frame.enable)&&(local_onu_lpb_detect_frame.policy&0x0001)))
#endif
        		{
        		    //unsigned long ulportif = IFM_ETH_CREATE_INDEX(PORTNO_TO_ETH_SLOT(lport), PORTNO_TO_ETH_PORTID(lport));
					{
            			gwd_port_admin_t status;
                        //IFM_GetIfAdminStatusApi(ulportif, &status);
                        call_gwdonu_if_api(LIB_IF_PORT_ADMIN_GET, 2, lport, &status);
                        LOOPBACK_DETECT_DEBUG(("\r\nport %lu admin status is : %d", lport, status));
                        if(status == PORT_ADMIN_UP)
                        { /* loopback port has not shuted down */
                    		//if(IFM_admin_down(ulportif, NULL, NULL) == GWD_RETURN_OK)
                    		if(call_gwdonu_if_api(LIB_IF_PORT_ADMIN_SET, 2, lport, PORT_ADMIN_DOWN) == GWD_RETURN_OK)
                    		{
                    			gulPortDownWhenLpbFound[lport] = 1;
                                setPortLpbStatus(usVid, lport, 1, oamSession, NULL);
                    			LOOPBACK_DETECT_DEBUG(("\r\nadmin down port %lu in vlan %d OK", lport, usVid));
                                LOOPBACK_DETECT_DEBUG(("\r\nset lpbportdown[%lu] = 1", lport));
//                                gw_log(GW_LOG_LEVEL_MAJOR, "Interface  eth%d/%lu marked loopback in vlan %d.\n", 1, lport, usVid);
//                                gw_log(GW_LOG_LEVEL_MAJOR, "Interface eth%d/%lu shut down for loopback\r\n",1,lport);
                    		}
                    		else
                    			LOOPBACK_DETECT_DEBUG(("\r\nadmin down fail for port %lu, in vlan %d", lport, usVid));
                        }
                    }
				}
		        else
				{
#if  0
                    if(pCtrl)
                    {
    	                 if(pCtrl->lpbmask[lport] != 1)
    	                 {
    	                      LOOPBACK_DETECT_DEBUG(("\r\nNot shutdown port , lpbmask[%lu] : %d", lport, pCtrl->lpbmask[lport]));
    	                      setPortLpbStatus(usVid, lport, 0, oamSession, NULL);
    	                      gw_log(GW_LOG_LEVEL_DEBUG, "Interface  eth%d/%lu marked loopback in vlan %u\n", 1, lport, usVid);
    	                 }
    					 else
    					 {
    					 	   pCtrl->lpbClearCnt[lport] = 0;
    					 }
                    }
#else
//                    if(pCtrl && pCtrl->lpbmask[lport] == 1)
//                    {
//						pCtrl->lpbClearCnt[lport] = 0;
//                    }
//					else
					{
//   	                      LOOPBACK_DETECT_DEBUG(("\r\nNot shutdown port , lpbmask[%lu] : %d", lport, pCtrl->lpbmask[lport]));
   	                      setPortLpbStatus(usVid, lport, 0, oamSession, NULL);
//   	                      gw_log(GW_LOG_LEVEL_MAJOR, "Interface  eth%d/%lu marked loopback in vlan %u\n", 1, lport, usVid);					
    				}
#endif
				}
            }
	    }
		else
	    {
		    LOOPBACK_DETECT_DEBUG(("\r\nno loopback port detected in vlan %d(p%lu)", usVid, lport));
	        clsPortLpbStatus( usVid, oamSession);
		}
	}
	else
	{
		LOOPBACK_DETECT_DEBUG(("\r\nLoopback detect mac lookup failed(%d) in vlan %d.", gtRet, usVid));
	    clsPortLpbStatus( usVid, oamSession);
	}
	
	return;
}

long lpbDetectRevPacketHandle(unsigned char *packet, unsigned long len, unsigned long slot, unsigned long port, unsigned short vid)
{
    int iRet, printFlag;
    unsigned char sysMac[6] = {0};
    LOOP_DETECT_FRAME_DATA *revLoopFrame;
    unsigned long ulLoopPort, onuIfindex, ulslot, ulport;

    if(NULL == packet)
        return GWD_RETURN_ERR;

    ulslot = 1;
    ulLoopPort = port;
    ulport = ulLoopPort;
    
    /*jiangxt added, 20111010.*/
    unsigned char SrcMac[6];
    memcpy(SrcMac, packet + 6, 6);
    if ((packet[12] == 0x81) && (packet[13] == 0x00))
    {
        LOOPBACK_DETECT_DEBUG(("\r\nLoopback frame with vlan tag. "));
        revLoopFrame = (LOOP_DETECT_FRAME_DATA *)(packet + 16);
    }
    else
    {
        LOOPBACK_DETECT_DEBUG(("\r\nLoopback frame with no vlan tag. "));
        revLoopFrame = (LOOP_DETECT_FRAME_DATA *) (packet + 12);
    }
    /*add end*/

    gw_onu_get_local_mac(sysMac);    
    if(ntohs(revLoopFrame->LoopFlag) != LOOP_DETECT_CHECK)
    {
        LOOPBACK_DETECT_DEBUG(("\r\nLoopFlag error "));
        return GWD_RETURN_ERR;
    }
    iRet = GWD_RETURN_OK;

    if(!gulLoopIgnorePort[ulLoopPort])
    {
        /* if the loop port is PON port, report the first member in VLAN of vid */
        if(!boards_port_is_uni(ulLoopPort))
        {
            LOOPBACK_DETECT_DEBUG(("\r\nThe loop port is PON port!"));
            unsigned long lport;

            if(IFM_GET_FIRST_PORTONVLAN(&lport, vid) == GWD_RETURN_OK)
            {
                ulLoopPort = lport;
            }     
	  }
    }
    else /*I move the else out from the upper if loop, jiangxt, 20111010.*/
    {
	  LOOPBACK_DETECT_DEBUG(("\r\nport %lu/%lu ignored loop detect", ulslot, ulLoopPort));
	  return GWD_RETURN_OK;
    }
    
    gulLoopRecFlag[ulLoopPort]  = 1;
    memset(&sendLoopAlarmOam, 0, sizeof(ALARM_LOOP));
    sendLoopAlarmOam.oltType = revLoopFrame->OltType;
    sendLoopAlarmOam.onuType = revLoopFrame->OnuType;
    memcpy(sendLoopAlarmOam.onuLocation,revLoopFrame->OnuLocation, 4);
    memcpy(sendLoopAlarmOam.onuMac, revLoopFrame->Onumac, 6);
    onuIfindex = revLoopFrame->Onuifindex;
    if(onuIfindex != 0)
    {
        sendLoopAlarmOam.onuPort[2]= IFM_ETH_GET_SLOT(onuIfindex);
        sendLoopAlarmOam.onuPort[3] = IFM_ETH_GET_PORT(onuIfindex);
    }
    else
    	memset(sendLoopAlarmOam.onuPort, 0, 4);
    printFlag = 0;
    if(revLoopFrame->OnuLocation[1] >= 1&&revLoopFrame->OnuLocation[1] <= MAX_GWD_OLT_SLOT
    	&&revLoopFrame->OnuLocation[2] >= 1&&revLoopFrame->OnuLocation[2] <= MAX_GWD_OLT_PORT
        &&revLoopFrame->OltType >= GWD_OLT_NONE &&revLoopFrame->OltType <= GWD_OLT_NOMATCH)
    {
        printFlag = 1;
    }
                
    memcpy(sendLoopAlarmOam.oltMac, SrcMac, 6);
   /******************************************************************
   drop oam_onu_lpb_detect_frame.policy api;
   because,the judgment,can only be reported after the open loop contrl can make the loop
   *******************************************************************/
//#ifdef __LOOP_DEBUG__
#if 1
    if(((!local_onu_lpb_detect_frame.enable)&&(oam_onu_lpb_detect_frame.policy&0x0001))||
    	((local_onu_lpb_detect_frame.enable)&&(local_onu_lpb_detect_frame.policy&0x0001)))
#else
    if((!local_onu_lpb_detect_frame.enable)||
    	((local_onu_lpb_detect_frame.enable)&&(local_onu_lpb_detect_frame.policy&0x0001)))
#endif
    {
        LOOPBACK_DETECT_DEBUG(("\r\nBegin check port status!"));
        gwd_port_admin_t status;
        call_gwdonu_if_api(LIB_IF_PORT_ADMIN_GET, 2, ulport, &status);
        if(PORT_ADMIN_UP == status)
        { /* loopback port has not shuted down */
            if(call_gwdonu_if_api(LIB_IF_PORT_ADMIN_SET, 2, ulport, PORT_ADMIN_DOWN) == GWD_RETURN_OK)
            {
                LOOPBACK_DETECT_DEBUG(("\r\nprintFlag : %d", printFlag));
                gulPortDownWhenLpbFound[ulLoopPort] = 1;
                LOOPBACK_DETECT_DEBUG(("\r\nreceive packet,admin down port %lu/%lu in vlan %d OK", ulslot, ulport, vid));

                if(0 == printFlag)
                {
#if 0					
                	gw_log(GW_LOG_LEVEL_CRI, "Interface  eth%lu/%lu marked loopback in vlan %d.\n", ulslot, ulport, vid);
                	gw_log(GW_LOG_LEVEL_CRI, "Interface  eth%lu/%lu loop[%s(%02x%02x.%02x%02x.%02x%02x)%d/%d %s(%02x%02x.%02x%02x.%02x%02x)V(%d)]\n",
                    ulslot, ulport, (revLoopFrame->OltType == 1)?"GFA6100":"GFA6700", SrcMac[0], SrcMac[1],SrcMac[2],SrcMac[3],SrcMac[4],SrcMac[5],
                    revLoopFrame->OnuLocation[1], revLoopFrame->OnuLocation[2], onu_product_name_get(revLoopFrame->OnuType), revLoopFrame->Onumac[0],
                    revLoopFrame->Onumac[1],revLoopFrame->Onumac[2],revLoopFrame->Onumac[3],revLoopFrame->Onumac[4],revLoopFrame->Onumac[5],vid);
#endif					
                }
                else if(onuIfindex == 0)
                {
                	gw_log(GW_LOG_LEVEL_CRI, "Interface  eth%lu/%lu loop[%s(%02x%02x.%02x%02x.%02x%02x)%d/%d %s(%02x%02x.%02x%02x.%02x%02x)V(%d)]\n",
                    ulslot, ulport, olt_type_string[revLoopFrame->OltType]/*(revLoopFrame->OltType == 1)?"GFA6100":"GFA6700"*/, SrcMac[0], SrcMac[1],SrcMac[2],SrcMac[3],SrcMac[4],SrcMac[5],
                    revLoopFrame->OnuLocation[1], revLoopFrame->OnuLocation[2], onu_product_name_get(revLoopFrame->OnuType), revLoopFrame->Onumac[0],
                    revLoopFrame->Onumac[1],revLoopFrame->Onumac[2],revLoopFrame->Onumac[3],revLoopFrame->Onumac[4],revLoopFrame->Onumac[5],vid);
                }
                else
                {
                	gw_log(GW_LOG_LEVEL_CRI, "Interface  eth%lu/%lu loop[%s(%02x%02x.%02x%02x.%02x%02x)%d/%d %s(%02x%02x.%02x%02x.%02x%02x)P(%d/%d)V(%d)]\n",
                    ulslot, ulport, olt_type_string[revLoopFrame->OltType]/*(revLoopFrame->OltType == 1)?"GFA6100":"GFA6700"*/, SrcMac[0], SrcMac[1],SrcMac[2],SrcMac[3],SrcMac[4],SrcMac[5],
                    revLoopFrame->OnuLocation[1], revLoopFrame->OnuLocation[2],onu_product_name_get(revLoopFrame->OnuType), revLoopFrame->Onumac[0],revLoopFrame->Onumac[1],
                    revLoopFrame->Onumac[2],revLoopFrame->Onumac[3],revLoopFrame->Onumac[4],revLoopFrame->Onumac[5],sendLoopAlarmOam.onuPort[2], sendLoopAlarmOam.onuPort[3],vid);
                }

		setPortLpbStatus(vid, ulLoopPort, 1, port_loop_back_session, (void *)&sendLoopAlarmOam);
                reportPortsLpbStatus(vid, port_loop_back_session);
            }
            else
                LOOPBACK_DETECT_DEBUG(("\r\nreceiver packet,admin down fail for port %lu/%lu, in vlan %d", ulslot, ulport, vid));
        }
        else
        {
            LOOPBACK_DETECT_DEBUG(("\r\n loop pkt reveived from down port!\r\n"));
        }
    }
    else
    {
//        OAM_ONU_LPB_DETECT_CTRL *pCtrl = getVlanLpbStasNode(vid);

#if 0
        if(pCtrl)
        {
            if(pCtrl->lpbmask[ulLoopPort] != 1)
            {
                LOOPBACK_DETECT_DEBUG(("\r\nNot shutdown port , lpbmask[%lu] : %d", ulLoopPort, pCtrl->lpbmask[ulLoopPort]));
    	     	setPortLpbStatus(vid, ulLoopPort, 0, port_loop_back_session, (void *)&sendLoopAlarmOam);

                if(0 == printFlag)
                {
                    gw_log(GW_LOG_LEVEL_DEBUG, "Interface  eth%lu/%lu marked loopback in vlan %d.\n", ulslot, ulport, vid);
                }
                else if(onuIfindex == 0)
                {
                    gw_log(GW_LOG_LEVEL_CRI, "Interface  eth%lu/%lu loop[%s(%02x%02x.%02x%02x.%02x%02x)%d/%d %s(%02x%02x.%02x%02x.%02x%02x)V(%d)]\n",
                    ulslot, ulport, (revLoopFrame->OltType == 1)?"GFA6100":"GFA6700", SrcMac[0], SrcMac[1],SrcMac[2],SrcMac[3],SrcMac[4],SrcMac[5],
                    revLoopFrame->OnuLocation[1], revLoopFrame->OnuLocation[2], onu_product_name_get(revLoopFrame->OnuType), revLoopFrame->Onumac[0],
                    revLoopFrame->Onumac[1],revLoopFrame->Onumac[2],revLoopFrame->Onumac[3],revLoopFrame->Onumac[4],revLoopFrame->Onumac[5],vid);
                }
                else
                {
                    gw_log(GW_LOG_LEVEL_CRI, "Interface  eth%lu/%lu loop[%s(%02x%02x.%02x%02x.%02x%02x)%d/%d %s(%02x%02x.%02x%02x.%02x%02x)P(%d/%d)V(%d)]\n",
                    ulslot, ulport, (revLoopFrame->OltType == 1)?"GFA6100":"GFA6700",SrcMac[0], SrcMac[1],SrcMac[2],SrcMac[3],SrcMac[4],SrcMac[5], 
                    revLoopFrame->OnuLocation[1], revLoopFrame->OnuLocation[2],onu_product_name_get(revLoopFrame->OnuType), revLoopFrame->Onumac[0],revLoopFrame->Onumac[1],
                    revLoopFrame->Onumac[2],revLoopFrame->Onumac[3],revLoopFrame->Onumac[4],revLoopFrame->Onumac[5],sendLoopAlarmOam.onuPort[2], sendLoopAlarmOam.onuPort[3],vid);
                }
            }
            else
            {
    	      pCtrl->lpbClearCnt[ulLoopPort] = 0;
            }
        }
#else

#if 0
	  if(pCtrl && pCtrl->lpbmask[ulLoopPort] == 1)
	  {
          LOOPBACK_DETECT_DEBUG(("\r\nNot shutdown port , lpbmask[%lu] : %d", ulLoopPort, pCtrl->lpbmask[ulLoopPort]));	  	
	  	  pCtrl->lpbClearCnt[ulLoopPort] = 0;
	  }
	  else
#endif	  	
	  {
		  if(!gw_port_is_lpb_mark(vid, ulLoopPort))
		  {

				  if(0 == printFlag)
				  {
//					  gw_log(GW_LOG_LEVEL_CRI, "Interface  eth%lu/%lu marked loopback in vlan %d.\n", ulslot, ulport, vid);
				  }
				  else if(onuIfindex == 0)
				  {
					  gw_log(GW_LOG_LEVEL_CRI, "Interface  eth%lu/%lu loop[%s(%02x%02x.%02x%02x.%02x%02x)%d/%d %s(%02x%02x.%02x%02x.%02x%02x)V(%d)]\n",
					  ulslot, ulport, olt_type_string[revLoopFrame->OltType]/*(revLoopFrame->OltType == 1)?"GFA6100":"GFA6700"*/, SrcMac[0], SrcMac[1],SrcMac[2],SrcMac[3],SrcMac[4],SrcMac[5],
					  revLoopFrame->OnuLocation[1], revLoopFrame->OnuLocation[2], onu_product_name_get(revLoopFrame->OnuType), revLoopFrame->Onumac[0],
					  revLoopFrame->Onumac[1],revLoopFrame->Onumac[2],revLoopFrame->Onumac[3],revLoopFrame->Onumac[4],revLoopFrame->Onumac[5],vid);
				  }
				  else
				  {
					  gw_log(GW_LOG_LEVEL_CRI, "Interface  eth%lu/%lu loop[%s(%02x%02x.%02x%02x.%02x%02x)%d/%d %s(%02x%02x.%02x%02x.%02x%02x)P(%d/%d)V(%d)]\n",
					  ulslot, ulport, olt_type_string[revLoopFrame->OltType]/*(revLoopFrame->OltType == 1)?"GFA6100":"GFA6700"*/,SrcMac[0], SrcMac[1],SrcMac[2],SrcMac[3],SrcMac[4],SrcMac[5],
					  revLoopFrame->OnuLocation[1], revLoopFrame->OnuLocation[2],onu_product_name_get(revLoopFrame->OnuType), revLoopFrame->Onumac[0],revLoopFrame->Onumac[1],
					  revLoopFrame->Onumac[2],revLoopFrame->Onumac[3],revLoopFrame->Onumac[4],revLoopFrame->Onumac[5],sendLoopAlarmOam.onuPort[2], sendLoopAlarmOam.onuPort[3],vid);
				  }
		  }

		  setPortLpbStatus(vid, ulLoopPort, 0, port_loop_back_session, (void *)&sendLoopAlarmOam);
	  }
#endif
    }
	
    return iRet;
}

long ethLoopBackDetectActionCall( int enable, char * oamSession)
{
	int ret;
//    epon_sw_vlan_config_t vlan_entry;
//    gw_boolean result;
	int need_to_send;
	unsigned long lport, tag_portlist, untag_portlist;
	unsigned long i;
       gwd_port_oper_status_t status;
	unsigned short vid =0;
    unsigned int wakestat= 0;
    
	if(enable)
	{
		LOOPBACK_DETECT_DEBUG(("\r\n----------^_^ new loopback detect interval start here ^_^----------"));
		resetLpbPort(0, oamSession);

		if((!local_onu_lpb_detect_frame.enable)&&(oam_onu_lpb_detect_frame.vid != 0))
		{ 	/*detect loopback in the specific vlan, wakeup the shutdown ports which have not reached wakeup threshold in the specfic vlan*/
			LOOPBACK_DETECT_DEBUG(("\r\nOLT config detect in unique vlan(%d)",oam_onu_lpb_detect_frame.vid));
			lpbDetectWakeupPorts(oam_onu_lpb_detect_frame.vid);
			/*Send detect packet in specific vlan*/
			if(GWD_RETURN_OK != (ret = lpbDetectTransFrames(oam_onu_lpb_detect_frame.vid)))
			LOOPBACK_DETECT_DEBUG(("\r\nlpbDetectTransFrames failed(%d).", ret));
			/*detect loopback in the specific vlan, check mac table int the specific vlan*/
			lpbDetectCheckMacTable(oam_onu_lpb_detect_frame.vid, oamSession);
			reportPortsLpbStatus(oam_onu_lpb_detect_frame.vid, oamSession);
		}
		else if((local_onu_lpb_detect_frame.enable)&&(local_onu_lpb_detect_frame.vid != 0))
		{
			LOOPBACK_DETECT_DEBUG(("\r\nONU config detect in unique vlan(%d)", local_onu_lpb_detect_frame.vid));
			lpbDetectWakeupPorts(local_onu_lpb_detect_frame.vid);
			/*Send detect packet in specific vlan*/
			if(GWD_RETURN_OK != (ret = lpbDetectTransFrames(local_onu_lpb_detect_frame.vid)))
			LOOPBACK_DETECT_DEBUG(("\r\nlpbDetectTransFrames failed(%d).", ret));
			/*detect loopback in the specific vlan, check mac table int the specific vlan*/
			lpbDetectCheckMacTable(local_onu_lpb_detect_frame.vid, oamSession);
			reportPortsLpbStatus(local_onu_lpb_detect_frame.vid, oamSession);
		}
		else
             {
             	unsigned long int idx  = 0;

	        	LOOPBACK_DETECT_DEBUG(("\r\nDetect in all vlan"));
//	        	memset(&vlan_entry, 0, sizeof(vlan_entry));		
	        	while(1)
	        	{
//		             if(GW_RETURN_SUCCESS != epon_onu_sw_get_next_vlan_entry(0, &vlan_entry, &result))
			    if(GW_RETURN_SUCCESS != call_gwdonu_if_api(LIB_IF_VLAN_ENTRY_GETNEXT, 4,  idx,  &vid, &tag_portlist, &untag_portlist))
		                    break;
				
//		             if(result == GW_FALSE)
//		                    break;
	        		/* wakeup all the shutdown ports which have not reached wakeup threshold */
//	        		vid = vlan_entry.vlan;
	        		if(vid != 0)
	        		{
	        	  	  	lpbDetectWakeupPorts(vid);
                        /**************************************************************
                                        *·¢ÏÖÓÐ»·Â·¶Ë¿Ú»½ÐÑ£¬ÐèÒª×öÑÓÊ±2S£¬µÈ´ý¶Ë¿Ú×´Ì¬UP
                                        **************************************************************/
                        Gwd_loop_port_wakeup_get(&wakestat);
                        if(wakestat == WAKEUP_ENABLE)
                        {   
                            wakestat = WAKEUP_DISABLE;
                            gw_thread_delay(2000);
                            Gwd_loop_port_wakeup_set(wakestat);
                        }
					need_to_send = 0;
					/* VLAN without linkup ports, no need check, but need update loopback status */
					for(i=0; i<PHY_PORT_MAX; i++)
					{
//						if((vlan_entry.tagged_portmap & (1<<i)) || (vlan_entry.untagged_portmap & (1<<i)))
						if((tag_portlist & (1<<i) )|| (untag_portlist & (1<<i)))
						{
							if(boards_physical_to_logical(0, i, &lport))
							{
								call_gwdonu_if_api(LIB_IF_PORT_OPER_STATUS_GET, 2, lport, &status);
								if(status == PORT_OPER_STATUS_UP)
								{
					    		            need_to_send = 1;
					    		            break;
					    		        }
		            				}
			            		}
			        	}

        			LOOPBACK_DETECT_DEBUG(("\r\nVlan %d %s check.", vid, (1 == need_to_send)?"need":"no need"));
			        if (1 == need_to_send)
			        {
			            need_to_send = 0;
						/*check the MacTable first ,in case that the ports looped in defferent vlan cannot be detected */
			            lpbDetectCheckMacTable(vid,oamSession);   
		
	    				if(GWD_RETURN_OK != (ret = lpbDetectTransFrames(vid)))
	                    	LOOPBACK_DETECT_DEBUG(("\r\nlpbDetectTransFrames failed(%d).", ret));

	                }
	                else
	                {
                       	clsPortLpbStatus(vid, oamSession);
	                }
					
        			reportPortsLpbStatus(vid, oamSession);
                }
                idx++;
        	}
        }
	}
	else
	{
		LOOPBACK_DETECT_DEBUG(("\r\ndisable loopback detect"));
		resetLpbPort(1, oamSession);
		freeLpbStatusList();
	}

	/*check local port loop alarm and set loop alarm led*/
	gwdEthPortLoopLedAction();

	return 0;	
}

int gw_loopbackFrameParser( gw_uint8  *frame, gw_uint32  len)
{
    gw_ether_header_lb_t *plb_frame = NULL;
	LOOP_DETECT_FRAME_DATA  * pd = NULL;

	unsigned short ether_type;

//    LOOPBACK_DETECT_DEBUG(("\r\nloopbackFrameParser Func IN! "));

    if ((len==0) || (frame==NULL)) 
    {
	LOOPBACK_DETECT_DEBUG(("\r\nloopbackFrameParser fail, buff empty! "));	
        return GW_PKT_MAX;
    }

    plb_frame = (gw_ether_header_lb_t *)frame;
    ether_type = ntohs(plb_frame->ethertype);
    if (ether_type == EPON_ETHERTYPE_DOT1Q) 
    {
	pd = (LOOP_DETECT_FRAME_DATA *)(frame+16);	
    }
    else
	pd = (LOOP_DETECT_FRAME_DATA *)(frame+12);

	if(pd->Ethtype == ntohs(ETH_TYPE_LOOP_DETECT) && pd->LoopFlag == ntohs(LOOP_DETECT_CHECK))
	{
		LOOPBACK_DETECT_DEBUG(("\r\nloopbackFrameParser ok! "));
		return GW_PKT_LPB;
	}


	return GW_PKT_MAX;
    
}

int loopbackFrameRevHandle(gw_uint32  portid ,gw_uint32  len, gw_uint8  *frame)
{
    gw_ether_header_lb_t *plb_frame = NULL;
	unsigned short vid;
	unsigned short ether_type;

    LOOPBACK_DETECT_DEBUG(("\r\nloopbackFrameRevHandle Func IN! "));

    if ((len==0) || (frame==NULL)) 
    {
        return 0;	// No need handle
    }

    plb_frame = (gw_ether_header_lb_t *)frame;
    ether_type = ntohs(plb_frame->ethertype);
    if (ether_type == EPON_ETHERTYPE_DOT1Q) 
    {
        gw_ether_header_lb_vlan_t *plb_vlan_frame = (gw_ether_header_lb_vlan_t *)frame;
        vid = ntohs(plb_vlan_frame->vlan);
        vid &= 0x0FFF;
    }
    else
    {
    	vid = 1;
    }
    
    LOOPBACK_DETECT_DEBUG(("\r\n************************************ "));
    DUMPGWDPKT("\r\nLoopDetectFrameRevPKT : ", portid, frame, len);
    LOOPBACK_DETECT_DEBUG(("\r\n************************************ "));
    
    return lpbDetectRevPacketHandle(frame, len, 1, portid, vid);
}

int gw_loopbackFrameRevHandle(gw_uint8  *frame, gw_uint32  len, gw_uint32  portid )
{
	return loopbackFrameRevHandle(portid, len, frame);
}


#if 0
extern void epon_onu_start_alarm_led();
extern void epon_onu_stop_alarm_led();
#endif

//static unsigned long int g_portloopstatus = 0;
void
onu_event_port_loop(gwd_ethloop_msg_t *msg)
{

//    g_portloopstatus |= 1<<msg->portid;
#if 0
    epon_onu_start_alarm_led();
#else
    call_gwdonu_if_api(LIB_IF_ONU_START_LOOP_LED, 0, NULL);
#endif
}

void
onu_event_port_loop_clear(gwd_ethloop_msg_t *msg)
{

//	g_portloopstatus &= ~(1<<msg->portid);

//    if(!g_portloopstatus)
#if 0
    	epon_onu_stop_alarm_led();
#else
    	call_gwdonu_if_api(LIB_IF_ONU_STOP_LOOP_LED, 0, NULL);
#endif

}

#if 0
int gwdEthPortLoopMsgBuildAndSend(unsigned long int status)
{
    extern cyg_handle_t m0;
    gwd_ethloop_msg_t *msg = NULL;

    msg = malloc( sizeof(gwd_ethloop_msg_t));
    if (NULL == msg) {
        //IROS_LOG_CRI(IROS_MODULE_ID_EVENT, "memory is not enough!\n");
        return (-1);
    }

    msg->msgtype = IROS_MSG_GWD_PORT_LOOP_EVENT;
#if 0
    msg->portid = portid;
    msg->vid = vid;
#endif
    msg->loopstatus = status;

    if (!cyg_mbox_timed_put(m0, msg, cyg_current_time() + 100)) {
        //IROS_LOG_CRI(IROS_MODULE_ID_EVENT, "too many gwd port loop event!\n");
        free(msg);
    }

    return 0;
}
#endif

int gwdEthPortLoopLedAction()
{
	int ret = GWD_RETURN_ERR;
	LPB_CTRL_LIST *pNode = NULL;

	OAM_ONU_LPB_DETECT_CTRL *pCtrl = NULL;

	int found = 0;

	gw_lpb_sem_take();

	pNode = g_lpb_detect_ctrl_head;
	
	while(pNode != NULL)
	{
		pCtrl = pNode->ctrlnode;
		if(pCtrl)
		{
			int iport = 0;
			for(iport=1; iport < NUM_PORTS_PER_SYSTEM; iport++)
			{
				if(pCtrl->lpbmask[iport])
				{
					found = 1;
					break;
				}
			}
			if(found)
				break;
		}
		pNode = pNode->next;
	}

	gw_lpb_sem_give();

//	ret = gwdEthPortLoopMsgBuildAndSend(found?GWD_ETH_PORT_LOOP_ALARM:GWD_ETH_PORT_LOOP_ALARM_CLEAR);

	ret = call_gwdonu_if_api(LIB_IF_PORT_LOOP_EVENT_POST, 1, found?GWD_ETH_PORT_LOOP_ALARM:GWD_ETH_PORT_LOOP_ALARM_CLEAR);

	return ret;
}

/*end*/

