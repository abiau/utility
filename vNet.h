
#ifndef __VNET_H__
#define __VNET_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include "vDefine.h"
#include "vCommon.h"
#include "vLog.h"

typedef struct sockaddr_in  saddrin_t;

typedef struct {
	char       ip[32];
	int        port;
	saddrin_t  addr;
} VAddr;

typedef struct {
	/* method. */
	int (*getskt)  (void* self);
	int (*connect) (void* self, char* ip, int port);
	int (*write)   (void* self, int skt,  char* buf, int bufLen);
	int (*read)    (void* self, int skt,  char* buf, int bufLen);
	int (*sendto)  (void* self, int skt,  char* buf, int bufLen, char* ip, int port);
	int (*recvfrom)(void* self, int skt,  char* buf, int bufLen, VAddr* pAddr);
	int (*listen)  (void* self, int maxSession);
	int (*accept)  (void* self, VAddr* pAddr);

	/* data. */
	char            m_type;     /* t: SOCKREAM ;  u: SOCK_DGRAM  */
	char            m_zero[7];  /* nouse */
	int             m_skt;
	int             m_bindPort;
	vmutex_t        m_mutex;
} VNet;

VNet* vnet_create  (char type, int bindPort);
void  vnet_destroy (VNet* pNet);


int _vnet_Open     (char type);
int _vnet_Close    (int skt);
int _vnet_Set      (int skt);
int _vnet_Bind     (int skt, int port);
int vnet_GetSkt    (void* self);
int vnet_Listen    (void* self, int maxSession);
int vnet_Accept    (void* self, VAddr* pAddr);
int vnet_Connect   (void* self, char* ip, int port);
int vnet_Write     (void* self, int skt, char* buf, int bufLen);
int vnet_Read      (void* self, int skt, char* buf, int bufLen);
int vnet_Sendto    (void* self, int skt, char* buf, int bufLen, char* ip, int port);
int vnet_Recvfrom  (void* self, int skt, char* buf, int bufLen, VAddr* pAddr);

void      toVAddr   (VAddr* pAddr, saddrin_t addr);
saddrin_t toSaddrIn (char* ip, int port);

#endif  /* __VNET_H__ */

//VNet* p = (VNet*) self;

