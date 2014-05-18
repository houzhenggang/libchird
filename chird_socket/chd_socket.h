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
//#include <linux/route.h>

#define MAX_NET_CARDS		10
#define SA struct sockaddr

#define IP_ADDR		 0
#define BROADCAST_IP 1
#define NETMASK_IP   2


int chird_net_travelcard();

/* ��������д��buf�У���:��������lo:eth0:wlan0 ��һ���϶���lo*/
int chird_net_getcard2buff(void * buf, int * cardnum);


int chird_net_setIP(const char * netcard, char flag, const char * addr, int enable);

/* 	��ȡ���� IP ��ַ
	ethname	: ָ���������ƣ���������:eth0  ��������: wlan1
	flag	: 0 ��ȡIP��ַ     1 ��ȡ�㲥��ַ
	padr	: ��ַ�ṹ�壬�洢��ȡ��ip��ַ */
int chird_net_getIP(const char * netcard, char flag, struct in_addr * padr);

/* ������IPת���ʮ���� */
char * chird_net_ntos(const struct in_addr * addr);		// inet_ntop	
/* ���ʮ���� ת ������IP*/
struct in_addr chird_net_ston(const char * ip);	// inet_pton

/*============================================================
*							TCP
*============================================================*/

/*�ͻ������Ӻ���*/
int chird_tcp_connect(const char * ip, int port, int timeout);

/*����˼�������*/
int chird_tcp_listen(int port/*, void (*paccept)(void * param), void * param*/);

/*����˵ȴ����Ӻ���*/
int chird_tcp_accept(int sockfd, struct sockaddr_in * addr);

/*����tcp���ݺ��� timeout Ϊ0ʱ����ʱ*/
int chird_tcp_recv(int fd, void * buf, int size, int timeout);

/*����tcp���ݺ���*/
int chird_tcp_send(int fd, const void * buf, int size);


/*============================================================
*							UDP
*============================================================*/

/* ����udp�׽���		port �˿ںţ�
*  nbcast : 1 �����㲥�׽���  0 : ������ͨ�׽��� 
   nettype: �������� ��eth0 wlan0��
*/
int chird_udp_create(int port, int nbcast, const char * nettype);

/*����udp����*/
int chird_udp_recv(int fd,  void * buf, int size, struct sockaddr_in *addr, int timeout);

/*����udp���� timeout Ϊ0ʱ����ʱ*/
int chird_udp_send(int fd,  void * buf, int buf_size, const struct sockaddr_in *addr);

/* ָ��IP�Ͷ˿ںŷ��� */
int chird_udp_send_byips(int fd,  void * buf, int size, const char  * ip, int port);

int chird_udp_send_byip(int fd,  void * buf, int size, struct in_addr inaddr, int port);

#endif
