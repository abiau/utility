
#ifndef __VNET_H__
#define __VNET_H__

#include <sys/socket.h>
#include <netinet/in.h>
#include "vDefine.h"
#include "vCommon.h"

typedef struct sockaddr_in  saddrin_t;

typedef struct {
	char   ip[32];
	int    port;
	saddrin_t  addr;
} VAddr_st;

typedef struct {
	/* method. */
	int (*getskt)  (void* self);
	int (*connect) (void* self, char* ip, int port);
	int (*write)   (void* self, int skt,  char* buf, int bufLen);
	int (*read)    (void* self, int skt,  char* buf, int bufLen);
	int (*sendto)  (void* self, int skt,  char* buf, int bufLen, char* ip, int port);
	int (*recvfrom)(void* self, int skt,  char* buf, int bufLen, VAddr_st* pAddr);
	int (*listen)  (void* self, int maxSession);
	int (*accept)  (void* self, VAddr_st* pAddr);

	/* data. */
	char             m_type;     /* t: SOCK_STREAM ;  u: SOCK_DGRAM  */
	char             m_zero[7];  /* nouse */
	int              m_skt;
	int              m_bindPort;
	pthread_mutex_t  m_mutex;
} VNet_st;

VNet_st* vnet_create  (char type, int bindPort);
void     vnet_destroy (VNet_st* pNet);


int _vnet_Open     (char type);
int _vnet_Close    (int skt);
int _vnet_Set      (int skt);
int _vnet_Bind     (int skt, int port);
int vnet_GetSkt    (void* self);
int vnet_Listen    (void* self, int maxSession);
int vnet_Accept    (void* self, VAddr_st* pAddr);
int vnet_Connect   (void* self, char* ip, int port);
int vnet_Write     (void* self, int skt, char* buf, int bufLen);
int vnet_Read      (void* self, int skt, char* buf, int bufLen);
int vnet_Sendto    (void* self, int skt, char* buf, int bufLen, char* ip, int port);
int vnet_Recvfrom  (void* self, int skt, char* buf, int bufLen, VAddr_st* pAddr);

void      toVAddr   (VAddr_st* pAddr, saddrin_t addr);
saddrin_t toSaddrIn (char* ip, int port);














#endif  /* __VNET_H__ */


