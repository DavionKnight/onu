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

#define	_LITTLE_ENDIAN_		RPU_NO
#define	_BIG_ENDIAN_		    RPU_YES

/***************************************************************************/

/*
 * os type enum
 * using micro _OS_ to select os type
 */
#define OS_TYPE_CYG_LINUX 1
#define OS_TYPE_LINUX 2
#define OS_TYPE_VXWORKS 3
#define OS_TYPE_UNKNOWN 4

/*
 * socket api class select
 * using micro _SOCKET_CLASS_ to select socket type
 */
#define BSD_SOCKET 1
#define LWIP_SOCKET 2

/*functions*/
#define RPU_MODULE_PPPOE_RELAY 			RPU_NO
#define RPU_MODULE_RCP_SWITCH			RPU_NO
#define RPU_MODULE_LOOPBACK_DETECT		RPU_NO
#define RPU_MODULE_S2E					RPU_NO
#define RPU_MODULE_POE					RPU_NO
#define RPU_MODULE_USER_LOCATE			RPU_NO


/*device info*/
#define NUMBER_PORTS_PER_SYSTEM			32

/*system type define*/
#define _OS_ OS_TYPE_LINUX

/*min stack size of threads*/
#define GW_THREAD_STACK_MIN_SIZE (4*1024)

/*socket class select*/
#define _SOCKET_CLASS_ BSD_SOCKET

#define OS_CYG_LINUX (_OS_ == OS_TYPE_CYG_LINUX)
#define OS_LINUX (_OS_ == OS_TYPE_LINUX)
#define OS_VXWORKS (_OS_ == OS_TYPE_VXWORKS)

#define USING_BSD_SOCK (_SOCKET_CLASS_ == BSD_SOCKET)
#define USING_LWIP_SOCK (_SOCKET_CLASS_ == LWIP_SOCKET)


#endif

