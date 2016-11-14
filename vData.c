
#include "vData.h"


/*************************************************************/
/*************************************************************/

int* int_new (int n)
{
	int* p = vc_malloc (sizeof(int));
	return p;
}

void int_del (int* p)
{
	vc_free (p, sizeof(int));
}


u64t vnode_newId (void)
{
	static u64t id=0;
	return  __sync_add_and_fetch (&id, 1);
}

VNode* vnode_create (void* arg)
{
	VNode* node = vc_malloc (sizeof(VNode));
	node->id = vnode_newId();
	node->arg = arg;
	return node;
}

void vnode_destroy (VNode* node)
{
	vc_free (node, sizeof(VNode));
	return ;
}



/*************************************************************/
/*************************************************************/


VList* vlist_create  (void)
{
	VList* pList = vc_malloc (sizeof(VList)); 
	
	/* method. */
	pList->Lock        = vlock_create();
	pList->lock        = vlist_lock;
	pList->unlock      = vlist_unlock;
	pList->insert      = vlist_insert;
	pList->search      = vlist_search;
	pList->delete      = vlist_delete;
	pList->seek        = vlist_seek;
	pList->foreach     = vlist_foreach;
	pList->travel      = vlist_travel;
	/* data. */
	
	/* inheritance. */

	return (VList*)pList;
}

void vlist_destroy (VList* pList)
{
	vlock_destroy(pList->Lock);
	vc_free (pList, sizeof(VList));
	return ;
}

#if 1
void vlist_lock (void* self)
{
	VList* pList = (VList*) self;
	pList->Lock->lock(pList->Lock);
}
void vlist_unlock (void* self) 
{
	VList* pList = (VList*) self;
	pList->Lock->unlock(pList->Lock);
}
#endif







void vlist_travel (void* self, todo_ft todo)
{
	VList* pList = (VList*) self;
	VNode* pNode=NULL;

	if (todo)
	{
		pNode = pList->head;
		while (pNode)
		{
			todo (pNode->arg);
			pNode = pNode->next;
		}
	}
	return ;
}


VNode* vlist_insertFrom (void* self, POS_e from, comp_ft less, void* arg)
{
	VList* pList = (VList*) self;
	VNode* node = vnode_create (arg);
	if (!node)
	{
		return NULL;
	}
	pList->size++;

	if (!less)
	{
		_vlist_pushNode (pList, END, node);
	}
	else
	{
		VNode* pNode = _vlist_searchFalse (pList, from, less, arg);

			if (from==BEGIN)
			{
				if (pNode)
				{
					if (pNode->prev)
					{
						/*  ...qNp...  */
						node->prev = pNode->prev;
						node->next = pNode;
						pNode->prev->next = node;
						pNode->prev = node;
					}
					else
					{
						/*         Np...  */
						_vlist_pushNode (pList, BEGIN, node);
					}
				}
				else
				{
						/*         N         */
						/*      ...N         */
						_vlist_pushNode (pList, END, node);
				}
			}
			else if (from==END)
			{
				if (pNode)
				{
					if (pNode->next)
					{
						/*  ...pNq...  */
						node->next = pNode->next;
						node->prev = pNode;
						pNode->next->prev = node;
						pNode->next = node;
					}
					else
					{
						/*  ...pN      */
						_vlist_pushNode (pList, END, node);
					}
				}
				else
				{
						/*      N      */
						/*      N...   */
						_vlist_pushNode (pList, BEGIN, node);
				}
			}
	}

	return node;
}

VNode* vlist_insert (void* self, comp_ft less, void* arg)
{
	return vlist_insertFrom (self, BEGIN, less, arg);
}


VNode* vlist_searchFrom (void* self, POS_e from, comp_ft equal, void* arg)
{
	VList* pList = (VList*) self;
	if (!equal)
	{
		return NULL;
	}
	
	VNode* pNode=NULL;
	if (from==BEGIN)
	{
		pNode = pList->head;
	}
	else if (from==END)
	{
		pNode = pList->tail;
	}

	while (pNode)
	{
		if (equal(pNode->arg, arg)==OK)
		{
			break;
		}
		else
		{
			if (from==BEGIN)
			{
				pNode = pNode->next;
			}
			else if (from==END)
			{
				pNode = pNode->prev;
			}
		}
	}

	return pNode;
}

VNode* vlist_search (void* self, comp_ft equal, void* arg)
{
	return vlist_searchFrom (self, BEGIN, equal, arg);
}

void vlist_delete (void* self, VNode* node, dtor_ft del)
{
	VList* pList = (VList*) self;
	VNode* pNode = node;
	if (pNode)
	{
		if (pList->cur == pNode)
		{
			pList->cur = NULL;
		}
		if (pNode->prev)
		{
			pNode->prev->next = pNode->next;
		}
		else
		{
			pList->head = pNode->next;
		}
		if (pNode->next)
		{
			pNode->next->prev = pNode->prev;
		}
		else
		{
			pList->tail = pNode->prev;
		}
		if (del)
		{
			del(pNode->arg);
		}
		vnode_destroy (pNode);
		pNode = NULL;
		pList->size--;
	}
	return ;
}

void vlist_seekNode (void* self, VNode* node)
{
	VList* pList = (VList*) self;
	if (node)
	{
		pList->cur = node;
	}
	else
	{
		pList->cur = pList->head;
	}
	return ;
}

void  vlist_seekFrom (void* self, POS_e from)
{
	VList* pList = (VList*) self;
	if (from==BEGIN)
	{
		pList->cur = pList->head;
	}
	else if (from==END)
	{
		pList->cur = pList->tail;
	}
	else
	{
		pList->cur = NULL;
	}
	return ;
}

void vlist_seek (void* self)
{
	vlist_seekFrom (self, BEGIN);
	return ;
}

VNode* vlist_foreachFrom   (void* self, POS_e from, comp_ft filter, void* arg)
{
	VList* pList = (VList*) self;
	VNode* pNode=NULL;

	if (filter && arg)
	{
		while ((pNode = pList->cur))
		{
			if (from == BEGIN)
			{
				pList->cur = pList->cur->next;
			}
			else if (from == END)
			{
				pList->cur = pList->cur->prev;
			}
			else
			{
				pList->cur = NULL;
			}
			if (filter(pNode->arg, arg)==OK)
			{
				break;
			}
		}
	}
	else
	{
		if ((pNode = pList->cur))
		{
			if (from == BEGIN)
			{
				pList->cur = pList->cur->next;
			}
			else if (from == END)
			{
				pList->cur = pList->cur->prev;
			}
			else
			{
				pList->cur = NULL;
			}
		}
	}
	return pNode;
}

VNode* vlist_foreach (void* self, comp_ft filter, void* arg)
{
	return vlist_foreachFrom (self, BEGIN, filter, arg);
}


void _vlist_pushNode (void* self, POS_e to, VNode* node)
{
	VList* pList = (VList*) self;
	
	if (to==BEGIN)
	{
		if (pList->head)
		{
			/*   N...   */
			node->next = pList->head;
			pList->head->prev = node;
			pList->head = node;
		}
		else
		{
			/*   N   */
			pList->head = node;
			pList->tail = node;
		}
	}
	else if (to==END)
	{
		if (pList->tail)
		{
			/*   ...N   */
			node->prev = pList->tail;
			pList->tail->next = node;
			pList->tail = node;
		}
		else
		{
			/*   N   */
			pList->head = node;
			pList->tail = node;
		}
	}

	return ;
}


VNode* _vlist_searchFalse (void* self, POS_e from, comp_ft comp, void* arg)
{
	VList* pList = (VList*) self;
	if (!comp)
	{
		return NULL;
	}
	
	VNode* pNode=NULL;
	if (from==BEGIN)
	{
		pNode = pList->head;
	}
	else if (from==END)
	{
		pNode = pList->tail;
	}

	while (pNode)
	{
		if (comp(pNode->arg, arg)==OK)
		{
			if (from==BEGIN)
			{
				pNode = pNode->next;
			}
			else if (from==END)
			{
				pNode = pNode->prev;
			}
		}
		else
		{
			break;
		}
	}
	return pNode;
}



/*************************************************************/
/*************************************************************/

























/*************************************************************/
/*************************************************************/




VTree* vtree_create (void)
{
	VTree* pTree = vc_malloc (sizeof(VTree)); 

	/* method. */
	pTree->Lock        = vlock_create();
	pTree->lock        = vtree_lock;
	pTree->unlock      = vtree_unlock;
	pTree->insert      = vtree_insert;
	pTree->search      = vtree_search;
	pTree->delete      = vtree_delete;
	pTree->seek        = vtree_seek;
	pTree->travel      = vtree_travel;
	pTree->foreach     = vtree_foreach;
	/* data. */
	//pthread_mutex_init (&pTree->mutex, NULL);

	return (VTree*)pTree;
}

void vtree_destroy (VTree* pTree)
{
	vlock_destroy (pTree->Lock);
	vc_free (pTree, sizeof(VTree));
	return ;
}

void vtree_lock (void* self)
{
	VTree* pTree = self;
	pTree->Lock->unlock(pTree->Lock);
	//pthread_mutex_lock (&pTree->mutex);
}
void vtree_unlock (void* self) 
{
	VTree* pTree = self;
	pTree->Lock->unlock(pTree->Lock);
}




void vtree_travel (void* self, todo_ft todo)
{
	VTree* pTree = (VTree*) self;
	if (todo)
	{
		VNode* pNode = _vtree_mostL (self, pTree->head);
		if (pNode)
		{
			todo (pNode->arg);
		}
		while ((pNode = _vtree_next(self, pNode)))
		{
			todo (pNode->arg);
		}
	}
	return ;
}

VNode* vtree_insert (void* self, comp_ft less, void* arg)
{
	if (!less)
	{
		return NULL;
	}
	VTree* pTree = (VTree*) self;
	VNode* node = vnode_create (arg);
	if (!node)
	{
		return NULL;
	}
	pTree->size++;

	VNode* pNode = pTree->head;
	if (!pNode)
	{
		pTree->head = node;
	}
	else
	{
		while (pNode)
		{
			/* |            Node            |
			 * |          /      \          |
			 * |     L:False     R:True     | */
			if(less(pNode->arg, arg)==OK)
			{
				if (!pNode->R)
				{
					pNode->R = node;
					node->F = pNode;
					break;
				}
				else
				{
					pNode = pNode->R;
					continue;
				}
			}
			else
			{
				if (!pNode->L)
				{
					pNode->L = node;
					node->F = pNode;
					break;
				}
				else
				{
					pNode = pNode->L;
					continue;
				}
			}
		}
	}
	return node;
}


VNode* vtree_search (void* self, comp_ft equal, void* arg)
{
	VTree* pTree = (VTree*) self;
	VNode* pNode=pTree->head;
	if (!pNode || !equal)
	{
		return NULL;
	}

	while (pNode)
	{
		int ret=equal(pNode->arg, arg);
		if (ret==0)
		{
			/* Found. */
			break;
		}
		else if (ret<0)
		{
			/* Find L. */
			pNode = pNode->L;
		}
		else if (ret>0)
		{
			/* Find R. */
			pNode = pNode->R;
		}
	}
	return pNode;
}

void vtree_delete (void* self, VNode* node, dtor_ft del)
{
	VTree* pTree = (VTree*) self;
	VNode* pNode = node;
	if (!pNode)
	{
		return ;
	}

	void* pArg = pNode->arg;
	while (pNode)
	{
		if (!pNode->L && !pNode->R)
		{
			/* 0 child. */
			if (_vtree_isLorR(self, pNode)<0)
			{
				pNode->F->L = NULL;
			}
			else if (_vtree_isLorR(self, pNode)>0)
			{
				pNode->F->R = NULL;
			}
			else if (_vtree_isLorR(self, pNode)==0)
			{
				pTree->head = NULL;
			}
			vnode_destroy (pNode);
			pNode = NULL;
			pTree->size--;
		}
		else if (pNode->L && pNode->R)
		{
			/* 2 children. */
			VNode* pCandidate = _vtree_mostR (self, pNode->L);
			pNode->arg = pCandidate->arg;
			pNode = pCandidate;
		}
		else
		{
			/* 1 child. */
			if (pNode->L)
			{
				if (_vtree_isLorR(self, pNode)<0)
				{
					pNode->F->L = pNode->L;
					pNode->L->F = pNode->F;
				}
				else if (_vtree_isLorR(self, pNode)>0)
				{
					pNode->F->R = pNode->L;
					pNode->L->F = pNode->F;
				}
				else if (_vtree_isLorR(self, pNode)==0)
				{
					pTree->head = pNode->L;
					pNode->L->F = NULL;
				}
			}
			else if (pNode->R)
			{
				if (_vtree_isLorR(self, pNode)<0)
				{
					pNode->F->L = pNode->R;
					pNode->R->F = pNode->F;
				}
				else if (_vtree_isLorR(self, pNode)>0)
				{
					pNode->F->R = pNode->R;
					pNode->R->F = pNode->F;
				}
				else if (_vtree_isLorR(self, pNode)==0)
				{
					pTree->head = pNode->R;
					pNode->R->F = NULL;
				}
			}
			vnode_destroy (pNode);
			pNode = NULL;
			pTree->size--;
		}
	}

	if (del)
	{
		del(pArg);
	}
	return ;
}


void vtree_seekNode (void* self, VNode* node)
{
	VTree* pTree = (VTree*) self;
	if (node)
	{
		pTree->cur = node;
	}
	else
	{
		pTree->cur = _vtree_mostL (self, pTree->head);
	}
	return ;
}

void vtree_seekFrom (void* self, POS_e from) 
{
	VTree* pTree = (VTree*) self;

	if (from==BEGIN)
	{
		pTree->cur = _vtree_mostL (self, pTree->head);
	}
	else if (from==END)
	{
		pTree->cur = _vtree_mostR (self, pTree->head);
	}
	else
	{
		pTree->cur = NULL;
	}
	return ;
}

void vtree_seek (void* self)
{
	vtree_seekFrom (self, BEGIN);
	return ;
}

VNode* vtree_foreachFrom (void* self, POS_e from, comp_ft filter, void* arg)
{
	VTree* pTree = (VTree*) self;
	VNode* pNode = NULL;
	
	if (filter && arg)
	{
		while ((pNode = pTree->cur))
		{
			if (from == BEGIN)
			{
				pTree->cur = _vtree_next(self, pNode);
			}
			else if (from == END)
			{
				pTree->cur = _vtree_prev(self, pNode);
			}
			else
			{
				pTree->cur = NULL;
			}
			if (filter(pNode->arg, arg)==OK)
			{
				break;
			}
		}
	}
	else
	{
		if ((pNode = pTree->cur))
		{
			if (from == BEGIN)
			{
				pTree->cur = _vtree_next(self, pNode);
			}
			else if (from == END)
			{
				pTree->cur = _vtree_prev(self, pNode);
			}
			else
			{
				pTree->cur = NULL;
			}
		}
	}
	return pNode;
}

VNode* vtree_foreach (void* self, comp_ft filter, void* arg)
{
	return vtree_foreachFrom (self, BEGIN, filter, arg);
}


VNode* _vtree_mostR (void* self, VNode* node)
{
	VNode* pNode=node;
	if (pNode)
	{
		while (pNode->R)
		{
			pNode = pNode->R;
		}
	}
	return pNode;
}

VNode* _vtree_mostL (void* self, VNode* node)
{
	VNode* pNode=node;
	if (pNode)
	{
		while (pNode->L)
		{
			pNode = pNode->L;
		}
	}
	return pNode;
}

VNode* _vtree_next (void* self, VNode* node)
{
	VNode* pNode = node;
	if (!pNode)
	{
		return NULL;
	}

	if (pNode->R)
	{
		return _vtree_mostL (self, pNode->R);
	}
	else
	{
		VNode* parent = pNode->F;
		while (parent && pNode==parent->R)
		{
			pNode = parent;
			parent = parent->F;
		}
		return parent;
	}
}

VNode* _vtree_prev (void* self, VNode* node)
{
	VNode* pNode=node;
	if (!pNode)
	{
		return NULL;
	}

	if (pNode->L)
	{
		return _vtree_mostR (self, pNode->L);
	}
	else
	{
		VNode* parent = pNode->F;
		while (parent && pNode==parent->L)
		{
			pNode = parent;
			parent = parent->F;
		}
		return parent;
	}
}


int _vtree_isLorR (void* self, VNode* node)
{
	if (node->F)
	{
		if (node==node->F->L)
		{
			return -1;
		}
		else if (node==node->F->R)
		{
			return 1;
		}
	}
	return 0;
}






