#ifndef __GW_CONFIG__
#define __GW_CONFIG__

#include "product.h"

///device type declared list
#define DEVICE_TYPE_GT821			0x0005	/* GT821 */
#define DEVICE_TYPE_GT831			0x0006	/* GT831 */
#define DEVICE_TYPE_GT813			0x0008	/* GT813 */
#define DEVICE_TYPE_GT812			0x0007/*7*/	/* GT812 */
#define DEVICE_TYPE_GT811			0x0004/*11 - a*/ /*4-*/	/* GT811 */
#define DEVICE_TYPE_GT810			0x000c	/* GT810 */
#define DEVICE_TYPE_GT816			0x0010	/* GT816 */
#define DEVICE_TYPE_GT821_A			0x0013	/* GT821A */
#define DEVICE_TYPE_GT831_A			0x0014	/* GT831A */
#define DEVICE_TYPE_GT812_A			0x0012/*7*/	/* GT812 */
#define DEVICE_TYPE_GT811_A			0x0011/*11 - a*/ /*4-*/	/* GT811 */
#define DEVICE_TYPE_GT865			0x000F	/* GT865 */
#define DEVICE_TYPE_GT861			0x000A	/* GT861 */
#define DEVICE_TYPE_GT815			0x0015	/* GT815 */
#define DEVICE_TYPE_UNKNOWN			0x0001	/* unknown */
#define DEVICE_TYPE_GT812PB			0x0016	/* GT812PB */
#define DEVICE_TYPE_GT831_B			0x0017	/* GT831B */
#define DEVICE_TYPE_GT866			0x0018	/* GT866 */
#define DEVICE_TYPE_GT811_B			0x0019	/* GT811_B */
#define DEVICE_TYPE_GT851			0x001A	/* GT851 */
#define DEVICE_TYPE_GT813_B			0x001B	/* GT813_B */
#define DEVICE_TYPE_GT862			0x001C	/* GT862 */
#define DEVICE_TYPE_GT892			0x001D	/* GT892 */
#define DEVICE_TYPE_GT835			0x001E	/* GT835 */
#define DEVICE_TYPE_GT831_B_CATV	0x001F	/* GT831_B_CATV */
#define DEVICE_TYPE_GT815_B			0x0020	/* GT815_B */

#define DEVICE_TYPE_GT863			0x000D	/* GT863 */ //added by wangxiaoyu 2009-05-25
#define DEVICE_TYPE_GT871B			0x0021	/* GT871 */ //added by dushaobo 2009-07-13
#define DEVICE_TYPE_GT871R                    0x0022
#define DEVICE_TYPE_GT872                       0x0025
#define DEVICE_TYPE_GT873                       0x0028
#define DEVICE_TYPE_GT870                       0x002C
#define DEVICE_TYPE_GT813_C 			0x002D
#define DEVICE_TYPE_GT815_C			0x002e
#define DEVICE_TYPE_GT812_C			0x002f
#define DEVICE_TYPE_GT811_C			0x0030
#define DEVICE_TYPE_GT873_A			0x0031
#define DEVICE_TYPE_GT810_A			0x0032
#define DEVICE_TYPE_VALID_MAX		DEVICE_TYPE_GT810_A
#define DEVICE_TYPE_VALID_MIN		DEVICE_TYPE_GT811
//#define PRODUCT_TYPE                DEVICE_TYPE_GT813_C
#define DeviceTypeIsValid( _device_type ) \
    ((_device_type)>=DEVICE_TYPE_VALID_MIN && (_device_type)<=DEVICE_TYPE_VALID_MAX)

/*----------------------------------------------------------------------*/
/*
 * ���������궨���ˡ������͡��ء�����״̬���������걻�����
 * �����á����磺
 *
 *	#define	RPU_MODULE_RTPRO_OSPF	RPU_NO
 *	#define	RPU_MODULE_RTPRO_RIP	RPU_YES
 *
 * ��ʾ��̬·��Э��OSPF���رգ���̬·��Э��RIP����������
 * ��ϵͳ�ڱ���ʱ���Ϳ��Բ��OSPFģ�����RIPģ�顣
 * ��Ҫע�������Щ�����ϻ����һ�Ժ꣬���ܱ�ͬʱ��Ϊ
 * ��RPU_YES����RPU_NO������һ�㽫�����ں���������С�
 * ע�⣬�벻Ҫ�޸����������ֵ��
 */
#define	RPU_YES		1
#define	RPU_NO		0

/*
 * RPU MODULE����
 * RPU_MODULE_LOOPBACK_DETECT
 * RPU_MODULE_RCP_SWITCH
 *
 */

/***************************************************************************/
#define RPU_MODULE_PPPOE_RELAY RPU_YES

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

#if ( PRODUCT_GT813C || PRODUCT_GT815C)

#define _OS_ OS_TYPE_LINUX
#define _SOCKET_CLASS_	BSD_SOCKET

#if(PRODUCT_GT813C)
#define PRODUCT_TYPE                DEVICE_TYPE_GT813_C
#endif
#if(PRODUCT_GT815C)
#define PRODUCT_TYPE                DEVICE_TYPE_GT815_C
#endif

#define RPU_MODULE_LOOPBACK_DETECT 		RPU_YES
#define RPU_MODULE_RCP_SWITCH					RPU_YES
#define RPU_MODULE_IGMP_TVM						RPU_NO

#define GW_THREAD_STACK_MIN_SIZE			(40*1024)

#define __BIG_ENDIAN__

#elif defined (PRODUCT_GT811C)
#define RPU_MODULE_LOOPBACK_DETECT 		RPU_YES
#define RPU_MODULE_RCP_SWITCH					RPU_YES
#define RPU_MODULE_IGMP_TVM						RPU_NO
#define PRODUCT_TYPE                DEVICE_TYPE_GT811_C

#define GW_THREAD_STACK_MIN_SIZE			(4*1024)

#define __LITTLE_ENDIAN__

#elif defined  PRODUCT_GT811D
#define RPU_MODULE_LOOPBACK_DETECT 		RPU_YES
#define RPU_MODULE_RCP_SWITCH					RPU_YES
#define RPU_MODULE_IGMP_TVM						RPU_NO
#define PRODUCT_TYPE                DEVICE_TYPE_GT811_D

#define GW_THREAD_STACK_MIN_SIZE			(4*1024)

#define __LITTLE_ENDIAN__

#elif defined PRODUCT_GT811G
#define RPU_MODULE_LOOPBACK_DETECT 		RPU_YES
#define RPU_MODULE_RCP_SWITCH					RPU_YES
#define RPU_MODULE_IGMP_TVM						RPU_NO
#define PRODUCT_TYPE                DEVICE_TYPE_GT811_G

#define GW_THREAD_STACK_MIN_SIZE			(4*1024)

#define __LITTLE_ENDIAN__

#elif defined PRODUCT_GT810A
#define _OS_ OS_TYPE_CYG_LINUX
#define _SOCKET_CLASS_	LWIP_SOCKET
#define RPU_MODULE_LOOPBACK_DETECT 		RPU_YES
#define RPU_MODULE_RCP_SWITCH					RPU_YES
#define RPU_MODULE_IGMP_TVM						RPU_NO
#define PRODUCT_TYPE                DEVICE_TYPE_GT810_A

#define GW_THREAD_STACK_MIN_SIZE			(4*1024)

#define __LITTLE_ENDIAN__

#elif defined  PRODUCT_GT816A
#define RPU_MODULE_LOOPBACK_DETECT 		RPU_YES
#define RPU_MODULE_RCP_SWITCH					RPU_YES
#define RPU_MODULE_IGMP_TVM						RPU_NO
#define PRODUCT_TYPE                DEVICE_TYPE_GT816_A

#define GW_THREAD_STACK_MIN_SIZE			(4*1024)

#define __LITTLE_ENDIAN__

#elif defined  PRODUCT_GT873M
#define RPU_MODULE_LOOPBACK_DETECT 		RPU_YES
#define RPU_MODULE_RCP_SWITCH					RPU_YES
#define RPU_MODULE_IGMP_TVM						RPU_NO
#define PRODUCT_TYPE                DEVICE_TYPE_GT873_M

#define GW_THREAD_STACK_MIN_SIZE			(4*1024)

#define __LITTLE_ENDIAN__
#else
#define RPU_MODULE_LOOPBACK_DETECT 		RPU_NO
#define RPU_MODULE_RCP_SWITCH					RPU_NO
#define RPU_MODULE_IGMP_TVM						RPU_NO
#define PRODUCT_TYPE			(DEVICE_TYPE_VALID_MAX+1)

#define GW_THREAD_STACK_MIN_SIZE			(40*1024)

#define __BIG_ENDIAN__
#endif

#define OS_CYG_LINUX (_OS_ == OS_TYPE_CYG_LINUX)
#define OS_LINUX (_OS_ == OS_TYPE_LINUX)
#define OS_VXWORKS (_OS_ == OS_TYPE_VXWORKS)

#define USING_BSD_SOCK (_SOCKET_CLASS_ == BSD_SOCKET)
#define USING_LWIP_SOCK (_SOCKET_CLASS_ == LWIP_SOCKET)

#define USERMAC_EN 0x1 

#endif

