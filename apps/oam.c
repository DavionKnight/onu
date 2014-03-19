#include <string.h>
#include <math.h>

//#include "cli_common.h"
#include "../include/gw_os_api_core.h"
#include "../include/gw_timer.h"
#include "../cli_lib/cli_common.h"
#include "gw_log.h"
#include "oam_core.h"
#include "oam.h"
#include "gwdonuif_interval.h"
#include "pkt_main.h"
#include "oamsnmp.h"
#include "gw_version.h"

//#include "sdl_api.h"

#define OAMDBGERR               diag_printf
extern broadcast_storm_s broad_storm;
//cs_llid_t active_pon_port = CS_PON_PORT_ID;

unsigned char gwdOamTrace = 0;
#define GWDOAMTRC               if(gwdOamTrace) gw_printf

#ifndef TOUPPER
#define TOUPPER(c)	(c >= 'a' && c <= 'z' ? c - ('a' - 'A') : c)
#endif

char GwONU831BootVer[]="V1.0";
char GwOUI[3] = {0x00, 0x0f, 0xe9};
char CTCOUI[3] = {0x11, 0x11, 0x11}; 
char gwd_vendor_id[4] = { 'G', 'W', 'D', 'L' }; 


unsigned long file_op_msg_que = 0;
unsigned long cli_msg_que = 0;
GWTT_OAM_MESSAGE_NODE GwOamMessageListHead={0};
//cyg_sem_t	OamListSem;
gw_uint32 OamListSem;
unsigned long gulOamSendSerNo = 1;
unsigned long	gulGwOamConnect = 0;

#ifndef PPPOE_RELAY_PACKET_DEBUG
#define PPPOE_RELAY_PACKET_DEBUG(str) if( gulDebugPppoeRrelay ){ OAMDBGERR str ;}
#endif

#ifndef DHCP_RELAY_PACKET_DEBUG
#define DHCP_RELAY_PACKET_DEBUG(str) if( gulDebugEthDhcpSwitch ){ OAMDBGERR str ;}
#endif

const unsigned char SYS_SOFTWARE_MAJOR_VERSION_NO = 2;
const unsigned char SYS_SOFTWARE_RELEASE_VERSION_NO = 3;
const unsigned char SYS_SOFTWARE_BRANCH_VERSION_NO = 2;
const unsigned char SYS_SOFTWARE_DEBUG_VERSION_NO = 1;

const unsigned char SYS_HARDWARE_MAJOR_VERSION_NO = 2;
const unsigned char SYS_HARDWARE_RELEASE_VERSION_NO = 1;
const unsigned char SYS_HARDWARE_BRANCH_VERSION_NO = 1;
const unsigned char SYS_HARDWARE_DEBUG_VERSION_NO = 1;


static int GwOamInformationRequest(GWTT_OAM_MESSAGE_NODE *pRequest );
static int GwOamMessageListNodeRem(GWTT_OAM_MESSAGE_NODE *pNode);
static int GwOamAlarmResponse(GWTT_OAM_MESSAGE_NODE *pRequest );
short int CtcOnuMsgReveive(CTC_OAM_MESSAGE_NODE **ppMessage,unsigned char *MessagData, unsigned short PayloadLength);

#if 0
void OamMessageRecevTimeOut(epon_timer_t *timer);
#else
void OamMessageRecevTimeOut(void *);
#endif
int sys_show_version_descript_to_buf(char *buf);
char *mn_get_sysname(void);
unsigned long   Igmp_Snoop_OAM_Req_Recv(GWTT_OAM_MESSAGE_NODE *pRequest);
unsigned long   Igmp_Snoop_OAM_Resp_Recv(GWTT_OAM_MESSAGE_NODE *pResponse);
int Debug_Print_Rx_OAM(GWTT_OAM_MESSAGE_NODE *pMessage);
int Debug_Print_Tx_OAM(GWTT_OAM_HEADER *avender, unsigned char *pSentData);
int Debug_Print_Rx_OAM_Unkown(unsigned char *pBuffer, unsigned short len);



extern int gw_cli_arg_help(struct cli_def *cli, int cr_ok, char *entry, ...);
extern void gw_cli_print(struct cli_def *cli, char *format, ...);

unsigned long   gulDebugOamRx = 0;
unsigned long   gulDebugOamTx = 0;
unsigned long   gulDebugOamRxCount = 0;
unsigned long   gulDebugOamTxCount = 0;
unsigned long   gulDebugOamFileOp = 0;

OAM_ONU_LPB_DETECT_FRAME oam_onu_lpb_detect_frame, tframe;
unsigned long lpb_detect_timeout_timer = 0;

unsigned long gulIpAddrInEeprom;

ONU_SYS_INFO_TOTAL gw_onu_system_info_total;
unsigned char *pStrGwdSwVer;
unsigned char *pStrGwdHwVer;

#if 0
struct {
    epon_timer_t    timer;
} gwd_oam_timer;
#endif

unsigned char *irosbootver = " boot ";

#define IROS_FW_VER_MAJOR "1"
#define IROS_FW_VER_MINOR "1"
#define IROS_FW_VER_BUILD "001"

unsigned char *iros_version = " ONU" \
        IROS_FW_VER_MAJOR"." \
        IROS_FW_VER_MINOR"."\
        IROS_FW_VER_BUILD" "__DATE__" "__TIME__"\n";

/*packet send sem declare*/
gw_uint32 g_pkt_send_sem;
gw_int8 g_pkt_send_sem_name[] = "pkt_send_sem";

int GW_Onu_Sysinfo_Get(void);
int GW_Onu_Sysinfo_Save(void);

int GwOamMessageListInit(void)
{
//	cyg_semaphore_init(&OamListSem, 1);
	gw_semaphore_init(&OamListSem, "gwOamSem", 1, 0);
	return 0;
}


/*******************************************************************
* GwOadMessageListGetNode
********************************************************************/

static GWTT_OAM_MESSAGE_NODE *GwOamMessageListGetNode(unsigned int SerNo)
{
	GWTT_OAM_MESSAGE_NODE *pNode;

	GWDOAMTRC("  GWD OAM handler - GwOamMessageListGetNode - SerNo: 0x%x.\n", SerNo);
//	cyg_semaphore_wait(&OamListSem);
	gw_semaphore_wait(OamListSem, GW_OSAL_WAIT_FOREVER);
	GWDOAMTRC("  GWD OAM handler - GwOamMessageListGetNode - semWait: ok.\n");
	pNode = GwOamMessageListHead.next;
	while(NULL!=pNode)
	{
	
		if(SerNo==pNode->SendSerNo)
		{
			break;
		}
		else
		{
			pNode=pNode->next;
		}
	}
//	cyg_semaphore_post(&OamListSem);
	gw_semaphore_post(OamListSem);
	return pNode;

}

/*******************************************************************
* GwOadMessageListNodeNew
********************************************************************/

static GWTT_OAM_MESSAGE_NODE *GwOamMessageListNodeNew(unsigned short MessLen)
{
	GWTT_OAM_MESSAGE_NODE *pNode=NULL;

//	pNode = iros_malloc(IROS_MID_OAM, sizeof(GWTT_OAM_MESSAGE_NODE));
	pNode = malloc(sizeof(GWTT_OAM_MESSAGE_NODE));
	if(pNode)
	{
		memset(pNode, '\0', sizeof(GWTT_OAM_MESSAGE_NODE));
	}
	else
	{
		return NULL;
	}

	if(0 !=MessLen )
	{
//		pNode->pPayLoad = iros_malloc(IROS_MID_OAM, MessLen+1);
		pNode->pPayLoad = malloc(MessLen+1);
		if(NULL != pNode->pPayLoad )
		{
			memset(pNode->pPayLoad, '\0', MessLen+1);
		}
		else
		{
//			iros_free(pNode);
			free(pNode);
			return NULL;
		}
	}
	return pNode;
}
#if 0
/*******************************************************************
* CtcOamMessageListNodeNew
*
*******************************************************************/

static CTC_OAM_MESSAGE_NODE *CtcOamMessageListNodeNew(unsigned short MessLen)
{
	CTC_OAM_MESSAGE_NODE *pNode=NULL;

	pNode = VOS_Malloc(sizeof(CTC_OAM_MESSAGE_NODE), MODULE_RPU_OAM);
	if(NULL!=pNode)
	{
		memset(pNode, '\0', sizeof(CTC_OAM_MESSAGE_NODE));
	}
	else
	{
		return NULL;
	}

	if(0 !=MessLen )
	{
		pNode->pPayLoad = VOS_Malloc(MessLen+1, MODULE_RPU_OAM);
		if(NULL != pNode->pPayLoad )
		{
			memset(pNode->pPayLoad, '\0', MessLen+1);
		}
		else
		{
			VOS_Free(pNode);
			return NULL;
		}
	}
	return pNode;
}

/*******************************************************************
* CtcOamMessageListNodeFree 
*
*******************************************************************/
void CtcOamMessageListNodeFree(CTC_OAM_MESSAGE_NODE *pNode)
{
	if(NULL == pNode)
		return;
	
	if(NULL != pNode->pPayLoad)
		VOS_Free(pNode->pPayLoad);
	VOS_Free(pNode);

	return;
}

/*******************************************************************
* CtcOamRecevOpcodeCheck 
*
*******************************************************************/
int CtcOamRecevOpcodeCheck(unsigned char CtcOpcode )
{
	switch(CtcOpcode)
	{
		case Extended_Variable_Request:
		case Extended_Variable_Set_Request:
		case Extended_Variable_Churning:
		case Extended_Variable_DBA:
			break;
		default:
			return OAM_MESSAGE_RECEV_OPCODE_ERR;
	}
	return OAM_MESSAGE_RECEV_OPCODE_OK;
}

#endif


/*******************************************************************
* GwOadMessageListNodeFree 
*
*******************************************************************/
void GwOamMessageListNodeFree(GWTT_OAM_MESSAGE_NODE *pNode)
{
	if(NULL == pNode)
		return;
	if(GWD_RETURN_OK!=GwOamMessageListNodeRem(pNode))
	{
//		IROS_LOG_CRI(IROS_MID_OAM, "GwOamMessageListNodeFree::GwOamMessageListNodeRem failed\n");
	}
	if(NULL != pNode->pPayLoad)
	{
//		iros_free(pNode->pPayLoad);
		free(pNode->pPayLoad);
	}
    //	iros_free(pNode);
	free(pNode);

	return;
}

/*******************************************************************
* GwOadMessageListNodeAdd 
*
*******************************************************************/
static void GwOamMessageListNodeAdd(GWTT_OAM_MESSAGE_NODE *pNode)
{
	if(NULL == pNode)
		return;
	if(NULL != GwOamMessageListGetNode(pNode->SendSerNo))
	{
//		IROS_LOG_CRI(IROS_MID_OAM, "GwOamMessageListNodeAdd::GwOamMessageListGetNode failed\n");
	}
//	cyg_semaphore_wait(&OamListSem);
	gw_semaphore_wait(OamListSem, GW_OSAL_WAIT_FOREVER);
	pNode->next = GwOamMessageListHead.next;
	GwOamMessageListHead.next = pNode;
//	cyg_semaphore_post(&OamListSem);
	gw_semaphore_post(OamListSem);
}

/*******************************************************************
* GwOamMessageListNodeRem 
*
*******************************************************************/
static int GwOamMessageListNodeRem(GWTT_OAM_MESSAGE_NODE *pNode)
{
	GWTT_OAM_MESSAGE_NODE *pPrNode=NULL;

	if(NULL == pNode)
		return GWD_RETURN_ERR;
//	cyg_semaphore_wait(&OamListSem);
	gw_semaphore_wait(OamListSem, GW_OSAL_WAIT_FOREVER);
	pPrNode = &GwOamMessageListHead;
	while(NULL!=pPrNode->next)
	{
		if(pNode == pPrNode->next)
		{
			pPrNode->next=pNode->next;
			pNode->next = NULL;
//			cyg_semaphore_post(&OamListSem);
			gw_semaphore_post(OamListSem);
			return GWD_RETURN_OK;
		}
		else
		{
			pPrNode=pPrNode->next;
		}
	}
//	cyg_semaphore_post(&OamListSem);
	gw_semaphore_post(OamListSem);
	return GWD_RETURN_ERR;
}


/*******************************************************************
* GwOamRecevOpcodeCheck 
*
*******************************************************************/
int GwOamRecevOpcodeCheck(unsigned char GwOpcode )
{
	switch(GwOpcode)
	{
		case EQU_DEVICE_INFO_REQ:
		case ALARM_RESP:
		case FILE_READ_WRITE_REQ:
		case FILE_TRANSFER_DATA:
		case FILE_TRANSFER_ACK:
		case SNMP_TRAN_REQ:
		case CLI_REQ_TRANSMIT:
		case IGMP_AUTH_TRAN_REQ:
		case IGMP_AUTH_TRAN_RESP:
		case CLI_PTY_TRANSMIT:
		case IGMP_TVM_REQ:
		case IGMP_TVM_RESP:
			break;
		default:
			return OAM_MESSAGE_RECEV_OPCODE_ERR;
	}
	return OAM_MESSAGE_RECEV_OPCODE_OK;
}

/*******************************************************************
* GwOamSendOpcodeCheck 
*
*******************************************************************/
int GwOamSendOpcodeCheck(unsigned char GwOpcode)
{
	switch(GwOpcode)
	{
		case EQU_DEVICE_INFO_RESP:
		case ALARM_REQ:
		case FILE_READ_WRITE_REQ:
		case FILE_TRANSFER_DATA:
		case FILE_TRANSFER_ACK:
		case SNMP_TRAN_RESP:
		case SNMP_TRAN_TRAP:		
		case CLI_RESP_TRANSMIT:
		case IGMP_AUTH_TRAN_REQ:
		case IGMP_AUTH_TRAN_RESP:
		case CLI_PTY_TRANSMIT:
		case IGMP_TVM_REQ:
		case IGMP_TVM_RESP:
			break;
		default:
			return OAM_MESSAGE_RECEV_OPCODE_ERR;
	}
	return OAM_MESSAGE_RECEV_OPCODE_OK;

}

/*******************************************************************
* CommOnuMsgReveive 
*
*******************************************************************/
int CommOnuMsgReveive(GWTT_OAM_MESSAGE_NODE **ppMessage,unsigned char *MessagData)
{
	GWTT_OAM_MESSAGE_NODE *pMessageNode=NULL;
	GWTT_OAM_HEADER *vender_header=NULL;
	unsigned char *payload=NULL;
	unsigned short PayLoadOffset=0;
	unsigned short PayloadLength=0;
	unsigned int WholePktLen=0;

	if(NULL == ppMessage)
		return OAM_MESSAGE_RECEV_ERR;	
	GWDOAMTRC("  GWD OAM handler - CommOnuMsgReveive\n");
	vender_header = (GWTT_OAM_HEADER*)MessagData;
	if (0 == memcmp(vender_header->oui, GwOUI, sizeof(GwOUI)))
	{
		/* GWTT Extend OAM */
		if(OAM_MESSAGE_RECEV_OPCODE_ERR==GwOamRecevOpcodeCheck(vender_header->opCode))
			return OAM_MESSAGE_RECEV_OPCODE_ERR;	
		GWDOAMTRC("  GWD OAM handler - CommOnuMsgReveive - opCode check OK.\n");
		WholePktLen = ntohs(vender_header->wholePktLen);
		GWDOAMTRC("  GWD OAM handler - CommOnuMsgReveive - PktLen = %d.\n", WholePktLen);
		if(OAM_DATA_LEN < WholePktLen)
			return OAM_MESSAGE_RECEV_TOO_LONG;
		payload = MessagData+sizeof(GWTT_OAM_HEADER);
		PayLoadOffset = ntohs(vender_header->payloadOffset);
		PayloadLength = ntohs(vender_header->payLoadLength);

	 	if (0 == PayLoadOffset)
	 	{
			pMessageNode = GwOamMessageListGetNode(vender_header->senderSerNo);
			if(NULL!=pMessageNode)
				return OAM_MESSAGE_RECEV_ERR;

			if(WholePktLen==PayloadLength)
			{
				pMessageNode = GwOamMessageListNodeNew(WholePktLen);
				if(NULL == pMessageNode)
					return OAM_MESSAGE_RECEV_NO_MEM;
				pMessageNode->GwOpcode=vender_header->opCode;
				pMessageNode->RevPktLen=PayloadLength;
				pMessageNode->WholePktLen=PayloadLength;
				pMessageNode->SendSerNo=vender_header->senderSerNo;
				memcpy(pMessageNode->SessionID,vender_header->sessionId,8);
				memcpy(pMessageNode->pPayLoad,payload,PayloadLength);
				GwOamMessageListNodeAdd(pMessageNode);	
				*ppMessage = pMessageNode;
				GWDOAMTRC("  GWD OAM handler - CommOnuMsgReveive - success 1.\n");
				return OAM_MESSAGE_RECEV_OK;
			}
			else
			{
				pMessageNode = GwOamMessageListNodeNew(WholePktLen);
				if(NULL == pMessageNode)
					return OAM_MESSAGE_RECEV_NO_MEM;
				pMessageNode->GwOpcode=vender_header->opCode;
				pMessageNode->RevPktLen=PayloadLength;
				pMessageNode->WholePktLen=WholePktLen;
				pMessageNode->SendSerNo=vender_header->senderSerNo;
				memcpy(pMessageNode->SessionID,vender_header->sessionId,8);
				memcpy(pMessageNode->pPayLoad,payload,PayloadLength);
				GwOamMessageListNodeAdd(pMessageNode);	
				*ppMessage = NULL;
#if 0
				(pMessageNode->TimerID).opaque = (void *)(pMessageNode->SendSerNo);

				epon_timer_add(&(pMessageNode->TimerID), OamMessageRecevTimeOut, WAIT_TIME_FOR_OAM_MESSAGE);
#else
				pMessageNode->TimerID = gw_timer_add(WAIT_TIME_FOR_OAM_MESSAGE, OamMessageRecevTimeOut, &pMessageNode->SendSerNo);
#endif
				/*pMessageNode->TimerID=VOS_TimerCreate(MODULE_RPU_OAM, (unsigned long)NULL, WAIT_TIME_FOR_OAM_MESSAGE, OamMessageRecevTimeOut, pMessageNode->SendSerNo, VOS_TIMER_NO_LOOP);
				if(0 == pMessageNode->TimerID)
				{
					GwOamMessageListNodeFree(pMessageNode);
					return OAM_MESSAGE_RECEV_TIMER_ERR;
				}*/			
			}
		}
		else
		{
			pMessageNode = GwOamMessageListGetNode(vender_header->senderSerNo);
			if(NULL==pMessageNode)
			{
				return OAM_MESSAGE_RECEV_ERR;
			}
			if(((pMessageNode->RevPktLen+PayloadLength) > pMessageNode->WholePktLen)||
				(pMessageNode->RevPktLen!= PayLoadOffset))			
			{
				return OAM_MESSAGE_RECEV_ERR;
			}
			if((pMessageNode->RevPktLen+PayloadLength) == pMessageNode->WholePktLen)
			{
#if 0
				epon_timer_del(&(pMessageNode->TimerID));
#else
				gw_timer_del(pMessageNode->TimerID);
#endif
				memcpy(pMessageNode->pPayLoad+pMessageNode->RevPktLen,payload,PayloadLength);
				pMessageNode->RevPktLen = pMessageNode->WholePktLen;
				*ppMessage = pMessageNode;
				GWDOAMTRC("  GWD OAM handler - CommOnuMsgReveive - success 2.\n");
				return OAM_MESSAGE_RECEV_OK;
			}
			else
			{
#if 0
				epon_timer_del(&(pMessageNode->TimerID));
				epon_timer_add(&(pMessageNode->TimerID), (pMessageNode->TimerID).tmfunc, WAIT_TIME_FOR_OAM_MESSAGE);
#else
				gw_timer_del(pMessageNode->TimerID);
				gw_timer_add(WAIT_TIME_FOR_OAM_MESSAGE, OamMessageRecevTimeOut, &pMessageNode->SendSerNo);
#endif
				memcpy(pMessageNode->pPayLoad+pMessageNode->RevPktLen,payload,PayloadLength);
				pMessageNode->RevPktLen+=PayloadLength;
				*ppMessage = NULL;
				return OAM_MESSAGE_RECEV_NOT_COMPLETE;
			}		
		}	
	}
	
	return OAM_MESSAGE_RECEV_ERR;
}



static int GwCommOamHeadBuild(GWTT_OAM_HEADER *pHead,  unsigned char GwOpcode,unsigned int SendSerNo,const unsigned short SendDataSize,unsigned char  *pSessionIdfield)
{
	/*
	oam_if_t *oamif = oam_intf_find(oam_sdl_get_llid());

    if(!oamif)
            return GWD_RETURN_ERR;

    oam_build_pdu_hdr(oamif, (oam_pdu_hdr_t *)pHead, OAM_PDU_CODE_ORG_SPEC);
    */
	gw_uint8 buf[128] = "";
	gw_uint32 size = 128, length = 0;

	length = call_gwdonu_if_api(LIB_IF_OAM_HDR_BUILDER, 2, buf, size);

	memcpy((gw_uint8*)pHead, buf, length);

	if((NULL == pHead)||(NULL == pSessionIdfield))
		return GWD_RETURN_ERR;
	if(OAM_MESSAGE_RECEV_OPCODE_ERR == GwOamSendOpcodeCheck(GwOpcode))
		return GWD_RETURN_ERR;

	pHead->oui[0] = 0x00;
	pHead->oui[1] = 0x0F;
	pHead->oui[2] = 0xE9;
	pHead->opCode = GwOpcode;
	pHead->senderSerNo = SendSerNo;
	#ifdef __BIG_ENDIAN__
	pHead->wholePktLen = gwd_htons(SendDataSize);	
	#else
	pHead->wholePktLen = SendDataSize;
	#endif
	if(NULL != pSessionIdfield)
		memcpy(pHead->sessionId,pSessionIdfield,8);
	return GWD_RETURN_OK;
}
int Gwd_OAM_get_length_negotiation(unsigned short *pusOAMFrameLen)
{
#define GWD_OAM_PKT_LENGTH	128

	*pusOAMFrameLen = GWD_OAM_PKT_LENGTH;

	return GWD_RETURN_OK;
}

/*******************************************************************
* CommOnuMsgSend 
*
*******************************************************************/
int CommOnuMsgSend(unsigned char GwOpcode, unsigned int SendSerNo, unsigned char *pSentData,const unsigned short SendDataSize, unsigned char  *pSessionIdfield)
{
	unsigned char OamFrame[2048] = {0};
	GWTT_OAM_HEADER *avender;
	unsigned short DataLenSended=0;
	unsigned short usOAMFrameLen;
	unsigned short usOAMPayloadLenGW;
	int	bSlowProtocol = FALSE;
	int	iSendPacketNumber = 0;
	
	/*
	cs_llid_t llid;
	cs_callback_context_t context;

	if(epon_request_onu_mpcp_llid_get(context, 0, 0, &llid) != GW_OK)
		return GWD_RETURN_ERR;
	 */
    
	GWDOAMTRC("CommOnuMsgSend -- len: %d, start %p\r\n", SendDataSize, pSentData);

	if( GWD_RETURN_OK != Gwd_OAM_get_length_negotiation(&usOAMFrameLen) )
	{
		return GWD_RETURN_ERR;
	}
	else
	{
		usOAMPayloadLenGW = usOAMFrameLen - OAM_OVERHEAD_LEN_STD - OAM_OVERHEAD_LEN_GW;
	}

	if (CLI_RESP_TRANSMIT == GwOpcode)
	{
		bSlowProtocol = TRUE;
	}
	
	gulDebugOamTxCount = 0;
	if(SendDataSize > (OAM_DATA_LEN-sizeof(GWTT_OAM_HEADER)))
		return GWD_RETURN_ERR;
	avender = (GWTT_OAM_HEADER *)OamFrame;
	if(GWD_RETURN_OK != GwCommOamHeadBuild(avender,GwOpcode,SendSerNo,SendDataSize,pSessionIdfield))
		return OAM_MESSAGE_SEND_ERROR;
	
	if(usOAMPayloadLenGW < SendDataSize)
	{
		while((usOAMPayloadLenGW+DataLenSended) < SendDataSize)
		{
			#ifdef 	__BIG_ENDIAN__
			avender->payLoadLength = gwd_htons(usOAMPayloadLenGW);
			avender->payloadOffset = gwd_htons(DataLenSended);
			#else
			avender->payLoadLength = (usOAMPayloadLenGW);
			avender->payloadOffset = (DataLenSended);
			#endif
			memset(OamFrame+sizeof(GWTT_OAM_HEADER), '\0',2048-sizeof(GWTT_OAM_HEADER));
			memcpy(OamFrame+sizeof(GWTT_OAM_HEADER),pSentData+DataLenSended,usOAMPayloadLenGW);
//			oam_send(llid, active_pon_port, (unsigned char *)avender,(int)(usOAMPayloadLenGW + sizeof(GWTT_OAM_HEADER)));
			GWDOAMTRC("CommOnuMsgSend -- call port send if\r\n");
			//gw_printf("oam storm alarm send 1 \n");

//			gw_semaphore_wait(g_pkt_send_sem, GW_OSAL_WAIT_FOREVER);
			call_gwdonu_if_api(LIB_IF_PORTSEND, 3, GW_PON_PORT_ID, (gw_uint8 *)avender,(gw_uint32)(usOAMPayloadLenGW + sizeof(GWTT_OAM_HEADER)));
//			gw_semaphore_post(g_pkt_send_sem);

			gulDebugOamTxCount++;
            OAM_TX_PACKET_DEBUG((avender, pSentData+DataLenSended));
			DataLenSended+=usOAMPayloadLenGW;

			iSendPacketNumber ++;
			if ((0 == (iSendPacketNumber % 10)) && (TRUE == bSlowProtocol))
			{
//				cyg_thread_delay(1); /* 1 tick 10ms */
				gw_thread_delay(10);
			}
		}

#ifdef 	__BIG_ENDIAN__
		avender->payLoadLength =gwd_htons ((SendDataSize-DataLenSended));
		avender->payloadOffset = gwd_htons(DataLenSended);
#else
		avender->payLoadLength = (SendDataSize-DataLenSended);
		avender->payloadOffset = (DataLenSended);
#endif
		memset(OamFrame+sizeof(GWTT_OAM_HEADER), '\0',2048-sizeof(GWTT_OAM_HEADER));
		memcpy(OamFrame+sizeof(GWTT_OAM_HEADER),pSentData+DataLenSended,SendDataSize-DataLenSended);
//		oam_send(llid, active_pon_port, (unsigned char *)avender,(int)(sizeof(GWTT_OAM_HEADER) + SendDataSize - DataLenSended));
		//gw_printf("oam storm alarm send 2 \n");

//		gw_semaphore_wait(g_pkt_send_sem, GW_OSAL_WAIT_FOREVER);
		call_gwdonu_if_api(LIB_IF_PORTSEND, 3, GW_PON_PORT_ID, (gw_uint8*)avender, (gw_uint32)(sizeof(GWTT_OAM_HEADER) + SendDataSize - DataLenSended));
//		gw_semaphore_post(g_pkt_send_sem);

		gulDebugOamTxCount++;
        OAM_TX_PACKET_DEBUG((avender, pSentData+DataLenSended));
		return GWD_RETURN_OK;
	}
	else
	{
#ifdef    __BIG_ENDIAN__
		avender->payLoadLength = gwd_htons(SendDataSize);
		avender->payloadOffset = 0;
#else
		avender->payLoadLength = (SendDataSize);
		avender->payloadOffset = 0;
#endif
		memcpy(OamFrame+sizeof(GWTT_OAM_HEADER),pSentData,SendDataSize);

//		oam_send(llid, active_pon_port, (unsigned char *)avender,(int)(sizeof(GWTT_OAM_HEADER)+SendDataSize));
		//gw_printf("oam storm alarm send 3 \n");

//		gw_semaphore_wait(g_pkt_send_sem, GW_OSAL_WAIT_FOREVER);
		call_gwdonu_if_api(LIB_IF_PORTSEND, 3, GW_PON_PORT_ID, (gw_uint8*)avender, (gw_uint32)(sizeof(GWTT_OAM_HEADER)+SendDataSize));
//		gw_semaphore_post(g_pkt_send_sem);

		gulDebugOamTxCount++;
        OAM_TX_PACKET_DEBUG((avender, pSentData));
		return GWD_RETURN_OK;
	}
}


#if 0
void OamMessageRecevTimeOut(epon_timer_t *timer)
{
	GWTT_OAM_MESSAGE_NODE *pNode;
	unsigned int SerNo = (unsigned int)(timer->opaque);

	pNode = GwOamMessageListGetNode(SerNo);
	if(NULL != pNode)
		GwOamMessageListNodeFree(pNode);
}
#else
void OamMessageRecevTimeOut(void *data)
{
	GWTT_OAM_MESSAGE_NODE *pNode;
	unsigned int SerNo = *(unsigned int*)data;

	pNode = GwOamMessageListGetNode(SerNo);
	if(NULL != pNode)
		GwOamMessageListNodeFree(pNode);

}
#endif

#if 0
static long GwOamIGMPResponseRecv(GWTT_OAM_MESSAGE_NODE *pRequest )
{
	if(NULL == pRequest)            return GWD_RETURN_ERR;
	if(NULL == pRequest->pPayLoad)	return GWD_RETURN_ERR;

	VOS_SysLog(LOG_TYPE_OAM, LOG_DEBUG_OUT, "Receive AUTH_RESP packet SerNo = %d PktLen = %d\r\n", VOS_HTONL(pRequest->SendSerNo),pRequest->RevPktLen);

	return(Igmp_Snoop_AuthRespond(pRequest->pPayLoad, pRequest->WholePktLen));
	return GWD_RETURN_OK;
}

static long GwOamIGMPRequireRecv(GWTT_OAM_MESSAGE_NODE *pRequest )
{
	if(NULL == pRequest)            return GWD_RETURN_ERR;
	if(NULL == pRequest->pPayLoad)	return GWD_RETURN_ERR;

	VOS_SysLog(LOG_TYPE_OAM, LOG_DEBUG_OUT, "Receive AUTH_RESP packet SerNo = %d PktLen = %d\r\n", VOS_HTONL(pRequest->SendSerNo),pRequest->RevPktLen);

	return(Igmp_Snoop_AuthRespond(pRequest->pPayLoad, pRequest->WholePktLen));
	return GWD_RETURN_OK;
}
#endif

/*============================================================*/


long GwOamIGMPRequireSend(char *pPkt, long lLen)
{
	unsigned char pSessId[8];
	
	memset(pSessId, 0, sizeof(pSessId));
	gulOamSendSerNo ++;
	pSessId[6] = gulOamSendSerNo;
	return(CommOnuMsgSend(IGMP_AUTH_TRAN_REQ, gulOamSendSerNo, (unsigned char*)pPkt, lLen, pSessId));
}


long GwOamIGMPRespondSend(char *pPkt, long lLen)
{
	unsigned char pSessId[8];

	memset(pSessId, 0, sizeof(pSessId));
	gulOamSendSerNo ++;
	pSessId[6] = gulOamSendSerNo;
	return(CommOnuMsgSend(IGMP_AUTH_TRAN_RESP, gulOamSendSerNo, (unsigned char*)pPkt, lLen, pSessId));
}

void Gwd_Oam_Handle(unsigned int port, unsigned char *frame, unsigned int len)
{
	GWTT_OAM_MESSAGE_NODE *pMessage = NULL;
	int iRet; 
	/*extern unsigned long cl_sn_service_status ;*/

    if(!frame || !port)
        return;

	if(len < sizeof(GWTT_OAM_HEADER))
		return;

	GWDOAMTRC("Gwd_Oam_Handle - (%d, %x %x %x %x %x %x %d)\n", port, 
		frame[0], frame[1], frame[2], frame[3], frame[4], frame[5], len);
	if(OAM_MESSAGE_RECEV_OK != (iRet = CommOnuMsgReveive(&pMessage, frame)))
	{
//        IROS_LOG_MAJ(IROS_MID_OAM, "GW OAM receiving error!\r\n");
		GWDOAMTRC("  GWD OAM handler - CommOnuMsgReveive - failed(%d).\n", iRet);
		return;
	}
	if(NULL == pMessage)
	{
		return;
	}
	/* Then process the packet */
	gulDebugOamRxCount++;
    OAM_RX_PACKET_DEBUG((pMessage));
	switch(pMessage->GwOpcode)
	{
		case EQU_DEVICE_INFO_REQ:
			GWDOAMTRC("Gwd_Oam_Handle - EQU_DEVICE_INFO_REQ received.\n");
			if(GWD_RETURN_OK != (iRet = GwOamInformationRequest(pMessage)))
			{
//				IROS_LOG_MAJ(IROS_MID_OAM, "Generate OAM(Information Request) response Error!(%d)", iRet);
				GWDOAMTRC("Gwd_Oam_Handle - EQU_DEVICE_INFO_REQ failed.(%d)\n", iRet);
			}
			GwOamMessageListNodeFree(pMessage);
			pMessage = NULL;
			break;

		case ALARM_RESP:
			if(GWD_RETURN_OK != GwOamAlarmResponse(pMessage))
			{
//				IROS_LOG_MAJ(IROS_MID_OAM, "Deal with OLT Alarm response Error!");
			}
			GwOamMessageListNodeFree(pMessage);
			pMessage = NULL;
			break;

		case CLI_REQ_TRANSMIT:
			gwd_oam_async_trans(pMessage);
			break;

		case CLI_PTY_TRANSMIT:
			gwd_oam_pty_trans(pMessage);
			GwOamMessageListNodeFree(pMessage);
			break;

		case SNMP_TRAN_REQ:
			gwd_oamsnmp_handle(pMessage);
			GwOamMessageListNodeFree(pMessage);
			break;
		#if (RPU_MODULE_IGMP_TVM == RPU_YES)
		case IGMP_TVM_REQ:
			GwOamTvmRequestRecv(pMessage);
			GwOamMessageListNodeFree(pMessage);
			pMessage = NULL;
			break;
		#endif
		case FILE_READ_WRITE_REQ:
		case FILE_TRANSFER_DATA:
		case FILE_TRANSFER_ACK:

		case IGMP_AUTH_TRAN_REQ:
	    case IGMP_AUTH_TRAN_RESP:

		default:
			GWDOAMTRC("Gwd_Oam_Handle - unknown opcode(%d) received.\n", pMessage->GwOpcode);
//			IROS_LOG_MAJ(IROS_MID_OAM, "Received an unknown packet(0x%x), drop it!\r\n", pMessage->GwOpcode);
			GwOamMessageListNodeFree(pMessage);
			break;
	}
}

int GwGetOltType(unsigned char *mac, GWD_OLT_TYPE *type)
{
	unsigned char gwPonMac[6] = { 0x00, 0x0c, 0xd5, 0x00, 0x01, 0x00 };
				
	if( 0 == memcmp(mac, gwPonMac, 5)) /* old type, parsed as before*/
	{
		if((mac[5] == 0x0) ||(mac[5] == 0x10) ||
		(mac[5] == 0x20) ||(mac[5] == 0x30))
			*type = GWD_OLT_GFA6100;
		else if(mac[5] >= 0xec)
			*type = GWD_OLT_GFA6700;
		else
			*type = GWD_OLT_NOMATCH;
	}
	else if( 0 == memcmp(mac, gwPonMac, 3)) /*new type, parsed in new formula*/
	{
		if(mac[3] == 0x61)
			*type = GWD_OLT_GFA6100;
		else if(mac[3] == 0x67)
			*type = GWD_OLT_GFA6700;
		else if(mac[3] == 0x69)
			*type = GWD_OLT_GFA6900;	
		else 
			*type = GWD_OLT_NOMATCH;
	}
	else
		*type = GWD_OLT_NONE;

	return GWD_RETURN_OK;
}

int GwGetPonSlotPort(unsigned char *mac, GWD_OLT_TYPE type, unsigned long *slot, unsigned long *port)
{
	unsigned char ponMac;
	unsigned char gwPonMac[6] = { 0x00, 0x0c, 0xd5, 0x00, 0x01, 0x00 };
	ponMac = mac[5];
	if( 0 == memcmp(mac, gwPonMac, 5)) /* old type, parsed as before*/	
	{
		switch(type)
		{
			case GWD_OLT_GFA6100 :
				switch(ponMac)
				{
					case 0 :
						*slot = 2; *port = 1;
						break;
					case 0x10 : 
						*slot = 2; *port = 2;
						break;
					case 0x20 :
						*slot = 3; *port = 1;
						break;
					case 0x30 :
						*slot = 3; *port = 2;
						break;
					default:
						return GWD_RETURN_ERR;
				}
				break;
			case GWD_OLT_GFA6700 :
				*slot = ((ponMac & 0x1c)>>2) + 1;
				*port = ((ponMac & 0x3)) + 1;
				break;
			default :				  /*others*/
				*slot = 0xff;
				*port = 0xff;
				return GWD_RETURN_ERR;
		}
	}
	else if( 0 == memcmp(mac, gwPonMac, 3))	/*new type, parsed in new formula*/
	{
		*slot = mac[4];
		*port = mac[5];
	}
	else 
		return GWD_RETURN_ERR;

	return GWD_RETURN_OK;
}
localtime_tm w_gw_tim;
static int GwOamInformationRequest(GWTT_OAM_MESSAGE_NODE *pRequest )
{
	unsigned char ver[4] = {1, 1, 1, 1};
	unsigned char Response[1024]={'\0'},*ptr, *pReq; 
	unsigned char temp[128];
	int ResLen=0;
	unsigned short device_type;
	int tmpRet;

	if(NULL == pRequest)
		return GWD_RETURN_ERR;
	if(EQU_DEVICE_INFO_REQ != pRequest->GwOpcode)
		return GWD_RETURN_ERR;
	if(NULL == pRequest->pPayLoad)
		return GWD_RETURN_ERR;
	switch(*pRequest->pPayLoad)
	{
		case ONU_INFOR_GET:
		{
			GWDOAMTRC("EQU_DEVICE_INFO_REQ - ONU_INFOR_GET received.\n");
			ptr = Response;
			/* Payload */
			*ptr++  = ONU_INFOR_GET;	/* type : 1 for opCode 1's reply */

			/* Device Type */
			device_type = DEVICE_TYPE_GT811_A;
			SET_SHORT(ptr, device_type);
			ptr += sizeof(short);
			/* OUI */
			*ptr ++ = 0x00;
			*ptr ++ = 0x0f;
			*ptr ++ = 0xe9;
			/* Contents */
			memset(temp, '\0', sizeof(temp));
//			ResLen = sprintf(temp, "V%d.%dB%d",SYS_HARDWARE_MAJOR_VERSION_NO, SYS_HARDWARE_RELEASE_VERSION_NO, SYS_HARDWARE_BRANCH_VERSION_NO);
			ResLen = sprintf(temp, "%s", gw_onu_system_info_total.hw_version);
			*ptr++ = ResLen;
			sprintf(ptr,"%s",temp);
			ptr += ResLen;
			/*Boot Version*/
			if (ver[0] != 0)
			{
				ResLen = sprintf(temp, "%s",irosbootver);
				*ptr++ = ResLen;
				sprintf(ptr,"%s",temp);
				ptr += ResLen;
			}
			else
			{
				*ptr ++= (strlen(GwONU831BootVer));
				sprintf(ptr,"%s",GwONU831BootVer);
				ptr += (strlen(GwONU831BootVer));
			}
			/*Software version*/
			memset(temp, '\0', sizeof(temp));
			/*
			sprintf(temp,"V%dR%02dB%03d",
							SYS_SOFTWARE_MAJOR_VERSION_NO,
							SYS_SOFTWARE_RELEASE_VERSION_NO,
							SYS_SOFTWARE_BRANCH_VERSION_NO);*/
			sprintf(temp, "%s", gw_onu_system_info_total.sw_version);
			*ptr++ = strlen(temp);
			sprintf(ptr, "%s",temp);
			ptr += strlen(temp);

			/*Firmware version*/
			memset(temp, '\0', sizeof(temp));
//			ResLen = sprintf(temp, "V%s.%s.%s.%s",IROS_ONU_APP_VER_MAJOR,IROS_ONU_APP_VER_MINOR,IROS_ONU_APP_VER_REVISION,IROS_ONU_APP_VER_BUILD);
			ResLen = sprintf(temp, "V%s.%s.%s.%s","3","1","1","1");
			*ptr++ = ResLen;
			sprintf(ptr,"%s",temp);
			ptr += ResLen;
			/*Onu name*/
			ResLen = strlen(gw_onu_system_info_total.device_name);
			if (ResLen > 128)
				ResLen = 128;
			*ptr++ = ResLen;
			memcpy(ptr, gw_onu_system_info_total.device_name, ResLen);
			ptr += ResLen;

			/*Description*/
			ResLen = strlen("CTC-ONU-Ready");
			if (ResLen > 128)
				ResLen = 128;
			*ptr++ = ResLen;
			memcpy(ptr, "CTC-ONU-Ready", ResLen);
			ptr += ResLen;

			/*Location*/
			ResLen = strlen("Beijing China");
			if (ResLen > 128)
				ResLen = 128;
			*ptr ++= ResLen;
			memcpy(ptr,"Beijing China", ResLen);
			ptr += ResLen;

			/*Vendor*/
			ResLen = strlen("GW Delight");
			if (ResLen > 128)
				ResLen = 128;
			*ptr ++= ResLen;
			memcpy(ptr, "GW Delight", ResLen);
			ptr += ResLen;

			/*Serial Number*/
			ResLen = strlen(gw_onu_system_info_total.serial_no);
			*ptr ++ = ResLen;
			memcpy(ptr, gw_onu_system_info_total.serial_no, ResLen);
			ptr += ResLen;
			/*Manufacture Date*/
			ResLen = strlen(gw_onu_system_info_total.hw_manufature_date);
			*ptr ++ = ResLen;
			memcpy(ptr, gw_onu_system_info_total.hw_manufature_date, ResLen);
			ptr += ResLen;

			/*auto config set*/
			*ptr ++ = 0;

			/*maximum slot config set*/
			*ptr ++ = 0;

			/*extension capality*/
			*ptr ++ = 0xfe;
			*ptr ++ = 3;
			*ptr ++= 0x80; /*added ctc statistic function surpport*/
			ResLen = ((unsigned long)ptr-(unsigned long)Response);			

			gulGwOamConnect = 1;
			break;
		}
		case ONU_INFOR_SET:
		{
			unsigned char nameLen, descrLen, locationLen;
			
			tmpRet = ONU_INFOR_SET<<8;
			pReq = pRequest->pPayLoad+1;

			ptr = Response;
			/* Payload */
			*ptr++  = ONU_INFOR_SET;	

			/* Name */
			nameLen = *pReq;
			pReq ++;
			if (nameLen)
			{
				unsigned char tmpLen;

				if (nameLen > sizeof(gw_onu_system_info_total.device_name))
					tmpLen = sizeof(gw_onu_system_info_total.device_name);
				else
					tmpLen = nameLen;
				
				GW_Onu_Sysinfo_Get();
				memset(gw_onu_system_info_total.device_name, 0, sizeof(gw_onu_system_info_total.device_name));
				memcpy(gw_onu_system_info_total.device_name, pReq, tmpLen);
				GW_Onu_Sysinfo_Save();
				/* Success */
				*ptr ++ = 1;
				pReq += nameLen;
			}

			/* Description */
			descrLen = *pReq;
			pReq ++;
			if (descrLen)
			{
#if 0
				char * errmsg = "";
				int ret;
				
				if (descrLen > 255)
					return (tmpRet|S_BAD_PARAM);
				
				VOS_MemZero(szTmp, 256);
				memcpy(szTmp, pReq, descrLen);

				ret = mn_set_sysdescr( szTmp , &errmsg, 1 );

				if (ret)
				{
					/* Success */
					*ptr ++ = 1;
				}
				else
				{
					/* Failed */
					*ptr ++ = 2;
				}
#else
				/* Success */
				*ptr ++ = 1;
#endif				
				pReq += descrLen;
			}

			/* Location */
			locationLen = *pReq;
			pReq ++;
			if (locationLen)
			{
#if 0
				char * errmsg = "";
				int ret;

				if (locationLen > 255)
					return (tmpRet|S_BAD_CONFIGURATION);
				
				VOS_MemZero(szTmp, 256);
				memcpy(szTmp, pReq, locationLen);

				ret = mn_set_syslocation( szTmp, &errmsg, 1 );

				if (ret)
				{
					/* Success */
					*ptr ++ = 1;
				}
				else
				{
					/* Failed */
					*ptr ++ = 2;
				}

#else
				/* Success */
				*ptr ++ = 1;
#endif				
				pReq += locationLen;
			}
			ResLen = ((unsigned int)ptr-(unsigned int)Response);
			break;
		}
		case ONU_REALTIME_SYNC:
		{
#if 1
			unsigned short usValue;
			unsigned char ucValue;
			int	i = 0;
			extern char   g_cSetTime[20];
			
			tmpRet = ONU_REALTIME_SYNC<<8;
			pReq = pRequest->pPayLoad+1;

			ptr = g_cSetTime;
			memset(&w_gw_tim,0,sizeof(localtime_tm));
			/* Year */
			/*usValue = *((unsigned short *)pReq);*//* Will cause exception: maybe because pReq not odd address */
			usValue = pReq[i];
			i++;
			usValue = usValue << 8;
			usValue += pReq[i];
			if (usValue < 1980 ||usValue > 2079)   //according to OLT system time range, added by dushb 2009-11-12
				return GWD_RETURN_ERR;
			ResLen = sprintf(ptr,"%4d/",usValue);
			w_gw_tim.tm_year = usValue;
			//diag_printf("year len:%d,year:%d\n",ResLen,w_gw_tim.tm_year);
			ptr += ResLen;
			i++;

			/* Month */
			ucValue = pReq[i];
			if (ucValue > 12) 
				return GWD_RETURN_ERR;
			ResLen = sprintf(ptr,"%02d/",ucValue);
			w_gw_tim.tm_mon = ucValue;
			//diag_printf("mon len:%d mon:%d\n",ResLen,w_gw_tim.tm_mon);
			ptr += ResLen;
			i++;

			/* Day */
			ucValue = pReq[i];
			if (ucValue > 31) 
				return GWD_RETURN_ERR;
			ResLen = sprintf(ptr,"%02d:",ucValue);
			w_gw_tim.tm_mday = ucValue;
			//diag_printf("day len:%d day:%d\n",ResLen,w_gw_tim.tm_mday);
			ptr += ResLen;
			i++;

			/* Hour */
			ucValue = pReq[i];
			if (ucValue > 24) 
				return GWD_RETURN_ERR;
			ResLen = sprintf(ptr,"%02d:",ucValue);
			w_gw_tim.tm_hour = ucValue;
			//diag_printf("hour len:%d hour:%d\n",ResLen,w_gw_tim.tm_hour);
			ptr += ResLen;
			i++;
			
			/* Minute */
			ucValue = pReq[i];
			if (ucValue > 59) 
				return GWD_RETURN_ERR;
			ResLen = sprintf(ptr,"%02d:",ucValue);
			w_gw_tim.tm_min = ucValue;
			//diag_printf("min len:%d min:%d\n",ResLen,w_gw_tim.tm_min);
			ptr += ResLen;
			i++;
			
			/* Second */
			ucValue = pReq[i];
			if (ucValue > 59) 
				return GWD_RETURN_ERR;
			ResLen = sprintf(ptr,"%02d",ucValue);
			w_gw_tim.tm_sec = ucValue;
			//diag_printf("sec len:%d sec:%d\n",ResLen,w_gw_tim.tm_sec);
			ptr += ResLen;
			i++;

			ResLen = i+1;
			//memcpy(Response,pRequest->pPayLoad,ResLen);
#ifdef __DEBUG__
			cl_do_set_time_nouser(NULL);

#if (RPU_YES == RPU_MODULE_TIMING_PKT)

            if((0 == TimingPkt_TaskID)&&(TIMPKT_SEND_ENABLE == gulTimingPacket))/*只锟斤拷使锟斤拷时锟斤拷锟斤拷一锟斤拷*/
            {
                TimingPkt_TaskID = VOS_TaskCreate("tEthTx", 220, (VOS_TASK_ENTRY) txEthTask, NULL);/*锟斤拷锟斤拷锟斤拷锟饺硷拷= port monitor*/
                VOS_ASSERT(TimingPkt_TaskID != 0);
            }
#endif
#endif		
#endif
			break;
		}

    case ACCESS_IDENTIFIER:
    {
#if 0
        int iret;
        extern PASONU_flow_desc_t rxEthTaskfdHigh;

        pReq = pRequest->pPayLoad+1;
        ptr = Response + 1;
        ResLen = pRequest->RevPktLen;
        
        memcpy(Response,pRequest->pPayLoad,pRequest->RevPktLen);
      
        pReq++;

        if (*pReq > RELAY_TYPE_DHCP)
        {
            *(ptr ++) = 2;
            break;
        }

#if (RPU_MODULE_PPPOE_RELAY == RPU_YES)
        if (RELAY_TYPE_PPPOE == *pReq)
        {
            PPPOE_RELAY_PACKET_DEBUG(("\r\n received pppoe relay-OAM pkt!\r\n"));

            PPPOE_RELAY_PACKET_DEBUG(("\r\n relay-mode=%d\r\n",*pReq));

            pReq++;
            
            if (*pReq > PPPOE_DSL_FORUM_MODE)
            {
                *(ptr ++) = 2;/*失锟斤拷*/
                break;
            }
            
            if(PPPOE_RELAY_DISABLE == *pReq)/*锟斤拷锟轿�锟斤拷锟斤拷示锟斤拷止状态*/
            {
                if (PPPOE_RELAY_DISABLE == g_PPPOE_relay)
                {
                    /*do nothing*/
                    PPPOE_RELAY_PACKET_DEBUG(("\r\n pppoe relay is already disabled! \r\n"));
                }
                else
                {
                    iret = PASONU_CLASSIFIER_remove_filter(PASONU_UPSTREAM, FRAME_ETHERTYPE, 0x8863);
                    if ((S_OK != iret)&&(S_NOT_FOUND != iret))
                    {
                        ASSERT(0);
                        PPPOE_RELAY_PACKET_DEBUG(("\r\n pppoe relay is disabled failed! \r\n"));
                        *(ptr ++) = 2;
                        break;
                    }
                    
                    if (S_OK != PasOnuClassL2RuleAdd(PASONU_UPSTREAM, FRAME_ETHERTYPE, 0x8863, 0, 2))
                    {
                        PPPOE_RELAY_PACKET_DEBUG(("\r\n pppoe relay is disabled failed! \r\n"));
                        *(ptr ++) = 2;
                        break;
                    }

                    if (S_OK != PasOnuClassL2RuleAdd(PASONU_UPSTREAM, FRAME_ETHERTYPE, 0x8864, 0, 3))
                    {
                        PPPOE_RELAY_PACKET_DEBUG(("\r\n pppoe relay is disabled failed! \r\n"));
                        *(ptr ++) = 2;
                        break;
                    }

#if ((!FOR_812_SERIES)&&(!FOR_BCM_ONU_PON_VOICE))/*GT812锟斤拷锟斤拷锟斤拷锟斤拷*/
                    if (S_OK != PASONU_PQUEUE_set_ingress_limit(PQ_RX_CPU_UNI, 1, 0))
                    {
                        PPPOE_RELAY_PACKET_DEBUG(("\r\n pppoe relay is disabled failed! \r\n"));
                        *(ptr ++) = 2;
                        break;
                    }
#endif
                    g_PPPOE_relay = PPPOE_RELAY_DISABLE;
                    PPPOE_RELAY_PACKET_DEBUG(("\r\n pppoe relay is disabled successfully! \r\n"));
                }
                *(ptr ++) = 1;
                
                break;
            }
            else
            {
                if (PPPOE_RELAY_ENABLE == g_PPPOE_relay)
                {
                    /*do nothing*/
                    PPPOE_RELAY_PACKET_DEBUG(("\r\n pppoe relay is already enabled! \r\n"));
                }
                else
                {
                    if (S_OK != PasOnuClassL2RuleAdd(PASONU_UPSTREAM, FRAME_ETHERTYPE, 0x8863, 0, 2))
                    {
                        PPPOE_RELAY_PACKET_DEBUG(("\r\n pppoe relay is enabled failed! \r\n"));
                        *(ptr ++) = 2;
                        break;
                    }
                            
                    if (S_OK != PasOnuClassL2RuleAdd(PASONU_UPSTREAM, FRAME_ETHERTYPE, 0x8864, 0, 3))
                    {
                        PPPOE_RELAY_PACKET_DEBUG(("\r\n pppoe relay is enabled failed! \r\n"));
                        *(ptr ++) = 2;
                        break;
                    }


                    iret = PASONU_CLASSIFIER_remove_filter(PASONU_UPSTREAM, FRAME_ETHERTYPE, 0x8863);
                    if ((S_OK != iret)&&(S_NOT_FOUND != iret))
                    {
                        ASSERT(0);
                        PPPOE_RELAY_PACKET_DEBUG(("\r\n pppoe relay is enabled failed! \r\n"));
                        *(ptr ++) = 2;
                        break;
                    }
                    
                    iret = PASONU_CLASSIFIER_add_filter(PASONU_UPSTREAM, FRAME_ETHERTYPE, 0x8863, PASONU_PASS_CPU, rxEthTaskfdHigh);
                    if ((S_OK != iret)&&(S_ALREADY_EXISTS != iret))
                    {
                        PPPOE_RELAY_PACKET_DEBUG(("\r\n pppoe relay is enabled failed! \r\n"));
                        *(ptr ++) = 2;
                        break;
                    }

#if ((!FOR_812_SERIES)&&(!FOR_BCM_ONU_PON_VOICE))/*GT812锟斤拷GT863锟斤拷GT866锟斤拷锟斤拷锟斤拷锟斤拷*/
                    if (S_OK != PASONU_PQUEUE_set_ingress_limit(PQ_RX_CPU_UNI, 1, 63))
                    {
                        PPPOE_RELAY_PACKET_DEBUG(("\r\n pppoe relay is enabled failed! \r\n"));
                        *(ptr ++) = 2;
                        break;
                    }
#endif
                    
                    g_PPPOE_relay = PPPOE_RELAY_ENABLE;
                    PPPOE_RELAY_PACKET_DEBUG(("\r\n pppoe relay is enabled successfully! \r\n"));
                }
            }
        
            if (PPPOE_GWD_PRIVITE_MODE == *pReq)
            {
                PPPOE_relay_mode = PPPOE_GWD_PRIVITE_MODE;
            }
            else
            {
                PPPOE_relay_mode = PPPOE_DSL_FORUM_MODE;
            }
      
            pReq ++;
      
            /*锟斤拷取锟街凤拷*/

            PPPOE_RELAY_PACKET_DEBUG(("\r\n olt-relay-string = %s\r\n",pReq));
            
            if(pRequest->RevPktLen - 4 > ((PPPOE_GWD_PRIVITE_MODE == PPPOE_relay_mode)?40:50))
            {
                PPPOE_RELAY_PACKET_DEBUG(("\r\n circuit id value is too long! \r\n"));
                PASONU_CLASSIFIER_remove_filter(PASONU_UPSTREAM, FRAME_ETHERTYPE, 0x8863);
                PASONU_PQUEUE_set_ingress_limit(PQ_RX_CPU_UNI, 1, 0);
                PasOnuClassL2RuleAdd(PASONU_UPSTREAM, FRAME_ETHERTYPE, 0x8863, 0, 1);
                g_PPPOE_relay = PPPOE_RELAY_DISABLE;/*锟街革拷锟斤拷disable锟斤拷状态锟斤拷mode锟酵诧拷锟斤拷锟斤拷*/
                *(ptr ++) = 2;
                break;
            }
        
            pppoe_circuitid_value_lenth = pRequest->RevPktLen - 4;/*4锟斤拷锟街节分憋拷锟斤拷msg_type,result,relay_type,relay_mode*/
            memcpy(pppoe_circuitid_value_head, pReq, pppoe_circuitid_value_lenth);
            *(pppoe_circuitid_value_head + pppoe_circuitid_value_lenth) = '\0';/*锟斤拷锟节达拷印string*/

        }
#endif

        if(RELAY_TYPE_DHCP == *pReq)/*锟斤拷锟斤拷pppoe_relay锟斤拷锟斤拷锟斤拷*/
        {
            DHCP_RELAY_PACKET_DEBUG(("\r\n received dhcp relay-OAM pkt!\r\n"));
            
            DHCP_RELAY_PACKET_DEBUG(("\r\n relay-mode=%d\r\n",*pReq));

            pReq++;
            
            if (*pReq > DHCP_OPTION82_RELAY_MODE_STD)
            {
                *(ptr ++) = 2;/*失锟斤拷*/
                break;
            }
            
            if(0 == *pReq)/*锟斤拷锟轿�锟斤拷锟斤拷示锟斤拷止状态*/
            {
                if (0 == g_DHCP_OPTION82_Relay)
                {
                    /*do nothing*/
                    DHCP_RELAY_PACKET_DEBUG(("\r\n dhcp relay is already disabled! \r\n"));
                }
                else
                {
    				PasOnuFilterL3L4RuleDelete(PASONU_UPSTREAM, PASONU_TRAFFIC_UDP_PORT,
    					PASONU_TRAFFIC_SOURCE, 0, DHCP_SERVER_PORT);					
#if (!FOR_812_SERIES)/*GT812锟斤拷锟斤拷锟斤拷锟斤拷*/
                    PASONU_PQUEUE_set_ingress_limit(PQ_RX_CPU_UNI, 0, 0);
#endif
                    g_DHCP_OPTION82_Relay = 0;
                    DHCP_RELAY_PACKET_DEBUG(("\r\n dhcp relay is disabled successfully! \r\n"));
                }
                *(ptr ++) = 1;
                
                break;
            }
            else
            {
                if (1 == g_DHCP_OPTION82_Relay)
                {
                    /*do nothing*/
                    DHCP_RELAY_PACKET_DEBUG(("\r\n dhcp relay is already enabled! \r\n"));
                }
                else
                {
        			PasOnuFilterL3L4RuleAdd(PASONU_UPSTREAM, PASONU_TRAFFIC_UDP_PORT,
        				PASONU_TRAFFIC_SOURCE, 0, DHCP_SERVER_PORT, PASONU_PASS_CPU);
#if (!FOR_812_SERIES)/*GT812锟斤拷锟斤拷锟斤拷锟斤拷*/
                    PASONU_PQUEUE_set_ingress_limit(PQ_RX_CPU_UNI, 0, 13);
#endif
                    
                    g_DHCP_OPTION82_Relay = 1;
                    DHCP_RELAY_PACKET_DEBUG(("\r\n dhcp relay is enabled successfully! \r\n"));
                }
            }
        
            if (DHCP_OPTION82_RELAY_MODE_CTC == *pReq)
            {
                g_DHCP_OPTION82_Relay_Mode = DHCP_OPTION82_RELAY_MODE_CTC;
            }
            else
            {
                g_DHCP_OPTION82_Relay_Mode = DHCP_OPTION82_RELAY_MODE_STD;
            }
      
            pReq ++;
      
            /*锟斤拷取锟街凤拷*/

            DHCP_RELAY_PACKET_DEBUG(("\r\n olt-relay-string=%s\r\n",pReq));
            
            if(pRequest->RevPktLen - 4 > 40)
            {
                DHCP_RELAY_PACKET_DEBUG(("\r\n circuit id value is too long! \r\n"));
                *(ptr ++) = 2;
                break;
            }
        
            dhcp_circuitid_value_lenth = pRequest->RevPktLen - 4;/*4锟斤拷锟街节分憋拷锟斤拷msg_type,result,relay_type,relay_mode*/
            memcpy(dhcp_circuitid_value_head, pReq, dhcp_circuitid_value_lenth);
            *(dhcp_circuitid_value_head + dhcp_circuitid_value_lenth) = '\0';/*锟斤拷锟节达拷印string*/

        }
      
        *(ptr ++) = 1;
#endif
        break;
    }      

		case ONU_LPB_DETECT:
		{
			int nv = 0;

			extern unsigned long gulLoopDetectMode;
			extern long EthLoopbackDetectControl(unsigned long oamEnable, unsigned long localEnable);

			GWDOAMTRC("EQU_DEVICE_INFO_REQ - ONU_LPB_DETECT received.\n");

			memset(&tframe, 0, sizeof(OAM_ONU_LPB_DETECT_FRAME));
			memcpy(&tframe, pRequest->pPayLoad, pRequest->WholePktLen);
			memcpy(&(oam_onu_lpb_detect_frame.smac), &(tframe.smac), 6);
			GWDOAMTRC("  ONU_LPB_DETECT - smac : %x-%x-%x-%x-%x-%x\n", 
				tframe.smac[0], tframe.smac[1], tframe.smac[2], 
				tframe.smac[3], tframe.smac[4], tframe.smac[5]);

			nv = ntohs(tframe.vid);
			oam_onu_lpb_detect_frame.vid = nv;
			GWDOAMTRC("  ONU_LPB_DETECT - vid : %d\n", nv); 
			
			nv = ntohs(tframe.interval);
			oam_onu_lpb_detect_frame.interval = (nv)?nv:10;
			GWDOAMTRC("  ONU_LPB_DETECT - interval : %d\n", nv); 

			nv = ntohs(tframe.policy);
			oam_onu_lpb_detect_frame.policy = nv;
			GWDOAMTRC("  ONU_LPB_DETECT - policy : %d\n", nv); 

			if(LPB_OLD_VER_LEN == pRequest->WholePktLen)
			{
				oam_onu_lpb_detect_frame.waitforwakeup = 3;
				oam_onu_lpb_detect_frame.maxwakeup = 3;
			}
			else
			{
				nv = ntohs(tframe.waitforwakeup);
				oam_onu_lpb_detect_frame.waitforwakeup = nv;

				nv = ntohs(tframe.maxwakeup);
				oam_onu_lpb_detect_frame.maxwakeup = nv;
			}
			GWDOAMTRC("  ONU_LPB_DETECT - waitforwakeup : %d\n", oam_onu_lpb_detect_frame.waitforwakeup); 
			GWDOAMTRC("  ONU_LPB_DETECT - maxwakeup : %d\n", oam_onu_lpb_detect_frame.maxwakeup); 
			GWDOAMTRC("  ONU_LPB_DETECT - enable : %d\n", oam_onu_lpb_detect_frame.enable); 


			EthLoopbackDetectControl(tframe.enable, gulLoopDetectMode);

		}
		return GWD_RETURN_OK;	

#if 0
		case IP_RESOURCE_ALLOC:
			return spawnOnuUpdateTask(pRequest->pPayLoad+2, pRequest->WholePktLen-2, pRequest->SessionID);
			break;
		case IP_RESOURCE_FREE:
			g_ftpDataPathCtrl = 0;
			return sendIpSourcManAck(IP_RESOURCE_FREE, 0, pRequest->SessionID);
			break;
#endif

#if (RPU_MODULE_USER_MAC_RELAY == RPU_YES)
		case ONU_LOCATE_USER:
			{
				unsigned char swmac[USR_MAC_LEN] = "", subsw = 0;
				userMacRequest_t info[USR_MAC_REQUEST_MAX];
			    userMacResponse_t responseInfo[USR_MAC_RESPONS_MAC_MAX] = {0};
				unsigned char nullmac[USR_MAC_LEN]={0,0,0,0,0,0};
				localMacsave_t macbuf[USR_MAC_MAX_T]={0};
				gw_uint8 lastmac[GW_MACADDR_LEN]={0,0,0,0,0,0};
				
				int onuslot =0, onuport=0, swport = 0;
				int	requestNum = 0,responseNum = 0;				
				int requestlen=0;

				int ifhavemac = 0;
				int macnumberget = 0;
				
			    userMacResponse_pdu_t *responsePdu = NULL;
                unsigned char *tempP = Response;
		
				userMacRequest_pdu_t *requestPdu = (userMacRequest_pdu_t *)pRequest->pPayLoad;
				requestlen = sizeof(userMacRequest_pdu_t);
                #ifdef __USE_MAC__
                int i=0;
                gw_printf("into ONU_LOCATE_USER now\n");
                #endif
                responsePdu = malloc(sizeof(userMacResponse_pdu_t));
                if (NULL == responsePdu)
                {
                   gw_printf("usermac malloc error\n");
                   return GWD_RETURN_ERR;
                }
				
                ResLen = sizeof(userMacResponse_pdu_t);
				
				/*********************************************************************
				��ȡusermac oam request ���е�mac
				*********************************************************************/
				if(requestPdu->macNum > USR_MAC_REQUEST_MAX)
				{
                    gw_printf("olt send mac number > mac_max(64)\n");
                    return GWD_RETURN_ERR; 
                }
				for (requestNum = 0; requestNum < requestPdu->macNum; requestNum++)
				{
					memcpy(info[requestNum].swmac,(pRequest->pPayLoad+requestlen),USR_MAC_LEN);
					requestlen += USR_MAC_LEN;
				}
                #ifdef __USE_MAC__
                gw_printf("===============================\n");
                for(i = 0; i < requestPdu->macNum; i++)
                {
                    
                    gw_printf("%02x %02x %02x %02x %02x %02x \n",info[i].swmac[0],info[i].swmac[1],info[i].swmac[2],info[i].swmac[3],info[i].swmac[4],info[i].swmac[5]);
                   
                }
                 gw_printf("===============================\n");
				gw_printf("requestPdu->macNum:%d\n",requestPdu->macNum);
                #endif
                
				while(!ifhavemac)
				{					
					if(user_mac_onu_fdb_get(macbuf,lastmac,&macnumberget,&ifhavemac))
					{
                        free(responsePdu);
					    responsePdu = NULL;
						return GWD_RETURN_ERR;
					}
                    
                    #ifdef __USE_MAC__
					gw_printf("macnumberget:%d\n",macnumberget);
                    #endif
                    
                    if(0 == macnumberget)
                    {
                        break;
                    }
                    
	                for (requestNum = 0; requestNum < requestPdu->macNum; requestNum++)
	                {
	    				/*generating response pdu only for found the mac because of OLT broadcast oam request*/
						if(!memcmp(info[requestNum].swmac,nullmac,USR_MAC_LEN))
						{
							continue;
						}
						
	    				if(GW_OK == locateUserMac( info[requestNum].swmac,macbuf,macnumberget,&onuslot, &onuport, &subsw, swmac, &swport))
	    				{
	    					memcpy(responseInfo[responseNum].usermac, info[requestNum].swmac, USR_MAC_LEN);/*USR MAC*/
	    					responseInfo[responseNum].reserved = 0;
	    					responseInfo[responseNum].onuslot = onuslot;/*SLOT ����λΪ0*/
	    					responseInfo[responseNum].onuport = onuport;
	    					responseInfo[responseNum].subsw = subsw;
	    					memcpy(responseInfo[responseNum].swmac, swmac, USR_MAC_LEN);
	    					responseInfo[responseNum].swport = swport;
							memset(info[requestNum].swmac,0,USR_MAC_LEN);/*�ҵ�֮�����mac���´ξͲ���Ҫ�ڲ���*/
	                        responseNum++;
	                        ResLen += sizeof(userMacResponse_t);
							/*****************************************************************************
							���Ҫ���mac�����Ѿ�����fdb���Ҷ��ҵ��˾Ͳ���Ҫ������
							����ҵ�mac�����32��
							*******************************************************************************/
							if((responseNum == requestPdu->macNum) || (responseNum >= USR_MAC_RESPONS_MAC_MAX))
							{
								ifhavemac=1;
								macnumberget = 0;
                                #ifdef __USE_MAC__
                                gw_printf("while responseNum:%d\n",responseNum);
                                #endif
								goto END;
							}
	    				}
	                }
					ifhavemac=0;
						
				}
END:
    #ifdef __USE_MAC__
				gw_printf("true responseNum:%d\n",responseNum);
    #endif
                ResLen += 1;/*reserved bit.*/
                if (0 != responseNum)
                {
                	responsePdu->type = ONU_LOCATE_USER;/*cheak type*/
                    responsePdu->result = 1;
					responsePdu->mode	= USR_MAC_ADDRES_CHEAK;
                    responsePdu->macNum = responseNum;
                    
                    memcpy(tempP, responsePdu, sizeof(userMacResponse_pdu_t));
                    tempP += sizeof(userMacResponse_pdu_t);

					memcpy(tempP, responseInfo, (responseNum*sizeof(userMacResponse_t)));
					free(responsePdu);
					responsePdu = NULL;
                }
                else
                {
                    ResLen = 0;
					free(responsePdu);
					responsePdu = NULL;
					return GWD_RETURN_ERR;
                }
				
			}
			break;
#endif
		default:
		{
//			IROS_LOG_MAJ(IROS_MID_OAM, "OAM INFO request (%d) no suportted!", *pRequest->pPayLoad);
			GWDOAMTRC("EQU_DEVICE_INFO_REQ - unknown received.(%d)\n", *pRequest->pPayLoad);
			return GWD_RETURN_ERR;
		}
	}
	return (CommOnuMsgSend(EQU_DEVICE_INFO_RESP, pRequest->SendSerNo, Response, ResLen, pRequest->SessionID));
	//return GWD_RETURN_ERR;
}

static int GwOamAlarmResponse(GWTT_OAM_MESSAGE_NODE *pRequest )
{
	if(NULL == pRequest)
		return GWD_RETURN_ERR;
	if(NULL == pRequest->pPayLoad)
		return GWD_RETURN_ERR;
	switch((*pRequest->pPayLoad))
	{
		case ONU_TEMPRATURE_ALARM:
		{	
			break;
		}
		case ONU_ETH_PORT_STATE:
		{	
			break;
		}
		case ONU_ETH_PORT_ABILITY:
		{	
			break;
		}
		case ONU_ETH_WORK_STOP:
		{	
			break;
		}
		case ONU_STP_EVENT:
		{	
			break;
		}
		case ONU_DEVICE_INFO_CHANGE:
		{	
			break;
		}
		case ONU_SWITCH_STATUS_CHANGE_ALARM:
		{	
	        break;
		}
		case ONU_FILE_DOWNLOAD:
		{
#if 0
			if(UPGRADE_RESULT_OK == *(pRequest->pPayLoad + 3))
			{
				switch(*(pRequest->pPayLoad + 2))
				{
					case SOFTWARE_UPGRADE:
					{
						VOS_SysLog(LOG_TYPE_OAM, LOG_NOTICE, "Software Upgrade successed!");
						queueFileopCmd(pRequest->SessionID, FCMD_RECVALAMRESP);
						break;
					}
					case FIRMWARE_UPGRADE:
					{
						VOS_SysLog(LOG_TYPE_OAM, LOG_NOTICE, "Firmware Upgrade successed!");
						break;
					}
					case BOOT_UPGRADE:
					{
						VOS_SysLog(LOG_TYPE_OAM, LOG_NOTICE, "Boot file Upgrade successed!");
						break;
					}
					case CONFIG_UPGRADE:
					{
						VOS_SysLog(LOG_TYPE_OAM, LOG_NOTICE, "Config file Upgrade successed!");
						break;
					}
					case VOICE_UPGRADE:
					{
						VOS_SysLog(LOG_TYPE_OAM, LOG_NOTICE, "Voip Software Upgrade successed!");
						queueFileopCmd(pRequest->SessionID, FCMD_RECVALAMRESP);
						break;
					}
#if( RPU_HAVE_FPGA == RPU_YES )				
					case FPGA_UPGRADE:
					{
						VOS_SysLog(LOG_TYPE_OAM, LOG_NOTICE, "FPGA Upgrade successed!");
						queueFileopCmd(pRequest->SessionID, FCMD_RECVALAMRESP);
						break;
					}
#endif
					default:
						break;
				}				
				/* DEV_ResetMySelf(); */
			}
			else
			{
				switch(*(pRequest->pPayLoad + 2))
				{
					case SOFTWARE_UPGRADE:
					{
						VOS_SysLog(LOG_TYPE_OAM, LOG_ERR, "Software Upgrade failed!");
						queueFileopCmd(pRequest->SessionID, FCMD_RECVALAMRESP);
						break;
					}
					case FIRMWARE_UPGRADE:
					{
						VOS_SysLog(LOG_TYPE_OAM, LOG_ERR, "Firmware Upgrade failed!");
						break;
					}
					case BOOT_UPGRADE:
					{
						VOS_SysLog(LOG_TYPE_OAM, LOG_ERR, "Boot file Upgrade failed!");
						break;
					}
					case CONFIG_UPGRADE:
					{
						VOS_SysLog(LOG_TYPE_OAM, LOG_ERR, "Config file Upgrade failed!");
						break;
					}
					case VOICE_UPGRADE:
					{
						VOS_SysLog(LOG_TYPE_OAM, LOG_NOTICE, "Voip Software Upgrade successed!");
						queueFileopCmd(pRequest->SessionID, FCMD_RECVALAMRESP);
						break;
					}
#if( RPU_HAVE_FPGA == RPU_YES )				
					case FPGA_UPGRADE:
					{
						VOS_SysLog(LOG_TYPE_OAM, LOG_ERR, "FPGA Upgrade failed!");
						queueFileopCmd(pRequest->SessionID, FCMD_RECVALAMRESP);
						break;
					}
#endif
					default:
					{
						VOS_SysLog(LOG_TYPE_OAM, LOG_ERR, "OAM Alarn response no suportted!");
						break;
					}
				}
			}
#endif
			break;
		}
		default:
			return GWD_RETURN_ERR;
	}
	return GWD_RETURN_OK;
}


/* Functions for debug */
int Debug_Print_Rx_OAM(GWTT_OAM_MESSAGE_NODE *pMessage)
{
    int i;
    OAMDBGERR("\r\nRx OAM packet as following:\r\n");
    OAMDBGERR("    GwOpCode:    %02X", pMessage->GwOpcode);
    switch (pMessage->GwOpcode)
    {
        case EQU_DEVICE_INFO_REQ:
            OAMDBGERR("(EQU_DEVICE_INFO_REQ)\r\n");
            break;
        case EQU_DEVICE_INFO_RESP:
            OAMDBGERR("(EQU_DEVICE_INFO_RESP)\r\n");
            break;
        case ALARM_REQ:
            OAMDBGERR("(ALARM_REQ)\r\n");
            break;
        case ALARM_RESP:
            OAMDBGERR("(ALARM_RESP)\r\n");
            break;
        case FILE_READ_WRITE_REQ:
            OAMDBGERR("(FILE_READ_WRITE_REQ)\r\n");
            break;
        case FILE_RESERVER:
            OAMDBGERR("(FILE_RESERVER)\r\n");
            break;
        case FILE_TRANSFER_DATA:
            OAMDBGERR("(FILE_TRANSFER_DATA)\r\n");
            break;
        case FILE_TRANSFER_ACK:
            OAMDBGERR("(FILE_TRANSFER_ACK)\r\n");
            break;
        case CHURNING:
            OAMDBGERR("(CHURNING)\r\n");
            break;
        case DBA:
            OAMDBGERR("(DBA)\r\n");
            break;
        case SNMP_TRAN_REQ:
            OAMDBGERR("(SNMP_TRAN_REQ)\r\n");
            break;
        case SNMP_TRAN_RESP:
            OAMDBGERR("(SNMP_TRAN_RESP)\r\n");
            break;
        case SNMP_TRAN_TRAP:
            OAMDBGERR("(SNMP_TRAN_TRAP)\r\n");
            break;
        case CLI_REQ_TRANSMIT:
            OAMDBGERR("(CLI_REQ_TRANSMIT)\r\n");
            break;
        case CLI_RESP_TRANSMIT:
            OAMDBGERR("(CLI_RESP_TRANSMIT)\r\n");
            break;
        case IGMP_AUTH_TRAN_REQ:
            OAMDBGERR("(IGMP_AUTH_TRAN_REQ)\r\n");
            break;
        case IGMP_AUTH_TRAN_RESP:
            OAMDBGERR("(IGMP_AUTH_TRAN_RESP)\r\n");
            break;
        case CLI_PTY_TRANSMIT:
            OAMDBGERR("(CLI_PTY_TRANSMIT)\r\n");
            break;
        default:
            OAMDBGERR("(unknown)\r\n");
            break;
    }
    OAMDBGERR("    SendSerNo:   %u\r\n", pMessage->SendSerNo);
    OAMDBGERR("    WholePktLen: %u\r\n", pMessage->WholePktLen);
    OAMDBGERR("    RevPktLen:   %u\r\n", pMessage->RevPktLen);
    OAMDBGERR("    SessionID:   ");
    for (i=0; i<8; i++)
        OAMDBGERR("%02X", pMessage->SessionID[i]);
#if 0
    OAMDBGERR("\r\n    TimerID:     %u\r\n", (unsigned int)((pMessage->TimerID).opaque));
#else
    OAMDBGERR("\r\n    TimerID:     %u\r\n", (unsigned int)((pMessage->TimerID)));
#endif
    OAMDBGERR("    Payload: \r\n");
    for (i=0; i<pMessage->RevPktLen; i++)
    {
        if ((i % 16) == 0)
            OAMDBGERR("        ");
        OAMDBGERR("%02X ", pMessage->pPayLoad[i]);
        if ((i % 16) == 15)
            OAMDBGERR("\r\n");
    }
    OAMDBGERR("\r\n");
    OAMDBGERR("Total Rx OAM frames'number: %u\r\n", (unsigned int)gulDebugOamRxCount);
    return GWD_RETURN_OK;
}

int Debug_Print_Tx_OAM(GWTT_OAM_HEADER *avender, unsigned char *pSentData)
{
    int i;
	unsigned short wholePktLen;			/* The whole packet length, including the fragments */
	unsigned short payloadOffset;		/* Offset in the entire packet */
	unsigned short payLoadLength;		/* Payload length in this packet */

	wholePktLen = /*VOS_NTOHS*/(avender->wholePktLen); 		/* LD modified*/
	payloadOffset = /*VOS_NTOHS*/(avender->payloadOffset);	/* LD modified*/
	payLoadLength = /*VOS_NTOHS*/(avender->payLoadLength);	/* LD modified*/
	
    OAMDBGERR("\r\nTx OAM packet as following:\r\n");
    OAMDBGERR("    GwOUI:        %02X-%02X-%02X\r\n", avender->oui[0], avender->oui[1], avender->oui[2]);
    OAMDBGERR("    GwOpCode:     %02X", avender->opCode);
    switch (avender->opCode)
    {
        case EQU_DEVICE_INFO_REQ:
            OAMDBGERR("(EQU_DEVICE_INFO_REQ)\r\n");
            break;
        case EQU_DEVICE_INFO_RESP:
            OAMDBGERR("(EQU_DEVICE_INFO_RESP)\r\n");
            break;
        case ALARM_REQ:
            OAMDBGERR("(ALARM_REQ)\r\n");
            break;
        case ALARM_RESP:
            OAMDBGERR("(ALARM_RESP)\r\n");
            break;
        case FILE_READ_WRITE_REQ:
            OAMDBGERR("(FILE_READ_WRITE_REQ)\r\n");
            break;
        case FILE_RESERVER:
            OAMDBGERR("(FILE_RESERVER)\r\n");
            break;
        case FILE_TRANSFER_DATA:
            OAMDBGERR("(FILE_TRANSFER_DATA)\r\n");
            break;
        case FILE_TRANSFER_ACK:
            OAMDBGERR("(FILE_TRANSFER_ACK)\r\n");
            break;
        case CHURNING:
            OAMDBGERR("(CHURNING)\r\n");
            break;
        case DBA:
            OAMDBGERR("(DBA)\r\n");
            break;
        case SNMP_TRAN_REQ:
            OAMDBGERR("(SNMP_TRAN_REQ)\r\n");
            break;
        case SNMP_TRAN_RESP:
            OAMDBGERR("(SNMP_TRAN_RESP)\r\n");
            break;
        case SNMP_TRAN_TRAP:
            OAMDBGERR("(SNMP_TRAN_TRAP)\r\n");
            break;
        case CLI_REQ_TRANSMIT:
            OAMDBGERR("(CLI_REQ_TRANSMIT)\r\n");
            break;
        case CLI_RESP_TRANSMIT:
            OAMDBGERR("(CLI_RESP_TRANSMIT)\r\n");
            break;
        case IGMP_AUTH_TRAN_REQ:
            OAMDBGERR("(IGMP_AUTH_TRAN_REQ)\r\n");
            break;
        case IGMP_AUTH_TRAN_RESP:
            OAMDBGERR("(IGMP_AUTH_TRAN_RESP)\r\n");
            break;
        case CLI_PTY_TRANSMIT:
            OAMDBGERR("(CLI_PTY_TRANSMIT)\r\n");
            break;
        default:
            OAMDBGERR("(unknown)\r\n");
            break;
    }
    OAMDBGERR("    SendSerNo:    %u\r\n", (unsigned int)(avender->senderSerNo));
    OAMDBGERR("    WholePktLen:  %u\r\n", wholePktLen);
    OAMDBGERR("    PayloadOffSet:%u\r\n", payloadOffset);
    OAMDBGERR("    payLoadLength:%u\r\n", payLoadLength);
    OAMDBGERR("    SessionID:    ");
    for (i=0; i<8; i++)
        OAMDBGERR("%02X", avender->sessionId[i]);
    OAMDBGERR("\r\n    Payload: \r\n");
    for (i=0; i<payLoadLength; i++)
    {
        if ((i % 16) == 0)
            OAMDBGERR("        ");
        OAMDBGERR("%02X ", pSentData[i]);
        if ((i % 16) == 15)
            OAMDBGERR("\r\n");
    }
    OAMDBGERR("\r\n");
    OAMDBGERR("Total Tx OAM frames'number this time: %u\r\n", (unsigned int)gulDebugOamTxCount);
    return GWD_RETURN_OK;
}

int Debug_Print_Rx_OAM_Unkown(unsigned char *pBuffer, unsigned short len)
{
    int i;
    OAMDBGERR("\r\nRx unknown OAM packet as following:\r\n");
    OAMDBGERR("    Payload: \r\n");
    for (i=0; i<len; i++)
    {
        if ((i % 16) == 0)
            OAMDBGERR("        ");
        OAMDBGERR("%02X ", pBuffer[i]);
        if ((i % 16) == 15)
            OAMDBGERR("\r\n");
    }
	return 0;
}

void ONU_Oam_BCStorm_Trap_Report_API(unsigned long slot, unsigned long port, unsigned char operate, unsigned char state,unsigned char *session)
{
	char temp[16]={0};
	
	*(temp) = 22;
	*(temp+3) = slot;
	*(temp+4) = port;
	*(temp+5) = state;
	*(temp+6) = operate;

	if(GWD_RETURN_OK != CommOnuMsgSend(ALARM_REQ,0,temp,7, session))
		OAMDBGERR("Send ALARM_REQ for port %u broadcast storm detect trap failed.\r\n", (unsigned int)port);
	return;
}

int GW_Onu_Sysinfo_Save_To_Flash(VOID)
{
#if 0
	unsigned char *tempBuff = NULL;
#endif
    unsigned char *buff=NULL;
    int size=0;
#if 0
	unsigned char *pConfig = NULL;
#endif
    int ret=0;

    buff=(unsigned char  *)&gw_onu_system_info_total;
    size =sizeof (gw_onu_system_info_total);

   // gw_dump_pkt((unsigned char*)&gw_onu_system_info_total, sizeof(gw_onu_system_info_total), 16);

    ret = call_gwdonu_if_api(LIB_IF_SYSCONF_SAVE, 2, buff, size);

    return ret;
}

static void resetSysInfoToDefault()
{
//	unsigned char ucsDeviceNameDef[] = "";

	unsigned int portnum = gw_onu_read_port_num();

	ONU_SYS_INFO_TOTAL * pi = &gw_onu_system_info_total;

	memset(&gw_onu_system_info_total, 0, sizeof(gw_onu_system_info_total));

	snprintf(pi->device_name, sizeof(pi->device_name), "ONU-%dFE", portnum);

//	snprintf(pi->device_name, sizeof(pi->device_name), "%s", ucsDeviceNameDef);
	pi->product_type = DEVICE_TYPE_UNKNOWN;
	snprintf(pi->serial_no, sizeof(pi->serial_no), "%s", "SN00000001");
	snprintf(pi->sw_version, sizeof(pi->sw_version), "%s", "V1R01B001");
	snprintf(pi->hw_version, sizeof(pi->hw_version), "%s", "V1.0");
	snprintf(pi->hw_manufature_date, sizeof(pi->hw_manufature_date), "%s", "1970-01-01");

	pi->valid_flag = 'E';
}

int GW_Onu_Sysinfo_Get_From_Flash(VOID)
{
	int ret=GWD_RETURN_OK;
	int iLastChar;
//	unsigned char ucsDeviceNameDef[] = "GT811_C";

	memset(&gw_onu_system_info_total, 0, sizeof(gw_onu_system_info_total));

//	if (GWD_RETURN_OK != (ret = get_userdata_from_flash((unsigned char *)&gw_onu_system_info_total, GWD_PRODUCT_CFG_OFFSET,  sizeof(gw_onu_system_info_total))))
	if(GW_OK != call_gwdonu_if_api(LIB_IF_SYSCONF_RESTORE, 2, (unsigned char *)&gw_onu_system_info_total, sizeof(gw_onu_system_info_total)))
	{
	//		memset(&gw_onu_system_info_total, 0, sizeof(gw_onu_system_info_total));
		resetSysInfoToDefault();
//		ret = GWD_RETURN_ERR;
	}
	/* Avoid invalid string data */
	if('E' != gw_onu_system_info_total.valid_flag)
	{
//		memcpy(gw_onu_system_info_total.device_name, ucsDeviceNameDef, sizeof(ucsDeviceNameDef));
		resetSysInfoToDefault();
	}
	iLastChar = sizeof(gw_onu_system_info_total.device_name) - 1;
	gw_onu_system_info_total.device_name[iLastChar] = '\0';
	iLastChar = sizeof(gw_onu_system_info_total.serial_no) - 1;
	gw_onu_system_info_total.serial_no[iLastChar] = '\0';
	iLastChar = sizeof(gw_onu_system_info_total.hw_manufature_date) - 1;
	gw_onu_system_info_total.hw_manufature_date[iLastChar] = '\0';
	/*
	gw_onu_system_info_total.product_type = DEVICE_TYPE_GT870;
	sprintf(gw_onu_system_info_total.sw_version, "V%dR%02dB%03d", 
		SYS_SOFTWARE_MAJOR_VERSION_NO,
		SYS_SOFTWARE_RELEASE_VERSION_NO,
		SYS_SOFTWARE_BRANCH_VERSION_NO);*/
	
	call_gwdonu_if_api(LIB_IF_ONU_VER_GET, 2, gw_onu_system_info_total.sw_version, sizeof(gw_onu_system_info_total.sw_version));
	return ret;
}

int GW_Onu_Sysinfo_Save(void)
{
	/* Save to flash */
	gw_onu_system_info_total.product_type = DEVICE_TYPE_GT813_C;
	gw_onu_system_info_total.valid_flag = 'E';
	/*sprintf(gw_onu_system_info_total.sw_version, "V%dR%02dB%03d", 
		SYS_SOFTWARE_MAJOR_VERSION_NO,
		SYS_SOFTWARE_RELEASE_VERSION_NO,
		SYS_SOFTWARE_BRANCH_VERSION_NO);
	sprintf(gw_onu_system_info_total.hw_version, "V%d.%d", 
		SYS_HARDWARE_MAJOR_VERSION_NO,
		SYS_HARDWARE_RELEASE_VERSION_NO);*/
	
	return GW_Onu_Sysinfo_Save_To_Flash();
}

int GW_Onu_Sysinfo_Get(void)
{
    return GW_Onu_Sysinfo_Get_From_Flash();
}

int cmd_onu_mgt_config_product_date_local(struct cli_def *cli, char *command, char *argv[], int argc)
{
	int year, month, date;
        
    // deal with help
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
        case 1:
            return gw_cli_arg_help(cli, 0,
                "<2007-2100>", "Year",
                 NULL);
        case 2:
            return gw_cli_arg_help(cli, 0,
                "<1-12>", "Month",
                 NULL);
        case 3:
            return gw_cli_arg_help(cli, 0,
                "<1-31>", "date",
                 NULL);
        default:
            return gw_cli_arg_help(cli, argc > 1, NULL);
        }
    }

    if(3 == argc)
    {   
		year = atoi(argv[0]);
		month = atoi(argv[1]);
		date = atoi(argv[2]);

		GW_Onu_Sysinfo_Get();
		sprintf(gw_onu_system_info_total.hw_manufature_date, 
			   	"%d-%02d-%02d", year, month, date);
        
		if (GWD_RETURN_OK != GW_Onu_Sysinfo_Save())
		{
			gw_cli_print(cli, "  System information save error!\r\n");
		}
    } else
    {
        gw_cli_print(cli, "%% Invalid input.");
    }
    
    return CLI_OK;
}
int cmd_onu_mgt_config_product_date(struct cli_def *cli, char *command, char *argv[], int argc)
{
	int year, month, date;
        
    // deal with help
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
        case 1:
            return gw_cli_arg_help(cli, 0,
                "<2007-2100>", "Year",
                 NULL);
        case 2:
            return gw_cli_arg_help(cli, 0,
                "<1-12>", "Month",
                 NULL);
        case 3:
            return gw_cli_arg_help(cli, 0,
                "<1-31>", "date",
                 NULL);
        default:
            return gw_cli_arg_help(cli, argc > 1, NULL);
        }
    }

    if(3 == argc)
    {   
		year = atoi(argv[0]);
		month = atoi(argv[1]);
		date = atoi(argv[2]);
        if((year < 2007) || (year > 2100))
        {
            gw_cli_print(cli,"input year %d error\r\n",year);
            return CLI_ERROR;
        }
        if((month < 1) || (month > 12))
        {
            gw_cli_print(cli,"input Month %d error\r\n",year);
            return CLI_ERROR;
        }
        if((date < 1)|| (date > 31))
        {
            gw_cli_print(cli,"input Date %d error\r\n",year);
            return CLI_ERROR;
        }
		GW_Onu_Sysinfo_Get();
		sprintf(gw_onu_system_info_total.hw_manufature_date, 
			   	"%d-%02d-%02d", year, month, date);
        
		if (GWD_RETURN_OK != GW_Onu_Sysinfo_Save())
		{
			gw_cli_print(cli, "  System information save error!\r\n");
		}
    } else
    {
        gw_cli_print(cli, "%% Invalid input.");
    }
    
    return CLI_OK;
}
int cmd_onu_mgt_config_product_hw_version_local(struct cli_def *cli, char *command, char *argv[], int argc)
{
	int v_major, v_rel;
        
    // deal with help
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
        case 1:
            return gw_cli_arg_help(cli, 0,
                "<1-9>", "Major version",
                 NULL);
        case 2:
            return gw_cli_arg_help(cli, 0,
                "<1-9>", "Release version",
                 NULL);
        default:
            return gw_cli_arg_help(cli, argc > 1, NULL);
        }
    }

    if(2 == argc)
    {   
		v_major = atoi(argv[0]);
		v_rel = atoi(argv[1]);

		sprintf(gw_onu_system_info_total.hw_version, "V%d.%d", 
			v_major, v_rel);
        
		if (GWD_RETURN_OK != GW_Onu_Sysinfo_Save())
		{
			gw_cli_print(cli, "  System information save error!\r\n");
		}
    } else
    {
        gw_cli_print(cli, "%% Invalid input.");
    }
    
    return CLI_OK;
}

int cmd_onu_mgt_config_product_hw_version(struct cli_def *cli, char *command, char *argv[], int argc)
{
	int v_major, v_rel, vb;
        
    // deal with help
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
        case 1:
            return gw_cli_arg_help(cli, 0,
                "<1-9>", "Major version",
                 NULL);
        case 2:
            return gw_cli_arg_help(cli, 0,
                "<0-9>", "Release version",
                 NULL);
        case 3:
        	return gw_cli_arg_help(cli, 0,
        		"<1-9>", "Build version", NULL);
        default:
            return gw_cli_arg_help(cli, argc > 1, NULL);
        }
    }

    if(3 == argc)
    {   
		v_major = atoi(argv[0]);
		v_rel = atoi(argv[1]);
		vb = atoi(argv[2]);
        if((v_major < 1) || (v_major > 9))
        {
            gw_cli_print(cli,"input Major version  %d error\r\n",v_major);
            return CLI_ERROR;
        }
        if((v_rel < 0) || (v_rel > 9))
        {
            gw_cli_print(cli,"input Release version %d error\r\n",v_rel);
            return CLI_ERROR;
        }
        if((vb < 1)|| (vb > 9))
        {
            gw_cli_print(cli,"input Build version %d error\r\n",vb);
            return CLI_ERROR;
        }
		sprintf(gw_onu_system_info_total.hw_version, "V%d.%dB%d",
			v_major, v_rel, vb);
        
		if (GWD_RETURN_OK != GW_Onu_Sysinfo_Save())
		{
			gw_cli_print(cli, "  System information save error!\r\n");
		}
    } else
    {
        gw_cli_print(cli, "%% Invalid input.");
    }
    
    return CLI_OK;
}
int cmd_onu_mgt_config_product_sn_local(struct cli_def *cli, char *command, char *argv[], int argc)
{
	int  len, i;
	unsigned char tmpStr[18];

        
    // deal with help
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
        case 1:
            return gw_cli_arg_help(cli, 0,
                "<string>", "Manufacture serial number(length<16)",
                 NULL);
        default:
            return gw_cli_arg_help(cli, argc > 1, NULL);
        }
    }

    if(1 == argc)
    {   
		if((len = strlen(argv[0])) > 16)
		{
			gw_cli_print(cli, "  The length of serial number must be less than %d.\r\n", 16);
			return CLI_OK;
		}

		for(i=0; i<len; i++)
			tmpStr[i] = TOUPPER(argv[0][i]);
		tmpStr[i] = '\0';
		
		GW_Onu_Sysinfo_Get();
		sprintf(gw_onu_system_info_total.serial_no, "%s", tmpStr);

		if (GWD_RETURN_OK != GW_Onu_Sysinfo_Save())
		{
			gw_cli_print(cli, "  System information save error!\r\n");
		}

		return CLI_OK;
    } else
    {
        gw_cli_print(cli, "%% Invalid input.");
    }
    
    return CLI_OK;
}

int cmd_onu_mgt_config_product_sn(struct cli_def *cli, char *command, char *argv[], int argc)
{
	int  len, i;
	unsigned char tmpStr[18];

        
    // deal with help
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
        case 1:
            return gw_cli_arg_help(cli, 0,
                "<string>", "Manufacture serial number(length<16)",
                 NULL);
        default:
            return gw_cli_arg_help(cli, argc > 1, NULL);
        }
    }

    if(1 == argc)
    {   
		if((len = strlen(argv[0])) > 16)
		{
			gw_cli_print(cli, "  The length of serial number must be less than %d.\r\n", 16);
			return CLI_OK;
		}

		for(i=0; i<len; i++)
			tmpStr[i] = TOUPPER(argv[0][i]);
		tmpStr[i] = '\0';
		
		GW_Onu_Sysinfo_Get();
		sprintf(gw_onu_system_info_total.serial_no, "%s", tmpStr);

		if (GWD_RETURN_OK != GW_Onu_Sysinfo_Save())
		{
			gw_cli_print(cli, "  System information save error!\r\n");
		}

		return CLI_OK;
    } else
    {
        gw_cli_print(cli, "%% Invalid input.");
    }
    
    return CLI_OK;
}
int cmd_onu_mgt_config_device_name_local(struct cli_def *cli, char *command, char *argv[], int argc)
{
	int  len, i;
	unsigned char tmpStr[64];

        
    // deal with help
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
        case 1:
            return gw_cli_arg_help(cli, 0,
                "<string>", "Device name(length< 64)",
                 NULL);
        default:
            return gw_cli_arg_help(cli, argc > 1, NULL);
        }
    }

    if(1 == argc)
    {   
		if((len = strlen(argv[0])) > 64)
		{
			gw_cli_print(cli, "  The length of device name must be less than %d.\r\n", 15);
			return CLI_OK;
		}

		for(i=0; i<len; i++)
			tmpStr[i] = TOUPPER(argv[0][i]);
		tmpStr[i] = '\0';
		
		GW_Onu_Sysinfo_Get();
		sprintf(gw_onu_system_info_total.device_name, "%s", tmpStr);

		if (GWD_RETURN_OK != GW_Onu_Sysinfo_Save())
		{
			gw_cli_print(cli, "  System information save error!\r\n");
		}

		return CLI_OK;
    } else
    {
        gw_cli_print(cli, "%% Invalid input.");
    }
    
    return CLI_OK;
}

int cmd_onu_mgt_config_device_name(struct cli_def *cli, char *command, char *argv[], int argc)
{
	int  len, i;
	unsigned char tmpStr[129];

        
    // deal with help
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
        case 1:
            return gw_cli_arg_help(cli, 0,
                "<string>", "Device name(length<=128)",
                 NULL);
        default:
            return gw_cli_arg_help(cli, argc > 1, NULL);
        }
    }

    if(1 == argc)
    {   
		if((len = strlen(argv[0])) > 128)
		{
			gw_cli_print(cli, "  The length of device name must be less than %d.\r\n", 128);
			return CLI_OK;
		}

		for(i=0; i<len; i++)
			tmpStr[i] = TOUPPER(argv[0][i]);
		tmpStr[i] = '\0';
		
		GW_Onu_Sysinfo_Get();
		sprintf(gw_onu_system_info_total.device_name, "%s", tmpStr);

		if (GWD_RETURN_OK != GW_Onu_Sysinfo_Save())
		{
			gw_cli_print(cli, "  System information save error!\r\n");
		}

		return CLI_OK;
    } else
    {
        gw_cli_print(cli, "%% Invalid input.");
    }
    
    return CLI_OK;
}
int cmd_show_system_information_local(struct cli_def *cli, char *command, char *argv[], int argc)
{
	long lRet = GWD_RETURN_OK;
    char strMac[32];
	
    gw_onu_get_local_mac(strMac);
        
	lRet = GW_Onu_Sysinfo_Get();
	if (lRet != GWD_RETURN_OK)
	{
		gw_cli_print(cli, "  Get product information from flash with error.\r\n");
		return CLI_OK;
	}
	else
	{
        
		gw_cli_print(cli,  "\n  Product information as following--");
		gw_cli_print(cli,  "    ONU type         : %s", "GT873_A");
		gw_cli_print(cli,  "    DeviceName       : %s", gw_onu_system_info_total.device_name);
		gw_cli_print(cli,  "    Hardware version : %s", gw_onu_system_info_total.hw_version);
		gw_cli_print(cli,  "    Software version : %s", gw_onu_system_info_total.sw_version);
		gw_cli_print(cli,  "    Firmware version : %s", iros_version);
		gw_cli_print(cli,  "    Bootload version : %s", irosbootver);
		gw_cli_print(cli,  "    Manufature date  : %s", gw_onu_system_info_total.hw_manufature_date);
		gw_cli_print(cli,  "    Serial number    : %s", gw_onu_system_info_total.serial_no);
    	gw_cli_print(cli,  "    Onu mac address  : %s", strMac);

		return CLI_OK;
	}
}
extern void gwd_onu_poe_cpld_cheak();
int cmd_show_version_build_time(struct cli_def *cli, char *command, char *argv[], int argc)
{
    long lRet = GWD_RETURN_OK;
    char buildtimebuf[VERSION_LEN] = {0};

    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
            default:
                return gw_cli_arg_help(cli, 1, NULL);
        }
    }
       
    lRet = GW_Onu_Sysinfo_Get();
	if (lRet != GWD_RETURN_OK)
	{
		gw_cli_print(cli, "  Get product information from flash with error.\r\n");

		return CLI_ERROR;
	}
    else
    {
        memset(buildtimebuf,0,VERSION_LEN);
        if(call_gwdonu_if_api(LIB_IF_VER_BUILD_TIME_GET,1,buildtimebuf) != GW_OK)
        {
            gw_cli_print(cli,"get version build time fail\r\n");
            return CLI_ERROR;
        }
        gw_cli_print(cli,"Product_Version %s  Platform_Version %s  %s\r\n",gw_onu_system_info_total.sw_version,PLATFORM_VERSION,buildtimebuf);
    }

    return CLI_OK;
    
}
int cmd_show_system_information(struct cli_def *cli, char *command, char *argv[], int argc)
{
	long lRet = GWD_RETURN_OK;
    unsigned char strMac[32];
	

    gw_onu_get_local_mac((gw_macaddr_t*)strMac);

    snprintf(strMac, 32, "%02x:%02x:%02x:%02x:%02x:%02x", strMac[0],
    		strMac[1],strMac[2],strMac[3],strMac[4],strMac[5]);
        
	lRet = GW_Onu_Sysinfo_Get();
	if (lRet != GWD_RETURN_OK)
	{
		gw_cli_print(cli, "  Get product information from flash with error.\r\n");
		return CLI_OK;
	}
	else
	{
		gw_cli_print(cli,  "\n  Product information as following--");
		gw_cli_print(cli,  "    ONU type         : %s", onu_product_name_get(PRODUCT_TYPE));
		gw_cli_print(cli,  "    DeviceName       : %s", gw_onu_system_info_total.device_name);
		gw_cli_print(cli,  "    Hardware version : %s", gw_onu_system_info_total.hw_version);
		gw_cli_print(cli,  "    Software version : %s", gw_onu_system_info_total.sw_version);
		gw_cli_print(cli,  "    Firmware version : %s%s", onu_product_name_get(PRODUCT_TYPE), iros_version);
		gw_cli_print(cli,  "    Bootload version : %s%s", onu_product_name_get(PRODUCT_TYPE), irosbootver);
		gw_cli_print(cli,  "    Manufature date  : %s", gw_onu_system_info_total.hw_manufature_date);
		gw_cli_print(cli,  "    Serial number    : %s", gw_onu_system_info_total.serial_no);
    	gw_cli_print(cli,  "    Onu mac address  : %s", strMac);

		return CLI_OK;
	}
}
int cmd_show_opm_diagnostic_variables_local(struct cli_def *cli, char *command, char *argv[], int argc)
{

	gw_uint16 temp=0, vcc = 0, bias =0, txpow =0, rxpow=0;

    // deal with help
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
        default:
            return gw_cli_arg_help(cli, 1, NULL);
        }
    }

         if(call_gwdonu_if_api(LIB_IF_OPM_GET, 5, &temp, &vcc, &bias, &txpow, &rxpow) != GW_OK)
	{
		gw_cli_print(cli, "  Get optical module diagnostics from I2C with error.\r\n");
		return CLI_OK;
	}
	else
	{
		double txdbm = 0.9, rxdbm = 0.0;
		temp = temp/256;
		vcc = (gw_uint16)(vcc*0.1);
		bias = bias*0.002;
		txdbm = txpow;
		rxdbm = rxpow;

		txdbm = 10*log10(txdbm*0.0001);
		rxdbm = 10*log10(rxdbm*0.0001);
		gw_cli_print(cli,  "\n  optical module diagnostics as following--");
		gw_cli_print(cli,  "    temperature      : %d  cel", temp);
		gw_cli_print(cli,  "    voltage          : %d  mV", vcc);
		gw_cli_print(cli,  "    bias current     : %d  mA", bias);
		gw_cli_print(cli,  "    tx power         : %4.1f  dbm", txdbm);
		gw_cli_print(cli,  "    rx power         : %4.1f  dbm", rxdbm);

		return CLI_OK;
	}

}

int cmd_show_opm_diagnostic_variables(struct cli_def *cli, char *command, char *argv[], int argc)
{

	gw_uint16 temp=0, vcc = 0, bias =0, txpow =0, rxpow=0;

    // deal with help
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
        default:
            return gw_cli_arg_help(cli, 1, NULL);
        }
    }

         if(call_gwdonu_if_api(LIB_IF_OPM_GET, 5, &temp, &vcc, &bias, &txpow, &rxpow) != GW_OK)
	{
		gw_cli_print(cli, "  Get optical module diagnostics from I2C with error.\r\n");
		return CLI_OK;
	}
	else
	{
		double txdbm = 0.9, rxdbm = 0.0;
		temp = temp/256;
		vcc = (gw_uint16)(vcc*0.1);
		bias = bias*0.002;
		txdbm = txpow;
		rxdbm = rxpow;

		txdbm = 10*log10(txdbm*0.0001);
		rxdbm = 10*log10(rxdbm*0.0001);
		gw_cli_print(cli,  "\n  optical module diagnostics as following--");
		gw_cli_print(cli,  "    temperature      : %d  cel", temp);
		gw_cli_print(cli,  "    voltage          : %d  mV", vcc);
		gw_cli_print(cli,  "    bias current     : %d  mA", bias);
		gw_cli_print(cli,  "    tx power         : %4.1f  dbm", txdbm);
		gw_cli_print(cli,  "    rx power         : %4.1f  dbm", rxdbm);

		return CLI_OK;
	}

}

int cmd_show_fdb(struct cli_def * cli, char *command, char *argv[], int argc)
{
	int ret = CLI_OK;
#if 0
	cs_callback_context_t context;
	cs_uint16 idx = 0, next = 0;

	cs_sdl_fdb_entry_t entry;

    // deal with help
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
        default:
            return gw_cli_arg_help(cli, 1, NULL);
        }
    }

    gw_cli_print(cli, "====== FDB SW table is shown:======");
    gw_cli_print(cli, "index   mac_address        vid   port type ");

    while(epon_request_onu_fdb_entry_get_byindex(context, 0, 0, SDL_FDB_ENTRY_GET_MODE_ALL, idx, &entry, &next) == GW_OK)
    {
    	cs_uint16 vid = entry.vlan_id?entry.vlan_id:1;
    	idx = next;
        gw_cli_print(cli, " %2d   %02x:%02x:%02x:%02x:%02x:%02x %6d   %2d   %2d  ", idx,
            entry.mac.addr[0],
            entry.mac.addr[1],
            entry.mac.addr[2],
            entry.mac.addr[3],
            entry.mac.addr[4],
            entry.mac.addr[5],
            vid,
            entry.port,
            entry.type);
    }
    gw_cli_print(cli, "====== Totally %2d SW entries====\n", idx);
#else


    gw_uint32 vid = 0, egports = 0, idx = 0;
	gw_uint8 mac[GW_MACADDR_LEN]={0,0,0,0,0,0};
	gw_uint32 statics=0;

    gw_uint32 retv=0;
    gw_uint32 logport=0;
	gw_uint32 phyport = 0;
	
	
	unsigned char phyportmember[PHY_PORT_MAX ]={0};
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
        default:
            return gw_cli_arg_help(cli, 1, NULL);
        }
    }

    gw_cli_print(cli, " no     mac                   portlist       static         vid        priority");
	gw_cli_print(cli,"----------------------------------------------------------------------------------");

    while(call_gwdonu_if_api(LIB_IF_FDB_ENTRY_GETNEXT, 6, vid, mac, &vid, mac, &egports,&statics) == GW_OK)
    {
        
		retv = onu_bitport_phyport_get(egports,phyportmember);
		
		if(GW_ERROR == retv)
		{
			continue;
		}
		
		for(phyport = 0; phyport < PHY_PORT_MAX; phyport++)
		{
			if(PHY_OK == phyportmember[phyport])
			{
				if(!boards_physical_to_logical(0, phyport, &logport))
				{
					continue;
				}
				else
				{
					if(logport > NUM_PORTS_PER_SYSTEM || logport < NUM_PORTS_MINIMUM_SYSYTEM)
					{
						continue;
					}
					else
					{
						egports = logport;
						break;
					}
				}
			}

		}
        gw_cli_print(cli, "%2d     %02x:%02x:%02x:%02x:%02x:%02x     %6d           %2d           %2d          0", ++idx,
            mac[0],
            mac[1],
            mac[2],
            mac[3],
            mac[4],
            mac[5],
            egports,
            statics-1,
            vid
        	);
		gw_thread_delay(100);
    }
	gw_cli_print(cli,"----------------------------------------------------------------------------------");
	gw_cli_print(cli,"                       Total number of ATU table is %2d.",idx);
#endif
	return ret;
}
int cmd_set_onu_mac_local(struct cli_def *cli, char *command, char *argv[], int argc)
{
    int mac1[6];
    unsigned char mac2[6];
    int ret ;

    int i;
    
    // deal with help
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
        case 1:
            return gw_cli_arg_help(cli, 0,
                "<xx:xx:xx:xx:xx:xx>", "ONU  MAC Address",
                 NULL);
        default:
            return gw_cli_arg_help(cli, argc > 1, NULL);
        }
    }


    if(1 == argc)
    {   
        if(strlen(argv[0]) > 18){
            gw_cli_print(cli,"MAC address configuration is not OK\n");
            return CLI_ERROR;
        }
        
        ret = sscanf(argv[0], "%x:%x:%x:%x:%x:%x", 
            &mac1[0], &mac1[1], &mac1[2], &mac1[3],&mac1[4],&mac1[5]);
        
        if(ret != 6 || mac1[0]&0x01){
            gw_cli_print(cli,"Input MAC is not a unicast MAC\n");
            return CLI_ERROR;
        }

        for(i = 0 ; i < 6; i ++){
            mac2[i] = (unsigned char)mac1[i];
        }

		ret = call_gwdonu_if_api(LIB_IF_ONU_MAC_SET, 1, mac2);

		if(ret == GW_OK)
		{
			gw_onu_set_local_mac(mac2);
			return CLI_OK;
		}

		else
		{
			gw_cli_print(cli, "%% Invalid input.");
			return CLI_ERROR;
		}
    }
    
    return CLI_OK;
}

int cmd_set_onu_mac(struct cli_def *cli, char *command, char *argv[], int argc)
{
    int mac1[6];
    unsigned char mac2[6];
    int ret ;

    int i;
    
    // deal with help
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
        case 1:
            return gw_cli_arg_help(cli, 0,
                "<xx:xx:xx:xx:xx:xx>", "ONU  MAC Address",
                 NULL);
        default:
            return gw_cli_arg_help(cli, argc > 1, NULL);
        }
    }


    if(1 == argc)
    {   
        if(strlen(argv[0]) > 18){
            gw_cli_print(cli,"MAC address configuration is not OK\n");
            return CLI_ERROR;
        }
        
        ret = sscanf(argv[0], "%x:%x:%x:%x:%x:%x", 
            &mac1[0], &mac1[1], &mac1[2], &mac1[3],&mac1[4],&mac1[5]);
        
        if(ret != 6 || mac1[0]&0x01){
            gw_cli_print(cli,"Input MAC is not a unicast MAC\n");
            return CLI_ERROR;
        }

        for(i = 0 ; i < 6; i ++){
            mac2[i] = (unsigned char)mac1[i];
        }

		ret = call_gwdonu_if_api(LIB_IF_ONU_MAC_SET, 1, mac2);

		if(ret == GW_OK)
		{

			gw_onu_set_local_mac(mac2);
			return CLI_OK;

		}
		else
		{
			gw_cli_print(cli, "%% Invalid input.");
			return CLI_ERROR;
		}
    }
    
    return CLI_OK;
}

int cmd_dbg_mod_man(struct cli_def *cli, char *command, char *argv[], int argc)
{

	extern unsigned long   gulDebugRcp;
	extern unsigned long   gulDebugLoopBackDetect;
	extern unsigned long   g_onu_tx_policy;


    // deal with help
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
        case 1:
            return gw_cli_arg_help(cli, 0,
                "{[rcp|loop|txrcp|all]}*1", "module indicator",
                 NULL);
        default:
            return gw_cli_arg_help(cli, argc > 1, NULL);
        }
    }

    if(1 == argc)
    {
    	if(strcmp(argv[0], "rcp") == 0)
    		gulDebugRcp = !gulDebugRcp;
    	if(strcmp(argv[0], "loop") == 0)
    		gulDebugLoopBackDetect = !gulDebugLoopBackDetect;
    	if(strcmp(argv[0], "txrcp") == 0)
    	    g_onu_tx_policy = ! g_onu_tx_policy;

    	if(strcmp(argv[0], "all") == 0)
    	{
    		gulDebugRcp = !gulDebugRcp;
    		gulDebugLoopBackDetect = !gulDebugLoopBackDetect;
    		g_onu_tx_policy = ! g_onu_tx_policy;
    	}
    }
    else
    {
    	gw_cli_print(cli, "debug flag: ");
    	if(gulDebugLoopBackDetect)
    		gw_cli_print(cli, "loop ");
    	if(gulDebugRcp)
    		gw_cli_print(cli, "rcp ");
    	if(!g_onu_tx_policy)
    	    gw_cli_print(cli, "txpolicy");

    	if(!gulDebugLoopBackDetect && !gulDebugRcp && g_onu_tx_policy)
    		gw_cli_print(cli, "none");
    }

    return CLI_OK;
}

int cmd_dbg_lvl_man(struct cli_def *cli, char *command, char *argv[], int argc)
{

	int level = GW_LOG_LEVEL_DEBUG;

    // deal with help
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
        case 1:
            return gw_cli_arg_help(cli, 0,
                "[log|record]", "the class of level",
                 NULL);
        case 2:
        	return gw_cli_arg_help(cli, 0,
        		"{[debug|info|minor|major|cri]}*1","level for log",
        		NULL);
        default:
            return gw_cli_arg_help(cli, argc > 1, NULL);
        }
    }

    if(2 == argc)
    {
    	if(strcmp(argv[1], "debug") == 0)
    		level = GW_LOG_LEVEL_DEBUG;
    	else if(strcmp(argv[1], "info") == 0)
    		level = GW_LOG_LEVEL_INFO;
    	else if(strcmp(argv[1], "minor") == 0)
    		level = GW_LOG_LEVEL_MINOR;
    	else if(strcmp(argv[1], "major") == 0)
    		level = GW_LOG_LEVEL_MAJOR;
    	else
    		level = GW_LOG_LEVEL_CRI;

    	if(strcmp(argv[0], "log") == 0)
    		setGwLogLevel(level);
    	else
    		setGwLogRecordLevel(level);
    }
    else if(argc == 1)
    {

		if (strcmp(argv[0], "log") == 0)
			level = getGwlogLevel();
		else
			level = getGwLogRecordLevel();

		switch (level) {
		case GW_LOG_LEVEL_DEBUG:
			gw_cli_print(cli, "debug");
			break;
		case GW_LOG_LEVEL_INFO:
			gw_cli_print(cli, "info");
			break;
		case GW_LOG_LEVEL_MINOR:
			gw_cli_print(cli, "minor");
			break;
		case GW_LOG_LEVEL_MAJOR:
			gw_cli_print(cli, "major");
			break;
		case GW_LOG_LEVEL_CRI:
			gw_cli_print(cli, "cri");
			break;
		}

	}
    else
    {
        gw_cli_print(cli, "too short arguments!");
    }

    return CLI_OK;
}
#if (RPU_MODULE_NOT_USE == RPU_YES)

int cmd_malloc_space(struct cli_def *cli, char *command, char *argv[], int argc)
{
    int *buf = NULL;
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
            default:
                return gw_cli_arg_help(cli, argc > 1, NULL);
        }
    }
    gw_cli_print(cli,"---------------------malloc test-------------------------\r\n");

    buf = malloc(200);

    if(buf == NULL)
        gw_cli_print(cli,"malloc error\r\n");


    return CLI_OK;
    
}

#endif

extern gw_int32 g_pty_master, g_pty_slave, g_pty_id;
extern gw_uint8 pty_oam_2_telnet;
int cmd_entry_product_cli(struct cli_def *cli, char *command, char *argv[], int argc)
{

	gw_cli_print(cli,"start telnet pty ...\n");
#if 1
	cli->sockfd = 0;
	pty_oam_2_telnet = 1;
	call_gwdonu_if_api(LIB_IF_ENTRY_SDK_CLI, 2, g_pty_slave, g_pty_master);
	pty_oam_2_telnet = 0;
	cli->sockfd = g_pty_slave;
#else
	write(g_pty_slave,"slave write\r\n",14);
	write(g_pty_master,"master write\r\n",14);
#endif
	gw_cli_print(cli,"telnet pty end...\n");
	return CLI_OK;
}

extern int cmd_igmp_snooping_tvm(struct cli_def *cli, char *command, char *argv[], int argc);
extern int cmd_show_igmp_snooping_tvm(struct cli_def *cli, char *command, char *argv[], int argc);

void cli_reg_gwd_cmd(struct cli_command **cmd_root)
{
	//extern void cli_reg_rcp_cmd(struct cli_command **cmd_root);
    struct cli_command *set;
    struct cli_command *show, *sys, *dbg;
    // set cmds in config mode
    set = gw_cli_register_command(cmd_root, NULL, "set", NULL, PRIVILEGE_PRIVILEGED, MODE_CONFIG, "Set system information");
    	gw_cli_register_command(cmd_root, set, "date",    cmd_onu_mgt_config_product_date,     PRIVILEGE_PRIVILEGED, MODE_CONFIG, "Manufacture date");
    	gw_cli_register_command(cmd_root, set, "serial",    cmd_onu_mgt_config_product_sn,     PRIVILEGE_PRIVILEGED, MODE_CONFIG, "Manufacture serial number(<16)");
    	gw_cli_register_command(cmd_root, set, "devicename",    cmd_onu_mgt_config_device_name,     PRIVILEGE_PRIVILEGED, MODE_CONFIG, "Device name(<15)");
    	gw_cli_register_command(cmd_root, set, "hw-version",    cmd_onu_mgt_config_product_hw_version,     PRIVILEGE_PRIVILEGED, MODE_CONFIG, "Hardware version");
	    gw_cli_register_command(cmd_root, set, "mac", cmd_set_onu_mac, PRIVILEGE_PRIVILEGED, MODE_CONFIG, "set mac address");
   

    // display cmds in config mode
    show  = gw_cli_register_command(cmd_root, NULL, "display", NULL, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Show information");
    sys  = gw_cli_register_command(cmd_root, show, "product", cmd_show_system_information, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "System information");
           gw_cli_register_command(cmd_root, show, "version", cmd_show_version_build_time, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "System information");
	gw_cli_register_command(cmd_root, show, "opm", cmd_show_opm_diagnostic_variables, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "optical module diagnostic variables");

/*	atu = gw_cli_register_command(cmd_root, NULL, "atu", NULL, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "fdb table operation");
	gw_cli_register_command(cmd_root, atu, "show", cmd_show_fdb, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "show information");*/
    gw_cli_register_command(cmd_root, NULL, "igmp-snooping-tvm-show", cmd_show_igmp_snooping_tvm, PRIVILEGE_UNPRIVILEGED, MODE_CONFIG, "igmp snooping tvm");
	gw_cli_register_command(cmd_root, NULL, "igmp-snooping-tvm", cmd_igmp_snooping_tvm, PRIVILEGE_UNPRIVILEGED, MODE_DEBUG, "igmp-snooping-tvm config");
	dbg = gw_cli_register_command(cmd_root, NULL, "dbg", NULL, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "debug switch");
		gw_cli_register_command(cmd_root, dbg, "module", cmd_dbg_mod_man, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "management of debug module");
		gw_cli_register_command(cmd_root, dbg, "level", cmd_dbg_lvl_man, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "management of debug level");
#if (RPU_MODULE_NOT_USE == RPU_YES)
        gw_cli_register_command(cmd_root, NULL, "malloc",cmd_malloc_space, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "malloc test");
#endif
       gw_cli_register_command(cmd_root, NULL, "product-cli", cmd_entry_product_cli, PRIVILEGE_PRIVILEGED, MODE_CONFIG, "Entry the product private cli");
    // RCP switch cmds in config mode
//	cli_reg_rcp_cmd(cmd_root);
    return;
}

void cli_reg_gwd_cmd_local(struct cli_command **cmd_root)
{
	//extern void cli_reg_rcp_cmd(struct cli_command **cmd_root);
    struct cli_command *set;
    struct cli_command *show, *sys, *dbg;
    // set cmds in config mode
    set = gw_cli_register_command(cmd_root, NULL, "set", NULL, PRIVILEGE_UNPRIVILEGED, MODE_CONFIG, "Set system information");
    	gw_cli_register_command(cmd_root, set, "date",    cmd_onu_mgt_config_product_date_local,     PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Manufacture date");
    	gw_cli_register_command(cmd_root, set, "serial",    cmd_onu_mgt_config_product_sn_local,     PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Manufacture serial number(<16)");
    	gw_cli_register_command(cmd_root, set, "devicename",    cmd_onu_mgt_config_device_name_local,     PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Device name(<15)");
    	gw_cli_register_command(cmd_root, set, "hw-version",    cmd_onu_mgt_config_product_hw_version_local,     PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Hardware version");
	gw_cli_register_command(cmd_root, set, "mac", cmd_set_onu_mac_local, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "set mac address");

    // display cmds in config mode
    show  = gw_cli_register_command(cmd_root, NULL, "display", NULL, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Show information");
    sys  = gw_cli_register_command(cmd_root, show, "product", cmd_show_system_information_local, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "System information");
	gw_cli_register_command(cmd_root, show, "opm", cmd_show_opm_diagnostic_variables_local, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "optical module diagnostic variables");

/*	atu = gw_cli_register_command(cmd_root, NULL, "atu", NULL, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "fdb table operation");
	gw_cli_register_command(cmd_root, atu, "show", cmd_show_fdb, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "show information");*/

	dbg = gw_cli_register_command(cmd_root, NULL, "dbg", NULL, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "debug switch");
		gw_cli_register_command(cmd_root, dbg, "module", cmd_dbg_mod_man, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "management of debug module");
		gw_cli_register_command(cmd_root, dbg, "level", cmd_dbg_lvl_man, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "management of debug level");

    
    // RCP switch cmds in config mode
//	cli_reg_rcp_cmd(cmd_root);
    return;
}

gw_int32 gw_oam_parser(gw_int8 * pkt, const gw_int32 len)
{
	GWTT_OAM_HEADER * hdr = (GWTT_OAM_HEADER*)pkt;

	if(len < sizeof(GWTT_OAM_HEADER))
		return GW_PKT_MAX;

	if(hdr->std_hdr.type == ntohs(0x8100))
		hdr = (GWTT_OAM_HEADER*)(pkt+4);
	
	if(hdr->std_hdr.type == ntohs(0x8809) &&
			memcmp(hdr->oui, GwOUI, sizeof(GwOUI)) == 0)
		return GW_PKT_OAM;

	return GW_PKT_MAX;

}

gw_status gw_oam_handler(gw_int8 * pkt, const gw_int32 len, gw_int32 portid)
{
	GWTT_OAM_HEADER * hdr = (GWTT_OAM_HEADER*)pkt;
	gw_int32 rlen = len;

	if(hdr->std_hdr.type == ntohs(0x8100))
	{
		memmove(pkt+12, pkt+16, rlen-16);
		rlen-=4;
	}
	
	Gwd_Oam_Handle(portid, pkt, rlen);
	return GW_OK;
}
void gw_broadcast_storm_init()
{
	broad_storm.gulBcStormThreshold = 1000;
	broad_storm.gulBcStormStat = 0;
	return;
}

void gwd_onu_init(void)
{
extern void Rcp_Mgt_init(void);
extern void gw_cli_switch_gwd_cmd(struct cli_command **cmd_root);
extern void gw_cli_debeg_gwd_cmd(struct cli_command **cmd_root);
extern void cli_reg_rcp_cmd(struct cli_command **cmd_root);
extern void gw_cli_reg_oam_cmd(struct cli_command ** cmd_root);
extern void gw_cli_reg_native_cmd(struct cli_command ** cmd_root);
extern void gw_cli_debug_cmd(struct cli_command **cmd_root);
extern void cli_reg_mgtif_cmd(struct cli_command **cmd_root);
extern void init_oam_send_relay();
#if(RPU_MODULE_POE == RPU_YES)
unsigned stat_val;
extern void cli_reg_gwd_poe_cmd(struct cli_command **cmd_root);
#endif
extern void gw_cli_multicast_gwd_cmd(struct cli_command **cmd_root);


	gw_semaphore_init(&g_pkt_send_sem, g_pkt_send_sem_name, 1, 0);

	gw_lpb_detect_init();

	GwOamMessageListInit();

	GW_Onu_Sysinfo_Get();

	init_gw_oam_async();

	init_oam_pty();
    //init_oam_send_relay();/*add by luh 2013-4-27*/
//	init_oamsnmp();

	gw_reg_pkt_parse(GW_PKT_OAM, gw_oam_parser);
	gw_reg_pkt_handler(GW_PKT_OAM, gw_oam_handler);
	gw_broadcast_storm_init();
#if _cmd_line_

	GW_Onu_Sysinfo_Get();

	pStrGwdSwVer = gw_onu_system_info_total.sw_version;
	pStrGwdHwVer = gw_onu_system_info_total.hw_version;

	oam_vendor_handler_register(GwOUI, gwd_oam_handlers);
#endif

	Rcp_Mgt_init();

	userCmdInitHandlerInit();

	if(registerUserCmdInitHandler("gwd", cli_reg_gwd_cmd) != GW_OK)
		gw_printf("regist gwd cmds fail!\r\n");
    
    if(registerUserCmdInitHandler("gwd",gw_cli_debug_cmd) != GW_OK)
		gw_printf("regist gwd cmds fail!\r\n");

	if(registerUserCmdInitHandler("rcp-switch", gw_cli_switch_gwd_cmd) != GW_OK)
		gw_printf("regist rcp  switch cmds fail!\r\n");

	if(registerUserCmdInitHandler("rcp-switch-debug", gw_cli_debeg_gwd_cmd) != GW_OK)
		gw_printf("regist rcp  switch debug cmds fail!\r\n");

	if(registerUserCmdInitHandler("rcp-switch-show", cli_reg_rcp_cmd) != GW_OK)
		gw_printf("regist rcp  switch show cmds fail!\r\n");

	if(registerUserCmdInitHandler("oam-mgt", gw_cli_reg_oam_cmd) != GW_OK)
		gw_printf("regist oam cmds fail!\r\n");

	if(registerUserCmdInitHandler("native_mgt", gw_cli_reg_native_cmd) != GW_OK)
		gw_printf("regist native cmds fail!\r\n");

#if(RPU_MODULE_POE == RPU_YES)
    Gwd_onu_poe_exist_stat_get(&stat_val);
    if(stat_val)
    {
	    if(registerUserCmdInitHandler("gwd", cli_reg_gwd_poe_cmd) != GW_OK)
		    gw_printf("regist gwd poe cmds fail!\r\n");
    }
#endif
	if(registerUserCmdInitHandler("multicast", gw_cli_multicast_gwd_cmd) != GW_OK)
		gw_printf("regist multicast cmds fail!\r\n");
	if(registerUserCmdInitHandler("mgtifconfig", cli_reg_mgtif_cmd) != GW_OK)
		gw_printf("regist multicast cmds fail!\r\n");    
	oam_cli_start();

//	ctc_onu_stats_monitor_init();
}


