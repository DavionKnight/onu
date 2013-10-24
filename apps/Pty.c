
#include "../include/gw_config.h"

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#if USING_BSD_SOCK
#include <sys/socket.h>

#include <netinet/in.h>
#else
#include <lwip/sockets.h>
#endif

#include "Pty.h"

#include "gw_log.h"

#if OS_CYG_LINUX
#define OAM_VCONPTY_DEBUG(x) gw_log(GW_LOG_LEVEL_DEBUG, (x))
#else
#define OAM_VCONPTY_DEBUG(x) printf x
#endif

#define NAME_LEN 8

#if _USE_PIPE_PTY

typedef struct pty
{
	char name[NAME_LEN];
	int pty_num;
	int master_read;
	int master_write;
	int slave_read;
	int slave_write;
	struct pty *next;
}Pty;

Pty *head_pty = NULL;
int pty_maxnum = 0;

/*create a new pty added to the existing link */
/*failed return -1                            */
/*max number of pty is 8*/
int CreatePty(char *name)
{
	int name_length = 0;	
	Pty *p1,*p2;

	if(NULL == head_pty)
	{
		if(NULL == (head_pty = (Pty*)malloc(sizeof(Pty))))
		{
			gw_log(GW_LOG_LEVEL_DEBUG,("create pty device %s fail!\r\n", name));
			return -1;
		}
		if(strlen(name)+1 > NAME_LEN)
			name_length = NAME_LEN-1;
		else
			name_length = strlen(name);
		strncpy(head_pty->name,name,name_length);
		head_pty->name[name_length]='\0';
		head_pty->pty_num = 0;
		head_pty->next = NULL;
		
		pty_maxnum++;
		return head_pty->pty_num;
	}
	else
	{
		if(pty_maxnum > 8)
		{
			gw_log(GW_LOG_LEVEL_DEBUG,("you cannot create pty more than 8\n"));
			return -1;
		}
		pty_maxnum++;
		p1=head_pty;
		while(NULL != p1->next)
			p1 = p1->next;
		if(NULL == (p2 = (Pty*)malloc(sizeof(Pty))))
		{
			gw_log(GW_LOG_LEVEL_DEBUG,("create pty device %s fail!\r\n", name));
			return -1;
		}
		if(strlen(name)+1 > NAME_LEN)
			name_length = NAME_LEN-1;
		else
			name_length = strlen(name);
		strncpy(p2->name,name,name_length);
		p2->name[name_length] = '\0';
		p2->pty_num = p1->pty_num + 1;
		p2->next = NULL;
		p1->next = p2;
	}
	return p2->pty_num;	
}

int OpenMasterDev(int pty_num)
{
	int pipe_one[2];
	Pty *pty_dev = head_pty;	
	
	if((NULL == head_pty) || (pty_num < 0))
		return -1;
	while((pty_dev->pty_num != pty_num))
	{
		pty_dev = pty_dev->next;
		if(NULL == pty_dev)
			return -1;
	}
	if(pipe(pipe_one) < 0)
		return -1;
	pty_dev->master_read = pipe_one[0];
	pty_dev->master_write = pipe_one[1];

	return pty_num*2;
}


int OpenSlaveDev(int pty_num)
{
	int pipe_two[2];
	Pty *pty_dev = head_pty;	
	
	if((NULL == head_pty) || (pty_num < 0))
		return -1;
	while((pty_dev->pty_num != pty_num))
	{
		pty_dev = pty_dev->next;
		if(NULL == pty_dev)
			return -1;
	}
	if(pipe(pipe_two) < 0)
		return -1;
	pty_dev->slave_read = pipe_two[0];
	pty_dev->slave_write = pipe_two[1];

	return pty_num*2+1;
}
	
/*return the number real writed in pipe  */
/*0 is cannot write, -1 is wrong         */

int pty_write(int fd,char *str,int length)
{
        fd_set writefd;
	int ret,write_num = 0,ms_select;
	struct timeval tv;	
	Pty *pty_dev = head_pty,*test=head_pty;
	
	tv.tv_sec = 0;
	tv.tv_usec = 0;
        FD_ZERO(&writefd);
	
	ms_select = fd%2;
	fd = fd/2;
	if((NULL == pty_dev) || (fd <0))
		return -1;
	while(pty_dev->pty_num != fd)
	{
		pty_dev = pty_dev->next;
		if(NULL == pty_dev)
			return -1;
	}
        if(MASTER_WRITE == ms_select)
        {
        	FD_SET(pty_dev->slave_write,&writefd);
	        ret = select(pty_dev->slave_write+1,NULL,&writefd,NULL,&tv);
		if(ret <= 0)
			return ret;
		else
		{
                	write_num=write(pty_dev->slave_write,str,length);
//			printf("write_num=%d\n",write_num);
			return write_num;
		}

        }
        else if(SLAVE_WRITE == ms_select)
        {
        	FD_SET(pty_dev->master_write,&writefd);
	        ret = select(pty_dev->master_write+1,NULL,&writefd,NULL,&tv);
		if(ret <= 0)
		{
			return ret;
		}
		else
		{
                	write(pty_dev->master_write,str,length);
			return write_num;
		}
        }
	else
        	return -1;
}

/*return the number real readed from pipe  */
/*0 is cannot read, -1 is wrong            */

int pty_read(int fd,char *str,int length)
{
        fd_set readfd;
	int ret,read_num=0,ms_select = 0;
	Pty *pty_dev = head_pty;
	struct timeval tv;	

	tv.tv_sec = 0;
	tv.tv_usec = 0;
        FD_ZERO(&readfd);
	
	ms_select = fd%2;
	fd = fd/2;
	if((NULL == pty_dev) || (fd <0))
	{
		return -1;
	}
	while(pty_dev->pty_num != fd)
	{
		pty_dev = pty_dev->next;
		if(NULL == pty_dev)
			return -1;
	}
        if(MASTER_READ == ms_select)
        {
        	FD_SET(pty_dev->master_read,&readfd);
	        ret = select(pty_dev->master_read+1,&readfd,NULL,NULL,&tv);
		if(ret <= 0)
			return ret;
		else
		{
			read_num = read(pty_dev->master_read,str,length);
			return read_num;
		}
        }
        else if(SLAVE_READ == ms_select)
        {
        	FD_SET(pty_dev->slave_read,&readfd);
	        ret = select(pty_dev->slave_read+1,&readfd,NULL,NULL,&tv);
		if(ret <= 0)
			return ret;
		else
		{
//			printf("test here length is %d\n",length);
			read_num = read(pty_dev->slave_read,str,length);
			return read_num;
		}
        }
        else
                return -1;
}

/*close device*/

void CloseMasterDev(int fd)
{
	int ms_select;
	Pty *p = head_pty;
	
	fd = fd/2;
	if((NULL == head_pty) || fd<0)
	{
		printf("close failed\n");
		return;	
	}
	while(p->pty_num != fd)
	{
		p = p->next;
		if(NULL == p)
		{
			printf("close failed!\n");
			return;
		}
	}
	close(p->master_read);
	close(p->master_write);
//	printf("close successfully!\n");
}

void CloseSlaveDev(int fd)
{
	int ms_select;
	Pty *p = head_pty;
	
	fd = fd/2;
	if((NULL == head_pty) || fd<0)
	{
		printf("close failed\n");	
		return;	
	}
	while(p->pty_num != fd)
	{
		p = p->next;
		if(NULL == p)
		{
			return;
			printf("close failed!\n");
		}
	}
	close(p->slave_read);
	close(p->slave_write);
}

void DeletePty(int pty_num)
{
	Pty *pty_dev = head_pty;
	Pty *p = head_pty;
	
	if((NULL == pty_dev) || (pty_num <0))
	{
		printf("delete failed\n");
		return;
	}
	while(pty_dev->pty_num != pty_num)
	{
		p = pty_dev;
		pty_dev = pty_dev->next;
		if(NULL == pty_dev)
		{
			printf("delete failed\n");
			return;
		}
	}
	p->next = pty_dev->next;
	close(pty_dev->master_read);
	close(pty_dev->master_write);
	close(pty_dev->slave_read);
	close(pty_dev->slave_write);
	
	pty_maxnum--;
	free(pty_dev);
//	printf("delete successfully\n");
}

#else

typedef struct pty
{
	char name[NAME_LEN];
	int pty_num;
	int fd_master;
	int fd_slave;
	struct sockaddr_in sa_master;
	struct sockaddr_in sa_slave;
	struct pty *next;
}Pty;

Pty *head_pty = NULL;
int pty_maxnum = 0;

void dumpPtyList()
{
	Pty * p = head_pty;

	while(p)
	{
		gw_printf("pty:\r\n");
		gw_printf("name -- %s\r\n", p->name);
		gw_printf("m_fd: %d, s_fd %d\r\n", p->fd_master, p->fd_slave);
		p = p->next;
	}
}

/*create a new pty added to the existing link */
/*failed return -1                            */
/*max number of pty is 8*/
int CreatePty(char *name)
{
	int name_length = 0;
	Pty *p1,*p2;

	if(NULL == head_pty)
	{
		if(NULL == (head_pty = (Pty*)malloc(sizeof(Pty))))
		{
			gw_log(GW_LOG_LEVEL_DEBUG,("create pty device %s fail!\r\n", name));
			return -1;
		}
		if(strlen(name)+1 > NAME_LEN)
			name_length = NAME_LEN-1;
		else
			name_length = strlen(name);
		strncpy(head_pty->name,name,name_length);
		head_pty->name[name_length]='\0';
		head_pty->pty_num = pty_maxnum++;;
		head_pty->next = NULL;

		return head_pty->pty_num;
	}
	else
	{
		if(pty_maxnum > 8)
		{
			gw_log(GW_LOG_LEVEL_DEBUG,("you cannot create pty more than 8\n"));
			return -1;
		}

		p1=head_pty;
		while(NULL != p1->next)
			p1 = p1->next;

		if(NULL == (p2 = (Pty*)malloc(sizeof(Pty))))
		{
			gw_log(GW_LOG_LEVEL_DEBUG,("create pty device %s fail!\r\n", name));
			return -1;
		}

		if(strlen(name)+1 > NAME_LEN)
			name_length = NAME_LEN-1;
		else
			name_length = strlen(name);

		p2->pty_num = pty_maxnum++;
		strncpy(p2->name,name,name_length);
		p2->name[name_length] = '\0';
		p2->next = NULL;
		p1->next = p2;
	}
	return p2->pty_num;
}

int OpenMasterDev(int pty_num)
{

	Pty *pty_dev = head_pty;

	if((NULL == head_pty) || (pty_num < 0))
		return -1;
	while((pty_dev->pty_num != pty_num))
	{
		pty_dev = pty_dev->next;
		if(NULL == pty_dev)
			return -1;
	}

	pty_dev->fd_master = socket(AF_INET, SOCK_DGRAM, PF_UNSPEC);
	if(pty_dev->fd_master == -1)
	{
		gw_log(GW_LOG_LEVEL_DEBUG, ("OpenMasterPtyDev fail!\r\n"));
		return (-1);
	}
	pty_dev->sa_master.sin_family = AF_INET;
	pty_dev->sa_master.sin_port = htons(7000+2*pty_dev->pty_num);
	pty_dev->sa_master.sin_addr.s_addr = INADDR_ANY;

	if(bind(pty_dev->fd_master, &pty_dev->sa_master, sizeof(struct sockaddr)) == -1)
	{
		gw_log(GW_LOG_LEVEL_DEBUG, ("OpenMasterPtyDev bind sock fail!\r\n"));
		close(pty_dev->fd_master);
		return (-1);
	}

	return pty_dev->fd_master;
}

int OpenSlaveDev(int pty_num)
{

	Pty *pty_dev = head_pty;

	if((NULL == head_pty) || (pty_num < 0))
		return -1;
	while((pty_dev->pty_num != pty_num))
	{
		pty_dev = pty_dev->next;
		if(NULL == pty_dev)
			return -1;
	}

	pty_dev->fd_slave = socket(AF_INET, SOCK_DGRAM, PF_UNSPEC);
	if(pty_dev->fd_slave == -1)
	{
		gw_log(GW_LOG_LEVEL_DEBUG, ("OpenSlavePtyDev fail!\r\n"));
		return (-1);
	}
	pty_dev->sa_slave.sin_family = AF_INET;
	pty_dev->sa_slave.sin_port = htons(7000+2*pty_dev->pty_num+1);
	pty_dev->sa_slave.sin_addr.s_addr = INADDR_ANY;

	if(bind(pty_dev->fd_slave, &pty_dev->sa_slave, sizeof(struct sockaddr)) == -1)
	{
		gw_log(GW_LOG_LEVEL_DEBUG, ("OpenSlavePtyDev bind sock fail!\r\n"));
		close(pty_dev->fd_slave);
		return (-1);
	}

	return pty_dev->fd_slave;
}

/*return the number real writed in pipe  */
/*0 is cannot write, -1 is wrong         */

int pty_write(int fd,char *str,int length)
{
    fd_set writefd;
	int write_num = 0, sel = 0;
	struct timeval tv;
	Pty *pty_dev = head_pty;

	while(pty_dev)
	{
		if(pty_dev->fd_master == fd )
		{
			sel = SLAVE_WRITE;
			break;
		}
		if(pty_dev->fd_slave == fd)
		{
			sel = MASTER_WRITE;
			break;
		}
		pty_dev = pty_dev->next;
	}

	if(!pty_dev)
		return 0;

	tv.tv_sec = 0;
	tv.tv_usec = 10;
    FD_ZERO(&writefd);

	if((NULL == pty_dev) || (fd <0))
		return -1;
#if 0
	FD_SET(fd,&writefd);
    ret = select(fd+1,NULL,&writefd,NULL,&tv);
	if(ret <= 0)
		return ret;
	else
#endif
	{
      	write_num=sendto(fd,str,length, 0, (sel == MASTER_WRITE)?&pty_dev->sa_master:&pty_dev->sa_slave, sizeof(struct sockaddr));
		return write_num;
	}

}

/*return the number real readed from pipe  */
/*0 is cannot read, -1 is wrong            */

int pty_read(int fd,char *str,int length)
{
    fd_set readfd;
	int ret,read_num=0, sel = 0;
	Pty *pty_dev = head_pty;
	struct timeval tv;


	while(pty_dev)
	{
		if(pty_dev->fd_master == fd )
		{
			sel = SLAVE_READ;
			break;
		}
		if(pty_dev->fd_slave == fd)
		{
			sel = MASTER_READ;
			break;
		}
		pty_dev = pty_dev->next;
	}

	if(pty_dev == NULL)
	{
		OAM_VCONPTY_DEBUG(("pty_read: no pty found (%d)!\r\n", fd));

		dumpPtyList();

		return 0;
	}

	tv.tv_sec = 0;
	tv.tv_usec = 10;
    FD_ZERO(&readfd);

	if((NULL == pty_dev) || (fd <0))
	{
		return -1;
	}
#if 0
   	FD_SET(fd,&readfd);
    ret = select(fd+1,&readfd,NULL,NULL,&tv);
    if(ret <= 0)
		return ret;
	else
#endif
	{
		socklen_t socklen = sizeof(struct sockaddr);
		read_num = recvfrom(fd,str,length, 0, (struct sockaddr*)((sel==MASTER_READ)?&pty_dev->sa_master:&pty_dev->sa_slave), &socklen);
		return read_num;
	}
}

/*close device*/

void CloseMasterDev(int fd)
{
	Pty *pty_dev = head_pty;
	if(fd <= 0)
		return;

	while(pty_dev)
	{
		if(pty_dev->fd_master == fd)
		{
			close(fd);
			pty_dev->fd_master = 0;
		}
		pty_dev = pty_dev->next;
	}
}

void CloseSlaveDev(int fd)
{

	Pty *pty_dev = head_pty;

	if(fd <= 0)
		return;

	while(pty_dev)
	{
		if(pty_dev->fd_slave == fd)
		{
			close(fd);
			pty_dev->fd_slave = 0;
		}
		pty_dev = pty_dev->next;
	}
}


void DeletePty(int pty_num)
{
	Pty *pty_dev = head_pty;
	Pty *p = head_pty;

	if((NULL == pty_dev) || (pty_num <0))
	{
		printf("delete failed\n");
		return;
	}
	while(pty_dev->pty_num != pty_num)
	{
		p = pty_dev;
		pty_dev = pty_dev->next;
		if(NULL == pty_dev)
		{
			printf("delete failed\n");
			return;
		}
	}
	p->next = pty_dev->next;

	CloseMasterDev(p->fd_master);
	CloseSlaveDev(p->fd_slave);

	pty_maxnum--;
	free(pty_dev);
//	printf("delete successfully\n");
}

#endif










