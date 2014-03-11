#ifndef __MGT_IF__
#define __MGT_IF__

#include "gw_types.h"
#include "gwdonuif.h"

#define DEFAULT_IFCONFIG_ENABLE 1
#define DEFAULT_IFCONFIG_DISABLE 0

typedef struct inetconfig_tlv_s{
    gw_uint32 Defaultflag;
    gw_uint32 port;
    GwdUMnGlobalParameter inetconfig;
}inetconfig_tlv_t;

#endif

