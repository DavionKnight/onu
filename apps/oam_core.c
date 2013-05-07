/*
 * oam_core.c
 *
 *  Created on: 2012-11-8
 *      Author: tommy
 */


#include "oam_core.h"
#include "gw_log.h"
#include "Pty.h"
#include "gwdonuif_interval.h"

typedef enum
{
   CON_CTL_CODE_KPL = 1,
   CON_CTL_CODE_CONREQ,
   CON_CTL_CODE_CONRES,
   CON_CTL_CODE_FREEREQ,
   CON_CTL_CODE_FREERES,
   CON_CTL_CODE_DATA
} CONSOLE_CTRL_CODE;

typedef struct _CLI_PTY_CTRL_
{
    long   lConnect;

    long   lFd;

	char   bSessionId[8];

    long   lTimeOut;

	long   lSendKpl;

    unsigned long   lSerNo;

}CLI_PTY_CTRL;


// delcare variables for oam async

#define PTY_CONNECT_REQ_RESP_ACCEPT 3
#define PTY_CONNECT_REQ_RESP_REJECT 2


#define PTY_SHELL_CLOSE_FLAG   0x5192

#define PTY_KPL_TIMEOUT        30

#define PTY_KPL_SENDINT        3

#ifdef CYG_LINUX
#define OAM_VCONPTY_DEBUG(x) gw_log(GW_LOG_LEVEL_DEBUG, x)
#else
#define OAM_VCONPTY_DEBUG(x) printf x
#endif


gw_uint8 g_oam_cli_out_buf[OAM_CLI_OUT_BUF_LENGTH];
gw_uint32 g_oam_cli_out_len = 0;

static gw_uint32 g_oam_async_queue_id = 0,
		g_oam_async_queue_deepth = 100,
		g_oam_async_data_szie = 128,
		g_oam_async_queue_pri = 3,
		g_oam_async_thread_id,
		g_oam_async_thread_stack_size = GW_OSAL_THREAD_STACK_SIZE_HUGE,
		g_oam_async_thread_pri = GW_OSAL_THREAD_PRIO_NORMAL;

gw_int8 g_oam_async_queue_name[]="oam_async_queue";

gw_int8 g_oam_async_thread_name[] = "oam_async_thread";


void gw_dump_buffer(const gw_uint8 *buf, const gw_int32 len)
{
	gw_int32 i;

	gw_printf("\r\n");
	for(i=0; i<len; i++)
	{
		if(!(i&0xf))
			gw_printf("\r\n");
		gw_printf("%02X ", buf[i]);
	}
	gw_printf("\r\n");
}



void gw_oam_async_thread_entry(gw_uint32 * para);


// declare variables for oam pty

gw_uint8 g_pty_dev_name[]="oampty";

gw_int32 g_pty_master, g_pty_slave, g_pty_id;

static gw_uint32 g_oam_pty_cli_thread_id = GW_OSAL_MAX_THREAD,
g_oam_pty_cli_thread_stack_size = 16*1024,
g_oam_pty_cli_thread_pri = GW_OSAL_THREAD_PRIO_NORMAL+10;

static CLI_PTY_CTRL gmCliPtyCtrl = {0,0,{0},0, 0, 0};

static gw_uint32 g_oam_pty_queue_id = 0,
		g_oam_pty_queue_deepth = 100,
		g_oam_pty_data_szie = 128,
		g_oam_pty_queue_pri = 3,
		g_oam_pty_main_thread_id,
		g_oam_pty_main_thread_stack_size = GW_OSAL_THREAD_STACK_SIZE_HUGE,
		g_oam_pty_main_thread_pri = GW_OSAL_THREAD_PRIO_NORMAL,
		g_oam_pty_sub_thread_id,
		g_oam_pty_sub_thread_stack_size = GW_OSAL_THREAD_STACK_SIZE_HUGE,
		g_oam_pty_sub_thread_pri = GW_OSAL_THREAD_PRIO_NORMAL;

static gw_uint32 g_oam_relay_queue_id = 0,
		g_oam_relay_queue_deepth = 200,
		g_oam_relay_data_szie = 128,
		g_oam_relay_queue_pri = 4,
		g_oam_relay_thread_id,
		g_oam_relay_thread_stack_size = GW_OSAL_THREAD_STACK_SIZE_NORMAL,
		g_oam_relay_thread_pri = GW_OSAL_THREAD_PRIO_NORMAL;

static gw_uint8 g_oam_pty_queue_name[]="ptyqueue";
static gw_uint8 g_oam_pty_main_thread_name[]="ptymthread";
static gw_uint8 g_oam_pty_sub_thread_name[]="ptysthread";

static gw_uint8 g_oam_relay_queue_name[]="oamrelayqueue";
static gw_uint8 g_oam_relay_thread_name[]="oamrelaythread";
void gw_oam_pty_main_thread_entry(gw_uint32 * para);
void gw_oam_pty_sub_thread_entry(gw_uint32 * para);
void gw_oam_pty_cli_thread_entry(gw_uint32 * para);
void gw_oam_relay_thread_entry(gw_uint32 * para);

static void OamPtyPacketProcess(GWTT_OAM_SESSION_INFO *pSeInf, char *pPayLoad, long lPayLen);
static void OamPtyConnectReqPro(GWTT_OAM_SESSION_INFO *pSeInf, char *pPayLoad, long lPayLen);
static void OamPtyConFreeReqPro(GWTT_OAM_SESSION_INFO *pSeInf);
static void OamPtyNotiMsgProcess(long int flag, long int fd);
static void OamPtyShellTimerNoti();

gw_status gwd_oam_cli_trans_send_out()
{
	if(g_oam_cli_out_len > OAM_CLI_OUT_BUF_LENGTH)
	{
		gw_log(GW_LOG_LEVEL_DEBUG, ("oam cli out exceed limit, leng is %d\r\n", g_oam_cli_out_len));
		g_oam_cli_out_len = OAM_CLI_OUT_BUF_LENGTH;
	}

	return call_gwdonu_if_api(LIB_IF_PORTSEND, 3, GW_PON_PORT_ID, g_oam_cli_out_buf, g_oam_cli_out_len);
}

gw_int32 gwd_oam_cli_printf(gw_int8 * p, gw_int32 len)
{
	if(len)
	{
		if(len+g_oam_cli_out_len < OAM_CLI_OUT_BUF_LENGTH)
			g_oam_cli_out_len += sprintf(g_oam_cli_out_buf+g_oam_cli_out_len, "%s", p);
		else
		{
			len = OAM_CLI_OUT_BUF_LENGTH-g_oam_cli_out_len-1;
			strncpy(g_oam_cli_out_buf+g_oam_cli_out_len, p, len);
			g_oam_cli_out_len = OAM_CLI_OUT_BUF_LENGTH;
		}
	}

	return g_oam_cli_out_len;
}

void init_gw_oam_async()
{
	if(GW_OK != gw_pri_queue_create(&g_oam_async_queue_id, g_oam_async_queue_name, g_oam_async_queue_deepth,
			g_oam_async_data_szie, g_oam_async_queue_pri))
	{
		gw_log(GW_LOG_LEVEL_DEBUG, ("create oam async queue fail!\r\n"));
		return;
	}

	gw_log(GW_LOG_LEVEL_DEBUG, ("create oam async queue ok!\r\n"));

	if(GW_OK != gw_thread_create(&g_oam_async_thread_id, g_oam_async_thread_name, gw_oam_async_thread_entry, NULL, g_oam_async_thread_stack_size,
			g_oam_async_thread_pri, 0))
	{
		gw_log(GW_LOG_LEVEL_DEBUG, ("create oam async thread fail!\r\n"));
		gw_queue_delete(g_oam_async_queue_id);
	}

	gw_log(GW_LOG_LEVEL_DEBUG, ("create oam async thread ok!\r\n"));
}

void gw_oam_async_thread_entry(gw_uint32 * para)
{
	gw_uint32 buf = 0, len = 0;

	while(1)
	{
		if(gw_pri_queue_get(g_oam_async_queue_id, &buf, sizeof(gw_uint32), &len, GW_OSAL_WAIT_FOREVER) == GW_OK)
		{
			gw_log(GW_LOG_LEVEL_DEBUG,("recv oam async msg!\r\n"));
			if(len > 0)
			{
				GWTT_OAM_MESSAGE_NODE * msg = (GWTT_OAM_MESSAGE_NODE*)buf;

				gw_dump_pkt(msg->pPayLoad, msg->RevPktLen, 16);
				gw_log(GW_LOG_LEVEL_DEBUG, "\r\n");

				switch(msg->GwOpcode)
				{
				case CLI_REQ_TRANSMIT:
//FIX					call shell cmd_execute

				{
//					if(OAM_CLI_OUT_BUF_LENGTH >= msg->RevPktLen)
						//g_oam_cli_out_len = msg->RevPktLen;
//					memcpy(g_oam_cli_out_buf, msg->pPayLoad, g_oam_cli_out_len);

					gw_cli_run_oam_command( msg->pPayLoad);

					gw_log(GW_LOG_LEVEL_DEBUG, "oam_cli result(len == %d):\r\n", g_oam_cli_out_len);
					gw_dump_pkt(g_oam_cli_out_buf, g_oam_cli_out_len, 16);
					gw_log(GW_LOG_LEVEL_DEBUG, "\r\n");

					CommOnuMsgSend(CLI_RESP_TRANSMIT, msg->SendSerNo, g_oam_cli_out_buf, g_oam_cli_out_len, msg->SessionID);

					g_oam_cli_out_len = 0;

					GwOamMessageListNodeFree(msg);
				}
					break;
				case CLI_RESP_TRANSMIT:
					gwd_oam_cli_trans_send_out();
					break;
				}
			}
		}
	}
}

gw_status gwd_oam_async_trans(GWTT_OAM_MESSAGE_NODE * msg)
{
	gw_uint32 data = (gw_uint32)msg;
	return gw_pri_queue_put(g_oam_async_queue_id, &data, sizeof(data), GW_OSAL_WAIT_FOREVER, 0);
}

gw_status gwd_oam_pty_trans(GWTT_OAM_MESSAGE_NODE * msg)
{
	gw_uint32 data[4];
	GWTT_OAM_SESSION_INFO * pSessionInfo = NULL;
	gw_uint8 * payload = NULL;

	payload = malloc(msg->RevPktLen+8);
	pSessionInfo = malloc(sizeof(GWTT_OAM_SESSION_INFO));

	if(payload == NULL || pSessionInfo == NULL)
	{
		if(payload)
			free(payload);
		if(pSessionInfo)
			free(pSessionInfo);
		return GW_ERROR;
	}

	memset(payload, 0, msg->RevPktLen+8);
	memcpy(payload, msg->pPayLoad, msg->RevPktLen);

	pSessionInfo->SendSerNo = msg->SendSerNo;
	memcpy(pSessionInfo->SessionID, msg->SessionID, 8);

	data[0] = msg->RevPktLen;
	data[1] = (gw_uint32)pSessionInfo;
	data[2] = PTY_PACKET;
	data[3] = (gw_uint32)payload;

	if(GW_OK != gw_pri_queue_put(g_oam_pty_queue_id, data, sizeof(data), GW_OSAL_WAIT_FOREVER, 0))
	{
		free(payload);
		free(pSessionInfo);
		gw_log(GW_LOG_LEVEL_DEBUG, ("pty queue put fail!\r\n"));
		return GW_ERROR;
	}

	return GW_OK;
}

struct thread_master *master;

void init_oam_pty()
{
//	init pty device
	g_pty_id = CreatePty(g_pty_dev_name);

	if(g_pty_id != -1)
	{
		g_pty_master = OpenMasterDev(g_pty_id);
		g_pty_slave = OpenSlaveDev(g_pty_id);

		if(g_pty_master != -1 && g_pty_slave != -1)
		{
//			init queue and thread
			if(gw_pri_queue_create(&g_oam_pty_queue_id, g_oam_pty_queue_name, g_oam_pty_queue_deepth,
					g_oam_pty_data_szie, g_oam_pty_queue_pri) != GW_OK)
			{
				DeletePty(g_pty_id);
				gw_log(GW_LOG_LEVEL_DEBUG, ("create %s fail!\r\n", g_oam_pty_queue_name));
				return;
			}

			if(gw_thread_create(&g_oam_pty_main_thread_id, g_oam_pty_main_thread_name,
					gw_oam_pty_main_thread_entry, NULL, g_oam_pty_main_thread_stack_size,
					g_oam_pty_main_thread_pri, 0) != GW_OK)
			{
				DeletePty(g_pty_id);
				gw_queue_delete(g_oam_pty_queue_id);
				gw_log(GW_LOG_LEVEL_DEBUG,("create %s fail !\r\n", g_oam_pty_main_thread_name));
				return;
			}

			if(gw_thread_create(&g_oam_pty_sub_thread_id, g_oam_pty_sub_thread_name,
					gw_oam_pty_sub_thread_entry, NULL, g_oam_pty_sub_thread_stack_size,
					g_oam_pty_sub_thread_pri, 0) != GW_OK)
			{
				gw_log(GW_LOG_LEVEL_DEBUG,("create %s fail !\r\n", g_oam_pty_sub_thread_name));
			}

		}
		else
		{
			DeletePty(g_pty_id);
			g_pty_id = 0;
			gw_log(GW_LOG_LEVEL_DEBUG,("open pty dev  fail !\r\n"));
		}

	}

	gw_circle_timer_add(1000, OamPtyShellTimerNoti, NULL);


}
void init_oam_send_relay()
{
		if(gw_pri_queue_create(&g_oam_relay_queue_id, g_oam_relay_queue_name, g_oam_relay_queue_deepth,
				g_oam_relay_data_szie, g_oam_relay_queue_pri) != GW_OK)
		{
			gw_log(GW_LOG_LEVEL_DEBUG, ("create %s fail!\r\n", g_oam_relay_queue_name));
			return;
		}

		if(gw_thread_create(&g_oam_relay_thread_id, g_oam_relay_thread_name,
				gw_oam_relay_thread_entry, NULL, g_oam_relay_thread_stack_size,
				g_oam_relay_thread_pri, 0) != GW_OK)
		{
			gw_queue_delete(g_oam_relay_queue_id);
			gw_log(GW_LOG_LEVEL_DEBUG,("create %s fail !\r\n", g_oam_relay_thread_name));
			return;
		}
    
}
void start_oamPtyCliThread()
{
	if(g_oam_pty_cli_thread_id == GW_OSAL_MAX_THREAD)
	{
        if(gw_thread_create(&g_oam_pty_cli_thread_id, "ptycli", gw_oam_pty_cli_thread_entry, NULL, g_oam_pty_cli_thread_stack_size,
                g_oam_pty_cli_thread_pri, 0) != GW_OK)
            gw_log(GW_LOG_LEVEL_DEBUG,("create %s fail !\r\n", "testpty"));
	}
}

#if 0
void gw_oam_pty_sub_thread_entry(gw_uint32 * para)
{
	gw_uint8 rdata[128]="";
	gw_int32 length = 0;

	while(1)
	{
		memset(rdata, 0, sizeof(rdata));
		length = pty_read(g_pty_master, rdata+1, sizeof(rdata)-1);
		if(length > 0)
		{ 
            //gw_int32 data[4];
            //char *pdata = malloc(length+1);
			rdata[length+1] = 0;
#if 0
			gw_printf("pty sub thread recv:\r\n");
			gw_dump_buffer(rdata+1, length);
#else
#endif
			rdata[0] = 6;
//			*(gw_uint16*)(rdata+1) = htons(length);
#if 0
            if(pdata)
            {                
                memcpy(pdata, rdata, length+1);
                data[0] = 0;
            	data[1] = OAM_RELAY;
            	data[2] = (gw_uint32)pdata;
            	data[3] = length+1;                            
            }
            else
            {
                
        		gw_log(GW_LOG_LEVEL_DEBUG, ("oam relay thread molloc fail!\r\n"));
                continue;
            }
#endif            
#if 1
			CommOnuMsgSend(CLI_PTY_TRANSMIT, gmCliPtyCtrl.lSerNo++, rdata, length+1, gmCliPtyCtrl.bSessionId);
#else
        	if(GW_OK != gw_pri_queue_put(g_oam_relay_queue_id, data, sizeof(data), GW_OSAL_WAIT_FOREVER, 0))
        	{
        		free(pdata);
        		gw_log(GW_LOG_LEVEL_DEBUG, ("oam relay queue put fail!\r\n"));
        		return GW_ERROR;
        	}

#endif
		}
	}
}
#else
void gw_oam_pty_sub_thread_entry(gw_uint32 * para)
{
	gw_uint8 rdata[128]="";
	gw_int32 length = 0;

	gw_uint8 *rbuf = malloc(4096);

	if(rbuf == NULL)
		return;

	while(1)
	{
		memset(rdata, 0, sizeof(rdata));
		length = pty_read(g_pty_master, rbuf+1, 4094);
		rbuf[length+1] = 0;

//		gw_printf("\r\nrecv msg len: %d\r\n", length);
//		gw_printf("%s", rbuf+1);
		
		if(length > 0)
		{ 
			gw_thread_delay(30);
			rbuf[0] = 6;
#if 0
				gw_printf("pty sub thread recv:\r\n");
				gw_dump_buffer(rdata+1, length);
#else
#endif
	//			*(gw_uint16*)(rdata+1) = htons(length);
#if 0
	            if(pdata)
	            {                
	                memcpy(pdata, rdata, length+1);
	                data[0] = 0;
	            	data[1] = OAM_RELAY;
	            	data[2] = (gw_uint32)pdata;
	            	data[3] = length+1;                            
	            }
	            else
	            {
	                
	        		gw_log(GW_LOG_LEVEL_DEBUG, ("oam relay thread molloc fail!\r\n"));
	                continue;
	            }
#endif            
#if 1
				CommOnuMsgSend(CLI_PTY_TRANSMIT, gmCliPtyCtrl.lSerNo++, rbuf, length+1, gmCliPtyCtrl.bSessionId);
#else
	        	if(GW_OK != gw_pri_queue_put(g_oam_relay_queue_id, data, sizeof(data), GW_OSAL_WAIT_FOREVER, 0))
	        	{
	        		free(pdata);
	        		gw_log(GW_LOG_LEVEL_DEBUG, ("oam relay queue put fail!\r\n"));
	        		return GW_ERROR;
	        	}	
#endif
		}
	}
}
#endif

void gw_oam_pty_cli_thread_entry(gw_uint32 * para)
{

#if 0
	gw_uint8 rdata[128]="";
	gw_int32 length = 0;

	while(1)
	{
		memset(rdata, 0, sizeof(rdata));
		length = pty_read(g_pty_slave, rdata, sizeof(rdata));
		if(length > 0)
		{
			gw_log(GW_LOG_LEVEL_DEBUG, ("recv pty request msg %s\r\n", rdata));
//			CommOnuMsgSend(CLI_PTY_TRANSMIT, gmCliPtyCtrl.lSerNo, rdata, length, gmCliPtyCtrl.bSessionId);
			pty_write(g_pty_slave, rdata, length);
		}
		gw_thread_delay(20);
	}
#else
	cli_start();
#endif
}

void gw_oam_pty_main_thread_entry(gw_uint32 * para)
{
	gw_uint32 aumsg[4], len = 0;

	while(1)
	{
		if(gw_pri_queue_get(g_oam_pty_queue_id, aumsg, sizeof(aumsg), &len, GW_OSAL_WAIT_FOREVER) == GW_OK)
		{
			if(len > 0)
			{
				switch(aumsg[2])
				{
				case PTY_PACKET:
					OamPtyPacketProcess((GWTT_OAM_SESSION_INFO*)aumsg[1], (gw_uint8 *)aumsg[3], aumsg[0]);
					break;
				case PTY_NOTI_MSG:
					OamPtyNotiMsgProcess(aumsg[1], aumsg[3]);
					break;
				case PTY_TIMER_MSG:
					OamPtyTimerMsgProcess();
					break;
				case PTY_ONU_LOSE:
					break;
				}
			}
		}
	}
}
void gw_oam_relay_thread_entry(gw_uint32 * para)
{
	gw_uint32 aumsg[4], len = 0;

	while(1)
	{
        len = 0;
		if(gw_pri_queue_get(g_oam_relay_queue_id, aumsg, sizeof(aumsg), &len, GW_OSAL_WAIT_FOREVER) == GW_OK)
		{
			if(len > 0)
			{
   				switch(aumsg[1])
				{
				case OAM_RELAY:
        			CommOnuMsgSend(CLI_PTY_TRANSMIT, gmCliPtyCtrl.lSerNo++, (unsigned char *)aumsg[2], (unsigned short)aumsg[3], gmCliPtyCtrl.bSessionId);
					break;
				default:
					break;
				}
                if(aumsg[2])
                    free((unsigned char *)aumsg[2]);
			}
		}
		gw_thread_delay(5);
        
	}
}

void OamPtyShellCloseNoti(long lFd)
{
	unsigned long   lMsg[4] = {0};

	lMsg[0] = 0;
    lMsg[1] = PTY_SHELL_CLOSE_FLAG;
    lMsg[2] = PTY_NOTI_MSG;
    lMsg[3] = (unsigned long)lFd;

	if(GW_OK != gw_pri_queue_put(g_oam_pty_queue_id, lMsg, sizeof(lMsg), GW_OSAL_WAIT_FOREVER, 0))
	{
		gw_log(GW_LOG_LEVEL_DEBUG, ("OamPtyShellCloseNoti send msg fail!\r\n"));
	}
}

void OamPtyShellTimerNoti()
{
	unsigned long   lMsg[4] = {0};

	lMsg[0] = 0;
    lMsg[1] = 0;
    lMsg[2] = PTY_TIMER_MSG;
    lMsg[3] = 0;

	if(GW_OK != gw_pri_queue_put(g_oam_pty_queue_id, lMsg, sizeof(lMsg), GW_OSAL_WAIT_FOREVER, 0))
	{
		gw_log(GW_LOG_LEVEL_DEBUG, ("OamPtyShellTimerNoti send msg fail!\r\n"));
	}
}

void OamPtyTimerMsgProcess()
{
	if( (gmCliPtyCtrl.lConnect))
	{

		gmCliPtyCtrl.lTimeOut++;

		if(gmCliPtyCtrl.lTimeOut >= 30)
		{
			OamPtyShellCloseNoti(g_pty_master);
			gw_log(GW_LOG_LEVEL_MINOR, "pty time out, closed!\r\n");
		}
	}
}

void OamPtyNotiMsgProcess(long int flag, long int fd)
{
	if( (gmCliPtyCtrl.lConnect) && flag == PTY_SHELL_CLOSE_FLAG && fd == gmCliPtyCtrl.lFd)
	{
		char bRes[8];
		bRes[0] = CON_CTL_CODE_FREEREQ;

		CommOnuMsgSend(CLI_PTY_TRANSMIT, gmCliPtyCtrl.lSerNo++, bRes, 1, gmCliPtyCtrl.bSessionId);
		gw_thread_delay(100);
		CommOnuMsgSend(CLI_PTY_TRANSMIT, gmCliPtyCtrl.lSerNo++, bRes, 1, gmCliPtyCtrl.bSessionId);

		gmCliPtyCtrl.lConnect = 0;
		gmCliPtyCtrl.lSendKpl = 0;
		gmCliPtyCtrl.lTimeOut = 0;
		gmCliPtyCtrl.lFd = 0;

		memset(gmCliPtyCtrl.bSessionId, 0, sizeof(gmCliPtyCtrl.bSessionId));

		gw_thread_delete(g_oam_pty_cli_thread_id);

		g_oam_pty_cli_thread_id = GW_OSAL_MAX_THREAD;
	}
}

int cl_pty_fd_get(void(*closenoti)(long))
{
//    cl_serv_pty_closenoti = closenoti;

    if (1)
    /*end wugang*/
	     return(g_pty_master);
	else return(-1);
}

static void OamPtyPacketProcess(GWTT_OAM_SESSION_INFO *pSeInf, char *pPayLoad, long lPayLen)
{
    char lCtlCode;

    if((pSeInf == NULL) || (pPayLoad == NULL))
    {
        if(pSeInf)   free(pSeInf);
		if(pPayLoad) free(pPayLoad);
        return;
    }

    lCtlCode = pPayLoad[0];

	switch(lCtlCode)
    {
        case CON_CTL_CODE_KPL:
			 if((gmCliPtyCtrl.lConnect) &&
                (memcmp(gmCliPtyCtrl.bSessionId, pSeInf->SessionID, 8) == 0))
			 {
                 gmCliPtyCtrl.lTimeOut = 0;
			 }
			 break;
		case CON_CTL_CODE_CONREQ:
			 OamPtyConnectReqPro(pSeInf, pPayLoad, lPayLen);
			 break;

		case CON_CTL_CODE_CONRES:
			 break;

		case CON_CTL_CODE_FREEREQ:
			 OamPtyConFreeReqPro(pSeInf);
			 break;

		case CON_CTL_CODE_FREERES:
             break;
		case CON_CTL_CODE_DATA:

			 if((gmCliPtyCtrl.lConnect) &&
                (memcmp(gmCliPtyCtrl.bSessionId, pSeInf->SessionID, 8) == 0))
			 {
				 char *p = strstr(pPayLoad+1, "\r\n");

				 if(p)
					 lPayLen--;

                 gmCliPtyCtrl.lTimeOut = 0;

#if 0
                 {
				 	unsigned char *p =(unsigned char*) pPayLoad;
					 int i=0;
					 gw_printf("pty data payload:\r\n");

					 for(;i<lPayLen; i++)
					 {
						 gw_printf("%02X ", *(p+1+i));
					 }
					 gw_printf("\r\n");
                 }
#endif

				 lPayLen --;
				 if(*(pPayLoad+1) == 0x0a)
					 *(pPayLoad+1) = 0x0d;

//				 if((lPayLen > 0) && (gmCliPtyCtrl.lFd >= 0)) pty_write(gmCliPtyCtrl.lFd, pPayLoad + 1, lPayLen);
				 if((lPayLen > 0) && (gmCliPtyCtrl.lFd >= 0)) 
				 {
				 	int i = 0;
					for(i=0; i<lPayLen; i++)
					 	pty_write(gmCliPtyCtrl.lFd, pPayLoad + 1+i, 1);
				 }
				 else
				 {
					 OAM_VCONPTY_DEBUG(("pty main can't write: paylen %d, fd %d\r\n", lPayLen, gmCliPtyCtrl.lFd));
				 }
			 }
			 break;
    }

    if(pSeInf)   free(pSeInf);
    if(pPayLoad) free(pPayLoad);
}


static void OamPtyConnectReqPro(GWTT_OAM_SESSION_INFO *pSeInf, char *pPayLoad, long lPayLen)
{
    char bResBuf[8] = {0};

	if(gmCliPtyCtrl.lConnect)
	{
	    if(memcmp(gmCliPtyCtrl.bSessionId, pSeInf->SessionID, 8) == 0)
	    {
         	bResBuf[0] = CON_CTL_CODE_CONRES;
			bResBuf[1] = PTY_CONNECT_REQ_RESP_ACCEPT;  /* Í¬Òâ */
			/*if(pPayLoad[1] != 0)
			{
				char   *errmsg = "";
				mn_set_hostname(&pPayLoad[2], &errmsg, 1);
			}*/
	    }
		else
	    {
       	    bResBuf[0] = CON_CTL_CODE_CONRES;
			bResBuf[1] = PTY_CONNECT_REQ_RESP_REJECT;
	    }

		CommOnuMsgSend(CLI_PTY_TRANSMIT, gmCliPtyCtrl.lSerNo++, bResBuf, 2, pSeInf->SessionID);
	}
	else
	{
		gmCliPtyCtrl.lFd = cl_pty_fd_get(OamPtyShellCloseNoti);
		OAM_VCONPTY_DEBUG(("\r\n OnuPty : gmCliPtyCtrl.lFd = %d \r\n", gmCliPtyCtrl.lFd));

//		if(gmCliPtyCtrl.lFd >= 0)
//			VOS_PtyBufferFlush(gmCliPtyCtrl.lFd);

//		gmSubTaksId = VOS_TaskCreate("tCliSubPty", 120, PTY_SUB_TASK, NULL);

		if((gmCliPtyCtrl.lFd > 0) /*&& (gmSubTaksId > 0)*/)
		{
			gmCliPtyCtrl.lConnect = TRUE;
			gmCliPtyCtrl.lSerNo   = 0;
			gmCliPtyCtrl.lTimeOut = 0;
			gmCliPtyCtrl.lSendKpl = 0;

			memcpy(gmCliPtyCtrl.bSessionId, pSeInf->SessionID, 8);

			start_oamPtyCliThread();

		    bResBuf[0] = CON_CTL_CODE_CONRES;
			bResBuf[1] = PTY_CONNECT_REQ_RESP_ACCEPT;
	    }
		else
	    {
            gmCliPtyCtrl.lConnect = FALSE;
			gmCliPtyCtrl.lSerNo   = 0;
			gmCliPtyCtrl.lTimeOut = 0;
			gmCliPtyCtrl.lSendKpl = 0;

			memset(gmCliPtyCtrl.bSessionId, 0, 8);

       	    bResBuf[0] = CON_CTL_CODE_CONRES;
			bResBuf[1] = PTY_CONNECT_REQ_RESP_REJECT;

//			OamPtyDeleteSubTask();
	    }

        OAM_VCONPTY_DEBUG(("\r\n OnuPty : Send msg to OLT(%d)\r\n", bResBuf[1]));
		CommOnuMsgSend(CLI_PTY_TRANSMIT, gmCliPtyCtrl.lSerNo++, bResBuf, 2, pSeInf->SessionID);
	}

}

static void OamPtyConFreeReqPro(GWTT_OAM_SESSION_INFO *pSeInf)
{
	if(gmCliPtyCtrl.lConnect)
	{
		 if(memcmp(gmCliPtyCtrl.bSessionId, pSeInf->SessionID, 8) == 0)
	    {
	        char  bResBuf[8] = {0};
            char  bQuitCmd[8] = {13, 13, 3, 0};
			int i = 0;

			bResBuf[0] = CON_CTL_CODE_FREERES;
			bResBuf[1] = PTY_CONNECT_REQ_RESP_ACCEPT;  /* Í¬Òâ */

			CommOnuMsgSend(CLI_PTY_TRANSMIT, gmCliPtyCtrl.lSerNo++, bResBuf, 2, pSeInf->SessionID);

//			pty_write(gmCliPtyCtrl.lFd, bQuitCmd, 3);                    /*Ä£Äâ·¢ËÍquit*/

			gw_thread_delay(100);

            strcpy(bQuitCmd, "quit\r");

			for(i=0; i<strlen(bQuitCmd); i++)
	            pty_write(gmCliPtyCtrl.lFd, bQuitCmd+i, 1); /*Ä£Äâ·¢ËÍquit*/

//			OamPtyDeleteSubTask();

//			cl_pty_fd_free(gmCliPtyCtrl.lFd);

//              comment by wangxy 2013-05-06
//			gmCliPtyCtrl.lConnect = FALSE;
//			gmCliPtyCtrl.lFd      = 0;
//			memset(gmCliPtyCtrl.bSessionId, 0, 8);
	    }
	}
}

