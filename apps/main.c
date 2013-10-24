/*
 * main.c
 *
 *  Created on: 2012-11-12
 *      Author: tommy
 */

#include "../include/gw_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if USING_BSD_SOCK
#include <sys/socket.h>

#include <netinet/in.h>
#else
#include <lwip/sockets.h>
#endif

#include "pthread.h"

int mfd = 0, sfd = 0;
struct sockaddr_in sa, sb;

void init_sock_pty()
{


	memset(&sb, 0, sizeof(sb));
	memset(sa.sin_zero, 0, sizeof(sa.sin_zero));

	mfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(mfd == -1)
	{
		printf("master fd init fail\r\n");
		return;
	}

	sfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sfd == -1)
	{
		printf("slave fd init fail\r\n");
		return;
	}

	sa.sin_family = AF_INET;
	sa.sin_port = 1000;
	sa.sin_addr.s_addr = INADDR_ANY;


	bind(mfd, (struct sockaddr*)&sa, sizeof(sa));

	sb.sin_family = AF_INET;
	sb.sin_port = 2000;
	sb.sin_addr.s_addr = INADDR_ANY;
	bind(sfd, (struct sockaddr*)&sb, sizeof(sb));


}

void * sfd_recv(void * data)
{
	char c;
	int size = 1;
	int socklen = sizeof(sb);
	int len = 0;

	init_sock_pty();

	while(1)
	{
	 len = recvfrom(sfd, &c, size, 1, /*(__SOCKADDR_ARG)*/&sb, (socklen_t*)&socklen);
	 if(len > 0)
		printf(">>%c\r\n", c);
	}
}

int main(int argc, char **argv) {

	char c;
	pthread_t srecv;

	pthread_create(&srecv, NULL, sfd_recv,  NULL);

	while(1)
	{
		scanf("%c", &c);
		if(c == 'E')
			break;
		sendto(sfd, &c, 1, 0, /*(__CONST_SOCKADDR_ARG)*/&sb, (socklen_t)sizeof(sb));
	}


	return 0;
}

