/*
 * superset.h
 *
 *  Created on: Oct 21, 2013
 *      Author: gwtt
 */
/* 定义产品类别 */
#ifndef SUPERSET_H_
#define SUPERSET_H_

#if (PRODUCT_CLASS == GT813C)
#ifdef NUMBER_PORTS_PER_SYSTEM
#undef NUMBER_PORTS_PER_SYSTEM
#define NUMBER_PORTS_PER_SYSTEM 24
#endif
#elif (PRODUCT_CLASS == GT815C)
#else
#define RPU_MODULE_LOOPBACK_DETECT 		RPU_NO
#define RPU_MODULE_RCP_SWITCH					RPU_NO
#define RPU_MODULE_IGMP_TVM						RPU_NO
#define PRODUCT_TYPE			(DEVICE_TYPE_VALID_MAX+1)
#endif


#endif /* SUPERSET_H_ */
