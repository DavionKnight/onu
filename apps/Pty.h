#ifndef __PTY_H__
#define __PTY_H__

#define MASTER_WRITE 0
#define SLAVE_WRITE 1
#define MASTER_READ 0
#define SLAVE_READ 1

int CreatePty(char *name);
int OpenMasterDev(int pty_num);
int OpenSlaveDev(int pty_num);
int pty_write(int fd,char *str,int length);
int pty_read(int fd,char *str,int length);
void CloseMasterDev(int fd);
void CloseSlaveDev(int fd);
void DeletePty(int pty_num);

#endif



                 
