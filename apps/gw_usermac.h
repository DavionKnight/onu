#ifndef __GW_USERMAC__
#define __GW_USERMAC__
#define NOMAC 1
#define HAVEMAC 0
extern RCP_DEV *rcpDevList[MAX_RRCP_SWITCH_TO_MANAGE];
extern int RCP_Dev_Is_Exist(unsigned long parentPort);
int locateUserMac(char * mac, localMacsave_t *macbuf,int macnumberget,int * onuslot, int * onuport, unsigned char * subsw, char *sw_mac, int * sw_port);	
#endif

