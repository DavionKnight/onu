#ifndef __GW_CONFIG__
#define __GW_CONFIG__

/*----------------------------------------------------------------------*/
/*
 * 下面两个宏定义了“开”和“关”两个状态，这两个宏被后面的
 * 宏引用。例如：
 *
 *	#define	RPU_MODULE_RTPRO_OSPF	RPU_NO
 *	#define	RPU_MODULE_RTPRO_RIP	RPU_YES
 *
 * 表示动态路由协议OSPF被关闭，而动态路由协议RIP被开启，这
 * 样，系统在编译时，就可以拆掉OSPF模块而保留RIP模块。
 * 需要注意的是有些意义上互斥的一对宏，不能被同时置为
 * “RPU_YES”或“RPU_NO”，这一点将体现在后面的叙述中。
 * 注意，请不要修改这两个宏的值。
 */
#define	RPU_YES		1
#define	RPU_NO		0

/***************************************************************************/
#define RPU_MODULE_USER_MAC_RELAY RPU_YES
#define RPU_MODULE_IGMP_TVM RPU_YES
#define RPU_MODULE_POE RPU_YES


#define USERMAC_EN 0x1 

#endif

