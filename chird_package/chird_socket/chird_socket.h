#ifndef _CHIRD_SOCKET_H_
#define _CHIRD_SOCKET_H_

#ifdef SOLARIS
#include <sys/sockio.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <net/if_arp.h>


#define SA struct sockaddr

#define	 NETCARD_TYPE "eth0"		//��������: eth0 ����������wlan1 ��������


/* 	��ȡ���� IP ��ַ
	ethname	: ָ���������ƣ���������:eth0  ��������: wlan1
	flag	: 0 ��ȡIP��ַ     1 ��ȡ�㲥��ַ
	padr	: ��ַ�ṹ�壬�洢��ȡ��ip��ַ */
int chird_IP_Get(const char *ethname, char flag, struct in_addr *padr);

/* ������IPת���ʮ���� */
char * chird_ntos(const struct in_addr * addr);		// inet_ntop	
/* ���ʮ���� ת ������IP*/
struct in_addr chird_ston(const char * ip);	// inet_pton

/*============================================================
*							TCP
*============================================================*/

#if 0
typedef struct {
	
}CHIRD_TCP_INFO_S;
#endif

/*�ͻ������Ӻ���*/
int chird_tcp_connect(const char * ip , int port) ;

/*����˼�������*/
int chird_tcp_listen(int port/*, void (*paccept)(void * param), void * param*/);

/*����˵ȴ����Ӻ���*/
int chird_tcp_accept(int sockfd, struct sockaddr_in * addr);

/*����tcp���ݺ��� timeout Ϊ0ʱ����ʱ*/
int chird_tcp_recv(int fd, char * buf, int size, int timeout);

/*����tcp���ݺ���*/
int chird_tcp_send(int fd, const char * buf, int size);


/*============================================================
*							UDP
*============================================================*/

/* ����udp�׽���		port �˿ںţ�
*  nbcast : 1 �����㲥�׽���  0 : ������ͨ�׽��� */
int chird_udp_create(int port, int nbcast);

/*����udp����*/
int chird_udp_recv(int fd, char *buf, int size, struct sockaddr_in *addr, int timeout);

/*����udp���� timeout Ϊ0ʱ����ʱ*/
int chird_udp_send(int fd, char *buf, int buf_size, const struct sockaddr_in *addr);

/* ָ��IP�Ͷ˿ںŷ��� */
int chird_udp_send_byip(int fd, char *buf, int size, const char  * ip, int port);

#endif
