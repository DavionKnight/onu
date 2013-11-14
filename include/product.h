/*
 * product.h
 *
 *  Created on: Oct 21, 2013
 *      Author: gwtt
 */

#ifndef PRODUCT_H_
#define PRODUCT_H_

#define GT811C						0x01
#define GT811D						0x02
#define GT813C						0x03/*include GT813PB*/
#define GT815C						0x04/*include GT815PB*/
#define GT816A						0x05/*no gt810a any more*/
#define GT873M_1F2S					0x06
#define GT873M_4F4S					0x07
#define GT873A						0x08

#define FOR_BCM_ONU  ((PRODUCT_CLASS == GT813C)||(PRODUCT_CLASS == GT815C))
#define FOR_CORTINA_ONU ((PRODUCT_CLASS == GT811C)||(PRODUCT_CLASS == GT811D)||(PRODUCT_CLASS == GT873M_1F2S)||(PRODUCT_CLASS == GT873M_4F4S)||(PRODUCT_CLASS == GT816A))

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

#define DEVICE_TYPE_VALID_MAX		DEVICE_TYPE_GT873_A
#define DEVICE_TYPE_VALID_MIN		DEVICE_TYPE_GT811
//#define PRODUCT_TYPE                DEVICE_TYPE_GT813_C
#define DeviceTypeIsValid( _device_type ) \
    ((_device_type)>=DEVICE_TYPE_VALID_MIN && (_device_type)<=DEVICE_TYPE_VALID_MAX)


#endif /* PRODUCT_H_ */
