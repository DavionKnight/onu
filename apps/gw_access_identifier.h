/*
 * gw_access_identifier.h
 *
 *  Created on: Sep 17, 2014
 *      Author: wangzhw
 */

#ifndef GW_ACCESS_IDENTIFIER_H_
#define GW_ACCESS_IDENTIFIER_H_
#define DHCPDATALEN 300
#define ETH_IPV4_MAC_LEN 6

#define ETH_TYPE_IP 0X0800
#define DHCP_PKT_FLAG 1
#define DHCP_MALLOC_RESPONSE_LEN 1600
#define UdpProtocolType 0x0011  // UDP -IP protocol # 17

#define DhcpSvrPortNum  0x0043      // UDP -DHCP Server Port #67
#define DhcpCliPortNum  0x0044      // UDP -DHCP Client Port #68

#define DhcpV6CliPortNum  0x0222    // UDP -DHCPv6 Client Port #546
#define DhcpV6SvrPortNum  0x0223    // UDP -DHCPv6 Server Port #547

#define DHCP_PKT_DEF_LEN 312
#define IpV4Version      4
#define IpV6Version      6
#define EtherHeadLen 14
#define IPHEADERLEN 20
#define UDPHEADERLEN 8
#define ETHFCSLEN 4
#define Option82HeadLen 2
#define SubOptionHeadLen 2
#define EndOpion 0xff
#define Padding 0x00

typedef unsigned long IpAddr;
enum{
	DHCP_OPTION_REQ = 1,
	DHCP_OPTION_RESPONSE =2,
};
enum{
	PPPOE_IDENTIFIER_MODE = 1,
	DHCP_IDENTIFIER_MODE=2
};
enum{
	DHCP_RELAY_DISABLE =0,
	DHCP_RELAY_CTC_MODE=1,
	DHCP_RELAY_GWD_MODE=2,
};
enum{
	ACCESS_IDENT_SET_SUCCUSS = 1,
	ACCESS_IDENT_SET_FAIL =2,
};
enum{
	DHCP_ROUTER_IP_TYPE=3,
	DHCP_DNS_IP_TYPE=6,
	DHCP_WINS_IP_TYPE=44,
	DHCP_MESSAGE_TYPE=53,
	DHCP_SERVER_ID_TYPE=54,
	DHCP_CLIENT_ID_TYPE=61,
	DHCP_OPTION82_TYPE =82,
	DHCP_OPTION_END =255,
};
enum{
	DHCP_SUB_OPTION82_1 =1,
	DHCP_SUB_OPTION82_2 =2,
	DHCP_SUB_OPTION82_3 =3,
};
enum{
	DHCPDISCOVER = 0x01,
	DHCPOFFER = 0x02,
	DHCPREQUEST = 0x03,
	DHCPDECLINE =0x04,
	DHCPACK =0x05,
	DHCPNAK =0x06,
	DHCPRELEASE =0x07,
	DHCPINFORM = 0x08,
};
typedef struct tsd_header_s{
	IpAddr sourceip;
	IpAddr destip;
	unsigned char mzero;
	unsigned char ptcl;
	unsigned short udplen;
}tsd_header_t;
typedef struct access_identifier_admin_s
{
	unsigned char opcode;
	unsigned char returnstatus;
	unsigned char identifiermode;
	unsigned char proxymode;
}access_identifier_admin_t;
typedef struct dhcp_option82_data_info_s
{
	unsigned int datalen;
	unsigned char dhcprelayidentifierinfo[DHCPDATALEN];
}dhcp_option82_data_info_t;
typedef struct eth_head_info_s{
	unsigned char dstmac[ETH_IPV4_MAC_LEN];
	unsigned char srcmac[ETH_IPV4_MAC_LEN];
	unsigned short flag;
	unsigned short cvlan;
	unsigned short ethtype;
}eth_head_info_t;
typedef struct eth_iphead_info_s{
    unsigned char  	 VerHeadLen;                    //版本号和头长度
    unsigned char      TOS;                        //服务类型
    unsigned short     Totlength;                //IP包总长度
    unsigned short     ID;                            //IP包唯一标识
    unsigned short     Flags;                        //标志
    unsigned char      TTL;                        //生存时间
    unsigned char      Protocol;                    //协议
    unsigned short     Checksum;                    //校验和
    IpAddr               SourceIP;                    //源IP地址
    IpAddr               DestIP;                        //目标IP
}eth_iphead_info_t;

typedef struct udp_head_info_s{
	unsigned short srcUdpPort;
	unsigned short destUdpPort;
	unsigned short	 length;
	unsigned short checkSum;
}udp_head_info_t;

typedef struct gwd_dhcp_pkt_head_info_s{
	eth_head_info_t ethhead;
	eth_iphead_info_t iphead;
	udp_head_info_t udphead;
	unsigned char op;
	unsigned char Htype;
	unsigned char Hlen;
	unsigned char hops;
	unsigned char xip[4]; /*事件序列*/
	unsigned short second;
	unsigned short flag;
	IpAddr ciaddr; /*客户机地址*/
	IpAddr yiaddr; /*你的IP*/
	IpAddr siaddr; /*服务器IP*/
	IpAddr giaddr; /*中继代理地址*/
	unsigned char chaddr[16]; /*客户机硬件地址*/
	unsigned char sname[64]; /*服务主机名*/
	unsigned char filename[128]; /*启动文件名*/
	unsigned char magic[4];
}gwd_dhcp_pkt_info_head_t;
typedef struct dhcp_option82_s{
	unsigned char option_code;
	unsigned char option_len;
}dhcp_option82_t;
typedef struct dhcpOption82_ctc_str_s{
	dhcp_option82_t option82;
	struct  circuit_id_ctc_str
	{
		unsigned char subOp;
		unsigned char len;
		unsigned char str_info[100];
	} cir_id;
} dhcpOption82_ctc_str_t;

extern unsigned int Gwd_Func_Dhcp_relay_Oam_Admin_Process(unsigned char *pReq,unsigned int len);
#endif /* GW_ACCESS_IDENTIFIER_H_ */
