#include "../include/gwdigmptvm.h"
#include "oam.h"

#if (RPU_MODULE_IGMP_TVM == RPU_YES)

/**********************************************************************************************************************************************
*函数名：igmp_relation_tabel_del
*函数功能描述：组播关系表中删除元素
*函数返回值：int :0-成功，-1-失败
**********************************************************************************************************************************************/
extern int igmp_relation_tabel_ip_del(gw_uint32 GroupSt, gw_uint32 GroupEnd)
{
	int ret = GW_ERROR;
	gw_ulong32 ip_start;
	gw_ulong32 ip_end;

	ip_start = GroupSt;
	ip_end = GroupEnd;
	ret = call_gwdonu_if_api(LIB_IF_TVM_RELATION_TABEL_ITEM_ADD,2,ip_start,ip_end);
	return ret;
}
/**********************************************************************************************************************************************
*函数名：oam_igmp_relation_tabel_ip_del
*函数功能描述：通过oam 协议， 组播关系表中查找 ip ,删除元素
*函数返回值：int :0-成功，-1-失败
**********************************************************************************************************************************************/
static int oam_igmp_relation_tabel_ip_del(void *message_input)
{
	oam_through_vlan_igmp_t *message = NULL;
    if(message_input == NULL)
        return GW_ERROR;
	message = (oam_through_vlan_igmp_t *)((GWTT_OAM_MESSAGE_NODE *)message_input)->pPayLoad;

	gw_uint16 count = 0;
	memcpy(&count, message->count, 2);
	count = ntohs(count);

	gw_uint32 start_ip = 0;
	gw_uint32 end_ip = 0;

	if(0 == count)
	{
		call_gwdonu_if_api(LIB_IF_TVM_RELATION_TABEL_CLEAR,0,NULL);
	}
	else
	{
		int i = 0;
		for(i=0;i<count;i++)
		{
			igmp_relation_table_t *igmp_relation_table = NULL;
			int offset= 0;		/*解决字节对齐的问题*/
			offset = 2;
			igmp_relation_table = (igmp_relation_table_t *)(&(message->igmp_relation_table));
			memcpy(&start_ip, igmp_relation_table->start_ip-offset, 4);
			start_ip = ntohl(start_ip);
			memcpy(&end_ip, igmp_relation_table->end_ip-offset, 4);
			end_ip = ntohl(end_ip);

			igmp_relation_tabel_ip_del(start_ip, end_ip);
			igmp_relation_table++;
		}	
	}
	return 0;
}

static int igmp_relation_tabel_add_elem(Through_Vlan_Group_t *table)
{
    if(table == NULL)
        return GW_ERROR;
	gw_int32 ret = GW_OK;
	gw_ulong32 ip_start;
	gw_ulong32 ip_end;
	gw_uint16 vid;

	ip_start = table->GroupSt;
	ip_end = table->GroupEnd;
	vid = table->IVid;
    ret = call_gwdonu_if_api(LIB_IF_TVM_RELATION_TABEL_ITEM_ADD, 3,ip_start,ip_end,vid);
    #ifdef __tvm_debug__
	ret = addIgmpSnoopTvmItem(ip_start, ip_end, vid);
    #endif
	return ret;
}
/**********************************************************************************************************************************************
*函数名：oam_igmp_relation_tabel_add
*函数功能描述：通过oam 私有协议，向组播关系表中增加成员
*函数参数：
*函数返回值：int :0-成功，-1-失败
**********************************************************************************************************************************************/
static int oam_igmp_relation_tabel_add(void *message_input)
{
	gwdtvmprint("\n****in %s, line :%d***\n", __func__, __LINE__);
    if(message_input == NULL)
        return GW_ERROR;
	oam_through_vlan_igmp_t *message = NULL;
	message = (oam_through_vlan_igmp_t *)((GWTT_OAM_MESSAGE_NODE *)message_input)->pPayLoad;
	gw_uint16 count = 0;
	memcpy(&count, message->count, 2);
	count = ntohs(count);
	gw_uint16 VID = 0;
	gw_uint32 start_ip = 0;
	gw_uint32 end_ip = 0;
	gw_uint16 pon_id = 0;
	gw_uint32 ulIfindex = 0;
	gw_uint32 llid = 0;
	gwdtvmprint("count :0x%x\n", count);

	igmp_relation_table_t *igmp_relation_table = NULL;
	igmp_relation_table = &(message->igmp_relation_table);

	int i = 0;
	for(i=0;i<count;i++)
	{
		int offset= 0;
		offset = 2;
		memcpy(&VID, igmp_relation_table->VID-offset, 2);
		VID = ntohs(VID);
		memcpy(&start_ip, igmp_relation_table->start_ip-offset, 4);
		start_ip = ntohl(start_ip);
		memcpy(&end_ip, igmp_relation_table->end_ip-offset, 4);
		end_ip = ntohl(end_ip);

		memcpy(&pon_id, igmp_relation_table->pon_id-offset, 2);
		pon_id = ntohs(pon_id);
		memcpy(&ulIfindex, igmp_relation_table->ulIfindex-offset, 4);
		ulIfindex = ntohl(ulIfindex);
		memcpy(&llid, igmp_relation_table->llid-offset, 4);
		llid = ntohl(llid);
		
		
		

		gwdtvmprint("VID :0x%x\n", VID);
		gwdtvmprint("start_ip :0x%x\n", start_ip);
		gwdtvmprint("end_ip :0x%x\n", end_ip);

		gwdtvmprint("pon_id :0x%x\n", pon_id);
		gwdtvmprint("ulIfindex :0x%x\n", ulIfindex);
		gwdtvmprint("llid :0x%x\n", llid);


		Through_Vlan_Group_t table;
		memset(&table, 0, sizeof(Through_Vlan_Group_t));
		table.IVid = VID;
		table.GroupSt = start_ip;
		table.GroupEnd = end_ip;
		table.PonId = pon_id;
		table.ulIfIndex = ulIfindex;
		table.llid = llid;
		igmp_relation_tabel_add_elem(&table);

		igmp_relation_table++;

	}

	return 0;
}

/**********************************************************************************************************************************************
*函数名：igmp_tvm_status_set
*函数功能描述： 设置跨vlan组播功能的状态
*函数参数：int status, 0-跨vlan组播不使能， 1-跨vlan组播使能
*函数返回值：0:成功设置
**********************************************************************************************************************************************/

extern int igmp_tvm_status_set(int status)
{
	if(TVM_DISABLE== status)
	{
        call_gwdonu_if_api(LIB_IF_TVM_STATUS_SET, 1,TVM_DISABLE);
        call_gwdonu_if_api(LIB_IF_TVM_RELATION_TABEL_CLEAR,0,NULL);
        call_gwdonu_if_api(LIB_IF_CTC_Mcast_CTRL_TABEL_CLEAR,0,NULL);
        #ifdef __tvm_debug__
		g_ulIgmp_TVM_Enable = VOS_NO;
		igmp_relation_tabel_clear();
		igmp_table_clr();
        #endif
		
	}
	else
	{
        call_gwdonu_if_api(LIB_IF_TVM_STATUS_SET, 1,TVM_ENABLE);
        #ifdef __tvm_debug__
    	g_ulIgmp_TVM_Enable = VOS_YES;
        #endif
	}
	return 0;	
}
/**********************************************************************************************************************************************
*函数名：oam_igmp_relation_tabel_request
*函数功能描述：向 olt 发送同步所有组播关系表的请求
*函数参数： 
*函数返回值：int :0-成功，-1-失败
**********************************************************************************************************************************************/
extern int oam_igmp_relation_tabel_request(void *message_input)
{
    if(message_input == NULL)
        return GW_ERROR;
	gwdtvmprint("in oam_igmp_relation_tabel_request\n");

	GWTT_OAM_MESSAGE_NODE *pRequest = (GWTT_OAM_MESSAGE_NODE *)message_input;
	gw_uint8 Response[100]={'\0'};
	gw_uint8*ptr = NULL;
	int ResLen=0;

	ptr = Response;
	/* Payload */
	gw_uint16 *enable_olt = NULL;
	enable_olt = (gw_uint16 *)ptr;
	gw_uint16 enable_onu = 1;
	*enable_olt = htons(enable_onu);
	ptr = ptr + sizeof(gw_uint16);
	gw_uint16 *type_olt = NULL;
	type_olt = (gw_uint16 *)ptr;
	gw_uint16 type_onu = 5;
	*type_olt = htons(type_onu);
	ptr = ptr + sizeof(gw_uint16);

	
	// 向olt 发送  message_output 
	ResLen = ((unsigned long)ptr-(unsigned long)Response);	
	return (CommOnuMsgSend(IGMP_TVM_RESP, pRequest->SendSerNo, Response, ResLen, pRequest->SessionID));
}
static int oam_igmp_relation_tabel_synchronism(void *message_input)
{
	gwdtvmprint("\n****in oam_igmp_relation_tabel_synchronism***\n");

    if(message_input == NULL)
        return GW_ERROR;
	oam_through_vlan_igmp_t *message = NULL;
	message = (oam_through_vlan_igmp_t *)((GWTT_OAM_MESSAGE_NODE *)message_input)->pPayLoad;
//	message = (oam_through_vlan_igmp_t *)message_input;
	gw_uint32 crc_onu = 0;
	gw_uint32 crc_olt = 0;
	call_gwdonu_if_api(LIB_IF_TVM_RELATION_TABEL_CRC_GET,1,&crc_onu);
	memcpy(&crc_olt, message->crc, 4);
	crc_olt = ntohl(crc_olt);

	gwdtvmprint("message->crc :0x%x\n", message->crc);
	gwdtvmprint("crc_onu :0x%x\n", crc_onu);
	gwdtvmprint("crc_olt :0x%x\n", crc_olt);

	if(crc_onu == crc_olt)
	{
		//do nothing
	}
	else
	{
		call_gwdonu_if_api(LIB_IF_TVM_RELATION_TABEL_CLEAR,0,NULL);
		//ONU请求OLT发送所有的组播关系对应表
		oam_igmp_relation_tabel_request(message_input);
	}
	return 0;
}
/**********************************************************************************************************************************************
*函数名：oam_igmp_relation_tabel_vlan_del
*函数功能描述：通过oam 协议， 组播关系表中查找 vlan ,删除元素
*函数返回值：int :0-成功，-1-失败
**********************************************************************************************************************************************/
static int oam_igmp_relation_tabel_vlan_del(void *message_input)
{
	gwdtvmprint("\n****in oam_igmp_relation_tabel_vlan_del***\n");
    if(message_input == NULL)
        return GW_ERROR;
	oam_through_vlan_igmp_t *message = NULL;
	message = (oam_through_vlan_igmp_t *)((GWTT_OAM_MESSAGE_NODE *)message_input)->pPayLoad;
	gw_uint16 count = 0;
	memcpy(&count, message->count, 2);
	count = ntohs(count);

	gw_uint16 vlan_id = 0;
	if(0 == count)
	{
		call_gwdonu_if_api(LIB_IF_TVM_RELATION_TABEL_CLEAR,0,NULL);
	}
	else
	{
		int i = 0;
		for(i=0;i<count;i++)
		{
			int offset = 0;
			offset = 2;
			igmp_relation_table_t *igmp_relation_table = NULL;
			igmp_relation_table = (igmp_relation_table_t *)(&(message->igmp_relation_table));
			memcpy(&vlan_id, igmp_relation_table->VID-offset, 2);
			vlan_id = ntohs(vlan_id);
			call_gwdonu_if_api(LIB_IF_TVM_RELATION_TABEL_VLAN_DELETE,1,vlan_id);
			igmp_relation_table++;
		}	
	}
	return 0;
}


/**********************************************************************************************************************************************
*函数名：oam_through_vlan_igmp_proc
*函数功能描述：处理olt 下发的管理跨vlan 组播的oam 包
*函数参数：message_input-oam 包
*函数返回值：int 0-成功
**********************************************************************************************************************************************/
static int oam_through_vlan_igmp_proc(void *message_input)
{
	oam_through_vlan_igmp_t *message = NULL;
    if(message_input == NULL)
        return GW_ERROR;
	message = (oam_through_vlan_igmp_t *)((GWTT_OAM_MESSAGE_NODE *)message_input)->pPayLoad;
	gw_uint16 enable = 0;
	gw_uint16 type = 0;
	memcpy(&enable, message->enable, 2);
	enable = ntohs(enable);
	memcpy(&type, message->type, 2);
	type = ntohs(type);
    
	gwdtvmprint("enable 0x%x\n", enable);
	gwdtvmprint("type 0x%x\n", type);
    
	if(TVM_ENABLE == enable)
	{
		igmp_tvm_status_set(TVM_ENABLE);
		switch(type)
		{
			case GwdTvmMcastTabelADD:
				oam_igmp_relation_tabel_add(message_input);
				break;
			case GwdTvmMcastTabelDelete_IpIdx:
				oam_igmp_relation_tabel_ip_del(message_input);
				break;
			case GwdTvmMcastStatusSync:
				oam_igmp_relation_tabel_synchronism(message_input);
				break;
			case GwdTvmMcastTabelDelete_VlanIdx:
				oam_igmp_relation_tabel_vlan_del(message_input);
				break;
			default:
				break;
		}
	}
	else
	{
		igmp_tvm_status_set(TVM_DISABLE);
	}
	return 0;
}


/**********************************************************************************************************************************************
*函数名：GwOamTvmRequestRecv
*函数功能描述： ONU接收跨vlan组播OAM的入口
*函数返回值：long	:VOS_ERROR-失败，VOS_OK-成功

**********************************************************************************************************************************************/
extern long GwOamTvmRequestRecv(GWTT_OAM_MESSAGE_NODE *pRequest)
{
    if(pRequest == NULL)
        return GW_ERROR;
	gwdtvmprint("\n****in GwOamTvmRequestRecv***\n");
    
	if(NULL == pRequest)
	{
		return GWD_RETURN_ERR;
	}
	else
	{
		//do nothing
	}
	
	if(NULL == pRequest->pPayLoad)
	{
		return GWD_RETURN_ERR;
	}
	else
	{
		//do nothing
	}

	oam_through_vlan_igmp_proc(pRequest);

	return GWD_RETURN_OK;


}
/**********************************************************************************************************************************************
*函数名：igmp_relation_tabel_show
*函数功能描述：打印组播关系表
*函数参数：
*函数返回值：int :0-成功，1-失败
**********************************************************************************************************************************************/

int cmd_show_igmp_snooping_tvm(struct cli_def *cli, char *command, char *argv[], int argc)
{
    gw_int32 tvm_status = TVM_DISABLE;
    gw_int32 ret = GW_ERROR;
    gw_int32 ret =0;

	gw_uint16 num = 0;
    TVM_Cont_Head_t *TVM_Cont_Head_Info = NULL;
    if(CLI_HELP_REQUESTED)
	{
        switch(argc)
        {
            default:
                return gw_cli_arg_help(cli, argc > 0, NULL);
        }
    }
    
    ret = call_gwdonu_if_api(LIB_IF_TVM_STATUS_GET,1,&tvm_status);
    if(ret != GW_OK)
        return ret;
    
    if(TVM_DISABLE == &tvm_status)
	{
		gw_cli_print(cli,"igmp-tvm disabled \n");
		return 0;
	}
	else
	{
		//do nothing
	}
    
    ret = call_gwdonu_if_api(LIB_IF_TVM_RELATION_TABEL_GET,1,TVM_Cont_Head_Info);
    if(ret != GW_OK)
        return ret;
    
	Through_Vlan_Group_t *head = TVM_Cont_Head_Info->Through_Vlan_Group_head;

	num = TVM_Cont_Head_Info->TVMCOUNT;
	gw_cli_print(cli,"num :%u\n", num);

	if(NULL == head)
	{
		gw_cli_print(cli,"igmp_relation_tabel is NULL\n");
	}
	else
	{
		int i = 0;
		Through_Vlan_Group_t *front = NULL;
		front = head;
		gw_cli_print(cli,"NO.	GroupStart	GroupEnd	IVid\n");
		gw_cli_print(cli,"------------------------------------------------\n");
		for(front=head; front!=NULL; front = front ->next)
		{
			i++;
			gw_cli_print(cli,"%-4d 	0x%x 	0x%x 	0x%x\n", i, front->GroupSt, front->GroupEnd, front->IVid);
		}		
	}
	
	return ret;
}
static gw_uint32 inet_addr_abl(char *cp)
{
	gw_uint32 ip = 0;
	char *p[5] = {NULL};
	int i = 0;
	p[i++] = strtok(cp, ".");
	while((p[i++] = strtok(NULL, ".")))
	{
		//do nothing
	}
	ip = (atoi(p[0])<<24) + (atoi(p[1])<<16) +(atoi(p[2])<<8) +(atoi(p[3])<<0);
	return ip;
}
int cmd_igmp_snooping_tvm(struct cli_def *cli, char *command, char *argv[], int argc)
{
	gw_cli_print(cli, "in cmd_igmp_snooping_tvm\n");

	if(CLI_HELP_REQUESTED)
	{
        switch(argc)
        {
        case 1:
            	gw_cli_arg_help(cli, 0,
                "enable|disable", "through vlan igmp enable|disable",
                 NULL);
				gw_cli_arg_help(cli, 0,
                "add", "<h.h.h.h> <h.h.h.h> <1-4094>",
                 NULL);
				gw_cli_arg_help(cli, 0,
                "del", "<h.h.h.h> <h.h.h.h>",
                 NULL);
			return 	gw_cli_arg_help(cli, 0,
                "del", "all",
                 NULL);
		case 2:
				gw_cli_arg_help(cli, 0,
                "add", "<h.h.h.h> <h.h.h.h> <1-4094>",
                 NULL);
				gw_cli_arg_help(cli, 0,
                "del", "<h.h.h.h> <h.h.h.h>",
                 NULL);
			return gw_cli_arg_help(cli, 0,
                "del", "all",
                 NULL);
		case 3:
			return gw_cli_arg_help(cli, 0,
                "add", "<h.h.h.h> <h.h.h.h> <1-4094>",
                 NULL);
        default:
            return gw_cli_arg_help(cli, argc > 2, NULL);
        }		
	}
	else
	{
		//do nothing
	}
	if(1 == argc)
	{
		if(0 == strcmp("enable",argv[0]))
		{
			igmp_tvm_status_set(1);
		}
		else if(0 == strcmp("disable", argv[0]))
		{
			igmp_tvm_status_set(0);
		}
		else
		{
			gw_cli_print(cli, "wrong input,please input again\n");
		}
	}
	else if(2 == argc)
	{
		if((0 == strcmp("del", argv[0]))&&(0 == strcmp("all", argv[1])))
		{
			call_gwdonu_if_api(LIB_IF_TVM_RELATION_TABEL_CLEAR,0,NULL);
		}
		else
		{
			gw_cli_print(cli, "wrong input,please input again\n");
		}
	}
	else if(3 == argc)
	{
		if(0 == strcmp("del", argv[0]))
		{
			gw_uint32 GroupSt = 0;
			gw_uint32 GroupEnd = 0;
			//转化ip
			igmp_relation_tabel_ip_del(GroupSt, GroupEnd);
		}
		else
		{
			gw_cli_print(cli, "wrong input,please input again\n");
		}
	}
	else if(4 == argc)
	{
		if(0 == strcmp("add", argv[0]))
		{
			gw_uint32 GroupSt = inet_addr_abl(argv[1]);
			gw_uint32 GroupEnd = inet_addr_abl(argv[2]);
			gw_uint16 IVid = atoi(argv[3]);
            #if __tvm_debug__
			igmp_relation_tabel_add(GroupSt, GroupEnd, IVid);
            #endif
            call_gwdonu_if_api(LIB_IF_TVM_RELATION_TABEL_ITEM_ADD, 3,GroupSt, GroupEnd, IVid);
		}
		else
		{
			gw_cli_print(cli, "wrong input,please input again\n");
		}
	}
	else
	{
		gw_cli_print(cli, "wrong input,please input again\n");
	}
	return CLI_OK;
}
#endif
