
#include "vNet.h"


VNet* vnet_create  (SOCKTYPE_e type, int bindPort)
{
	VNet* p = vc_malloc (sizeof(VNet)); 

	/* method. */
	p->getskt     = vnet_GetSkt;
	p->connect    = vnet_Connect;
	p->listen     = vnet_Listen;
	p->accept     = vnet_Accept;
	p->write      = vnet_Write;
	p->read       = vnet_Read;
	p->sendto     = vnet_Sendto;
	p->recvfrom   = vnet_Recvfrom;
	/* data. */
	p->m_type     = type;
	p->m_bindPort = bindPort;
	pthread_mutex_init (&p->m_mutex, NULL);

	p->m_skt = vnet_Open (p->m_type);
	vnet_Set  (p->m_skt);
	vnet_Bind (p->m_skt, p->m_bindPort);

	return (VNet*)p;
}

void vnet_destroy (VNet* pNet)
{
	vnet_Close (pNet->m_skt);

	vc_free (pNet, sizeof(VNet));
	return ;
}

int vnet_GetSkt (void* self)
{
	VNet* p = (VNet*) self;
	return p->m_skt;
}


int vnet_Open (SOCKTYPE_e type)
{
	int skt;
	int ret;
	int sock_type = -1;

	if (type==TCP)
	{
		sock_type = SOCK_STREAM;
	}
	else if (type==UDP)
	{
		sock_type = SOCK_DGRAM;
	}
	skt = socket (PF_INET, sock_type, 0);
	if (skt<0)
	{
		verr ("socket"); 
	}
		
	return skt;
}

int vnet_Close (int skt)
{
	return close (skt);
}


int vnet_Set (int skt)
{
	if (skt<0)
	{
		return -1;
	}

	int ret  = 0;
    int flag = 1;
	int sock_opt;

	sock_opt = 1;
	ret = setsockopt(skt, SOL_SOCKET, SO_REUSEADDR, (void*)&sock_opt, sizeof(sock_opt) );
	if (ret<0)
	{
		verr ("setsockopt"); 
		return ret;
	}

#if 0
	flag = fcntl (skt, F_GETFL, 0);
	ret  = fcntl (skt, F_SETFL, flag | O_NONBLOCK);
	if (ret<0)
	{
		verr ("fcntl"); 
	}
#endif
	
	return ret;
}

int vnet_Bind (int skt, int port)
{
	int        ret;
	saddrin_t  addr = toSaddrIn (NULL, port);

	ret = bind (skt, (struct sockaddr*) &addr, sizeof(saddrin_t));
	if (ret<0)
	{
		verr ("bind"); 
	}
	return ret;
}

int vnet_Listen (void* self, int maxSession)
{
	VNet*  p = (VNet*) self;
	int       ret;

	ret = listen (p->m_skt, maxSession);
	if (ret<0)
	{
		verr ("listen"); 
	}
	return ret;
}

int vnet_Accept (void* self, VAddr* pAddr)
{
	VNet*   p = (VNet*) self;

	int        skt_accept; 
	saddrin_t  addr;
	socklen_t  clen = sizeof(saddrin_t);

	skt_accept = accept (p->m_skt, (struct sockaddr *)&addr, &clen); 

	toVAddr (pAddr, addr);

	return skt_accept;
}

int vnet_Connect (void* self, char* ip, int port)
{
	VNet*   p = (VNet*) self;

	int        ret;
	saddrin_t  addr = toSaddrIn(ip, port);

	ret = connect(p->m_skt, (struct sockaddr *)&addr, sizeof(saddrin_t));
	if(ret<0)
	{
		verr ("connect"); 
	}

	return ret;
}

int vnet_Write (void* self, int skt,  char* buf, int bufLen)
{
	VNet*  p = (VNet*) self;
	int       ret;
	
	ret = write (skt, buf, bufLen);
	if (ret<=0)
	{
		verr ("write"); 
	}

	return ret;
}

int vnet_Read (void* self, int skt,  char* buf, int bufLen) 
{
	VNet*  p = (VNet*) self;
	int       ret;

	ret = read (skt, buf, bufLen);
	if (ret<=0)
	{
		verr ("read"); 
	}

	return ret;
}

int vnet_Sendto (void* self, int skt,  char* buf, int bufLen, char* ip, int port)
{
	VNet*   p = (VNet*) self;
	int        ret;
	socklen_t  clen = sizeof(struct sockaddr);
	saddrin_t  addr = toSaddrIn(ip, port);
	
	ret = sendto (p->m_skt, buf, bufLen, 0, (struct sockaddr*)&(addr), clen);
	if (ret<=0)
	{
		verr ("sendto"); 
	}

	return ret;
}

int vnet_Recvfrom  (void* self, int skt, char* buf, int bufLen, VAddr* pAddr)
{
	VNet*   p = (VNet*) self;
	int        ret;
	saddrin_t  addr;
	socklen_t  clen = sizeof(saddrin_t);

	ret = recvfrom (skt, buf, bufLen, 0, (struct sockaddr*) &addr, &clen);
	if (ret<=0)
	{
		verr ("recvfrom"); 
	}
	if (pAddr)
	{
		toVAddr (pAddr, addr);
	}
	return ret;
}


void toVAddr (VAddr* pAddr, saddrin_t addr)
{
	vzero (pAddr, VAddr);
	memcpy (&pAddr->addr, &addr, sizeof(saddrin_t));
	snprintf (pAddr->ip, sizeof(pAddr->ip), "%s", inet_ntoa (addr.sin_addr));
	pAddr->port = addr.sin_port;
	return;
}

saddrin_t toSaddrIn (char* ip, int port)
{
	struct hostent*  hen;
	saddrin_t        addr; 

	vzero (&addr, saddrin_t);
	
	if (ip)
	{
		hen = gethostbyname (ip);
		if (hen==NULL)
		{
			verr ("gethostbyname"); 
			return addr;
		}
		memcpy (&addr.sin_addr.s_addr, hen->h_addr, hen->h_length);
	}
	else
	{
		addr.sin_addr.s_addr = INADDR_ANY;
	}
	addr.sin_family = PF_INET;
	addr.sin_port   = htons(port);

	return addr;
}




