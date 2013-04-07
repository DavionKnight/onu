
/*
 *
 * gw_types.h
 *
 * Include file containing the basic data types
 *
 *
 */

#ifndef _GW_TYPES_H_
#define _GW_TYPES_H_

#include "gw_os_config.h"

typedef unsigned long long      gw_uint64;
typedef long long               gw_int64;
typedef unsigned int            gw_uint32;
typedef int                     gw_int32;
typedef unsigned short          gw_uint16;
typedef short                   gw_int16;
typedef unsigned char           gw_uint8;
typedef char                    gw_int8;
typedef unsigned char           gw_boolean;
typedef float					gw_float;

typedef enum {
    GW_PON_PORT_ID     = 0x00,
    GW_UNI_PORT_ID1    = 0x01,
    GW_UNI_PORT_ID2    = 0x02,
    GW_UNI_PORT_ID3    = 0x03,
    GW_UNI_PORT_ID4    = 0x04,
    GW_DOWNLINK_PORT   = 0x20,
    GW_UPLINK_PORT     = 0x30,
    GW_MGMT_PORT_ID    = 0x4F,
    GW_VOIP_PORT_ID    = 0x50,
    GW_ALL_UNI_PORT_ID = 0xFF,
} gw_port_id_t;

typedef enum {
    GW_DOWN_STREAM     = 0x00,
    GW_UP_STREAM       = 0x01,
} gw_direction_t;


typedef enum {
    GW_E_OK               = 0,
    GW_E_RESOURCE         = 1,
    GW_E_PARAM            = 2,
    GW_E_NOT_FOUND        = 3,
    GW_E_CONFLICT         = 4,
    GW_E_TIMEOUT          = 5,
    GW_E_NOT_SUPPORT      = 6,
    GW_E_ERROR            = 0xffffffff
} gw_status;
typedef enum {
        GW_PORT_ADMIN_DOWN,
        GW_PORT_ADMIN_UP,
} gw_port_admin_status_t;
typedef struct{
	gw_uint64 gulBcStormThreshold;
	gw_uint64 gulBcStormStat;
}broadcast_storm_s;
broadcast_storm_s broad_storm;

typedef int (*gw_funcptr)(void *arg);

typedef gw_uint16                gw_ipv6_t[8];

enum {
    GW_IPV4,
    GW_IPV6
};

typedef struct {
    gw_uint32   ipver;
    union {
        gw_uint32 ipv4;
        gw_ipv6_t ipv6;
    } addr;
} gw_ip_t;

typedef gw_uint16                gw_dev_id_t;
typedef gw_uint16                gw_llid_t;
typedef struct {
    gw_uint32    bmp[4];
}                                gw_llid_bmp_t;

typedef gw_uint16                gw_sub_id_t;


#define GW_MACADDR_LEN          6

typedef union {
    gw_uint8       addr[GW_MACADDR_LEN];
}__attribute__((packed)) gw_mac_t;


#define GW_OK               GW_E_OK
#define GW_ERROR            GW_E_ERROR

#ifndef TRUE
#define TRUE                       1
#endif

#ifndef FALSE
#define FALSE                  0
#endif

#ifndef NULL
#define NULL                   ((void *) 0)
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef GW_IN
#define GW_IN
#endif

#ifndef GW_OUT
#define GW_OUT
#endif

#ifndef GW_IN_OUT
#define GW_IN_OUT
#endif

#define _GW_INLINE_ __inline__ static

#ifdef __BIG_ENDIAN__

#ifndef ntohl
#define ntohl(x)        (x)
#endif

#ifndef ntohs
#define ntohs(x)        (x)
#endif

#ifndef htonl
#define htonl(x)        (x)
#endif

#ifndef htons
#define htons(x)        (x)
#endif

#ifndef ntohll
#define ntohll(x)       (x)
#endif


#ifndef htonll
#define htonll(x)       (x)
#endif

#elif defined(__LITTLE_ENDIAN__)

#ifndef ntohl
#define ntohl(x)        ((((x) & 0x000000ff) << 24) | \
                            (((x) & 0x0000ff00) <<  8) | \
                            (((x) & 0x00ff0000) >>  8) | \
                            (((x) & 0xff000000) >> 24))
#endif

#ifndef htonl
#define htonl(x)        ((((unsigned long)(x) & 0x000000ff) << 24) | \
                            (((unsigned long)(x) & 0x0000ff00) <<  8) | \
                            (((unsigned long)(x) & 0x00ff0000) >>  8) | \
                            (((unsigned long)(x) & 0xff000000) >> 24))
#endif


#ifndef ntohs
#define ntohs(x)        ((((x) & 0x00ff) << 8) | \
                            (((x) & 0xff00) >> 8))
#endif

#ifndef htons
#define htons(x)        ((((x) & 0x00ff) << 8) | \
                            (((x) & 0xff00) >> 8))

#endif


#ifndef ntohll
#define ntohll(x)        ((((gw_uint64)ntohl(x)) << 32) | \
                                       ntohl((x) >> 32))
#endif


#ifndef htonll
#define htonll(x)        ((((gw_uint64)htonl(x)) << 32) | \
                                 htonl((x) >> 32))
#endif

#else
#error Endianness not defined
#endif

#ifndef gwd_ntohl
#define gwd_ntohl(x)        ((((x) & 0x000000ff) << 24) | \
                            (((x) & 0x0000ff00) <<  8) | \
                            (((x) & 0x00ff0000) >>  8) | \
                            (((x) & 0xff000000) >> 24))
#endif

#ifndef gwd_htonl
#define gwd_htonl(x)        ((((unsigned long)(x) & 0x000000ff) << 24) | \
                            (((unsigned long)(x) & 0x0000ff00) <<  8) | \
                            (((unsigned long)(x) & 0x00ff0000) >>  8) | \
                            (((unsigned long)(x) & 0xff000000) >> 24))
#endif


#ifndef gwd_ntohs
#define gwd_ntohs(x)        ((((x) & 0x00ff) << 8) | \
                            (((x) & 0xff00) >> 8))
#endif

#ifndef gwd_htons
#define gwd_htons(x)        ((((x) & 0x00ff) << 8) | \
                            (((x) & 0xff00) >> 8))

#endif


#ifndef gwd_ntohll
#define gwd_ntohll(x)        ((((gw_uint64)ntohl(x)) << 32) | \
                                       ntohl((x) >> 32))
#endif


#ifndef gwd_htonll
#define gwd_htonll(x)        ((((gw_uint64)htonl(x)) << 32) | \
                                 htonl((x) >> 32))
#endif



/* range: should assume only two values: 0 or 1 */
typedef unsigned int                    gw_mask_t;
typedef unsigned char                   gw_macaddr_t[GW_MACADDR_LEN];
typedef unsigned char                   *gw_macaddr_pt;

typedef gw_macaddr_t                  gw_onu_device_id_t;
typedef gw_uint32                   gw_olt_device_id_t;

typedef gw_uint32                   gw_onu_version_t;
typedef gw_uint32                   gw_olt_version_t;


#define INVALID_PORT_ID 0xffffffff
#define ONU_LLIDPORT_FOR_API   (-1)
#define ONU_DEVICEID_FOR_API   (-1)

#define GW_MAX_ONU_PON_PORTS          1
#define GW_MAX_ONU_LLID_PORTS         (GW_MAX_ONU_PON_PORTS * GW_MAX_LLIDS_PER_ONU_PON_PORT)

/* number of LLIDs assigned on an ONU PON port */
#define GW_MAX_LLIDS_PER_ONU_PON_PORT 1

#define USER_NAME_LEN               32
#define PASSWORD_LEN                32


/* asynchronous API result code */
typedef enum {
    GW_RESULT_SUCCESS = 0,
    GW_RESULT_FAIL,
    GW_RESULT_NO_REQ_RECORD,
    GW_RESULT_NO_OLT_MAC,
    GW_RESULT_NO_MEMORY,
    GW_RESULT_WRONG_EVENT,
    GW_RESULT_NULL_HANDLER,
    GW_RESULT_REQ_TIMEOUT,
    GW_RESULT_NO_ONU_MAC,
    GW_RESULT_PING_TOO_LONG,
    GW_RESULT_CONTROL_IF_DOWN,
    GW_RESULT_INV_PARAM,
    GW_RESULT_INV_CHIP_VERSION,
    GW_RESULT_DEVICE_NOT_SUPPORTED
} gw_result_code_t;


typedef enum {
    GW_RETURN_SUCCESS = 0,
    GW_RETURN_FAIL,
    GW_RETURN_REINIT,
    GW_RETURN_INV_CHIP_VERSION,
    GW_RETURN_INV_STATE,
    GW_RETURN_INV_MAC,
    GW_RETURN_INV_PARAM,
    GW_RETURN_NOT_SUPPORT,
    GW_RETURN_NOT_FOUND,
    GW_RETURN_NO_RESOURCE
} gw_return_code_t;


/*default CPU pkt priority */
#define ONU_CPU_PKT_PRIO        7
#define ONU_CPU_PKT_PRIO_6      6

/* boolean values */
#define GW_TRUE                       1
#define GW_FALSE                      0

#endif /* _GW_TYPES_H_ */
