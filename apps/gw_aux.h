#ifndef __GW_AUX__
#define __GW_AUX__

typedef union{
	struct ifmac_s
    {
#if 1
		unsigned multag:1;
        unsigned mulother:7;
#else
		unsigned multag:7;
        unsigned mulother:1;
#endif
    }if_mulimac_t;
    unsigned char mulimac_byte;
}GWD_ONU_IF_MULIMAC_U;

int onu_mulimac_cheak(unsigned char *fdbmac);
#endif

