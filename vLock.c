
#include "vLock.h"

VLock* vlock_create ()
{
	VLock* pLock = vc_malloc (sizeof(VLock));
	vzero (pLock, sizeof(VLock));
	pLock->lock = vlock_lock;
	pLock->unlock = vlock_unlock;
	pthread_mutex_init (&pLock->mutex, NULL);
	return pLock;
}

void vlock_destroy (VLock* pLock)
{
	vc_free (pLock, sizeof(VLock));
	return ;
}

void vlock_lock (void* self)
{
	VLock* pLock = self;
	pthread_mutex_lock (&pLock->mutex);
	return ;
}
void vlock_unlock (void* self) 
{
	VLock* pLock = self;
	pthread_mutex_unlock (&pLock->mutex);
	return ;
}




