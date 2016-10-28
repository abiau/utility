
#include "vData.h"


/*************************************************************/
/*************************************************************/

u64t vdatanode_newId ()
{
	static u64t id=0;
	return  __sync_add_and_fetch (&id, 1);
}

VDataNode* vdatanode_create (void* arg)
{
	VDataNode* node = vc_malloc (sizeof(VDataNode));
	if (!node)
	{
		return NULL;
	}
	memset (node, 0, sizeof(VDataNode));
	node->id = vdatanode_newId();
	node->arg = arg;
	return node;
}

void vdatanode_destroy (VDataNode* node)
{
	vc_free (node, sizeof(VDataNode));
	node = NULL;
	return ;
}

int* int_new (int n)
{
	int* p = vc_malloc (sizeof(int));
	if (p)
	{
		*p = n;
	}
	return p;
}

void int_del (int* p)
{
	vc_free (p, sizeof(int));
}



/*************************************************************/
/*************************************************************/







VDataList* vdatalist_create  ()
{
	VDataList* pList = vc_malloc (sizeof(VDataList)); 
	if (pList==NULL)
	{
		return NULL;
	}

	memset (pList, 0, sizeof(VDataList));
	/* method. */
	pList->lock        = vdatalist_Lock;
	pList->unlock      = vdatalist_Unlock;
	pList->insert      = vdatalist_Insert;
	pList->search      = vdatalist_Search;
	pList->delete      = vdatalist_Delete;
	pList->seek        = vdatalist_Seek;
	pList->foreach     = vdatalist_Foreach;
	pList->travel      = vdatalist_Travel;
	/* data. */
	pthread_mutex_init (&pList->m_mutex, NULL);

	return (VDataList*)pList;
}

void vdatalist_destroy (VDataList* pList)
{
	vc_free (pList, sizeof(VDataList));
	return ;
}

void vdatalist_Lock (void* self)
{
	VDataList* pList = (VDataList*) self;
	pthread_mutex_lock (&pList->m_mutex);
}
void vdatalist_Unlock (void* self) 
{
	VDataList* pList = (VDataList*) self;
	pthread_mutex_unlock (&pList->m_mutex);
}








void _vdatalist_PushNode (void* self, POS_e to, VDataNode* node)
{
	VDataList* pList = (VDataList*) self;
	
	if (to==HEAD)
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
	else if (to==TAIL)
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

VDataNode* vdatalist_Insert (void* self, POS_e from, comp_ft comp, void* arg)
{
	VDataList* pList = (VDataList*) self;
	VDataNode* node = vdatanode_create (arg);
	if (!node)
	{
		return NULL;
	}
	pList->size++;

	if (!comp)
	{
		_vdatalist_PushNode (pList, from, node);
	}
	else
	{
		VDataNode* pNode = _vdatalist_SearchFalse (pList, from, comp, arg);

			if (from==HEAD)
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
						_vdatalist_PushNode (pList, HEAD, node);
					}
				}
				else
				{
						/*         N         */
						/*      ...N         */
						_vdatalist_PushNode (pList, TAIL, node);
				}
			}
			else if (from==TAIL)
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
						_vdatalist_PushNode (pList, TAIL, node);
					}
				}
				else
				{
						/*      N      */
						/*      N...   */
						_vdatalist_PushNode (pList, HEAD, node);
				}
			}
	}

	return node;
}

VDataNode* _vdatalist_SearchFalse (void* self, POS_e from, comp_ft comp, void* arg)
{
	VDataList* pList = (VDataList*) self;
	if (!comp)
	{
		return NULL;
	}
	VDataNode* pNode;
	if (from==HEAD)
	{
		pNode = pList->head;
	}
	else if (from==TAIL)
	{
		pNode = pList->tail;
	}
	while (pNode)
	{
		if (comp(pNode->arg, arg)==OK)
		{
			if (from==HEAD)
			{
				pNode = pNode->next;
			}
			else if (from==TAIL)
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

VDataNode* vdatalist_Search (void* self, POS_e from, comp_ft equal, void* arg)
{
	VDataList* pList = (VDataList*) self;
	if (!equal)
	{
		return NULL;
	}
	
	VDataNode* pNode;
	if (from==HEAD)
	{
		pNode = pList->head;
	}
	else if (from==TAIL)
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
			if (from==HEAD)
			{
				pNode = pNode->next;
			}
			else if (from==TAIL)
			{
				pNode = pNode->prev;
			}
		}
	}

	return pNode;
}

void vdatalist_Delete (void* self, VDataNode* node, dtor_ft del)
{
	VDataList* pList = (VDataList*) self;
	if (node)
	{
		if (pList->cur == node)
		{
			pList->cur = NULL;
		}
		if (node->prev)
		{
			node->prev->next = node->next;
		}
		else
		{
			pList->head = node->next;
		}
		if (node->next)
		{
			node->next->prev = node->prev;
		}
		else
		{
			pList->tail = node->prev;
		}
		if (del)
		{
			del(node->arg);
		}
		vdatanode_destroy (node);
	}
	return ;
}

void vdatalist_Seek (void* self, VDataNode* node)
{
	VDataList* pList = (VDataList*) self;
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

VDataNode* vdatalist_Foreach (void* self, POS_e from, comp_ft filter, void* arg)
{
	VDataList* pList = (VDataList*) self;
	VDataNode* pNode=NULL;

	if (filter && arg)
	{
		while (pNode = pList->cur)
		{
			if (from == HEAD)
			{
				pList->cur = pList->cur->next;
			}
			else if (from == TAIL)
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
		if (pNode = pList->cur)
		{
			if (from == HEAD)
			{
				pList->cur = pList->cur->next;
			}
			else if (from == TAIL)
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

void vdatalist_Travel (void* self, POS_e from, todo_ft todo)
{
	VDataList* pList = (VDataList*) self;
	VDataNode* pNode;

	if (todo)
	{
		if (from==HEAD)
		{
			pNode = pList->head;
			while (pNode)
			{
				todo (pNode->arg);
				pNode = pNode->next;
			}
		}
		else if (from==TAIL)
		{
			pNode = pList->tail;
			while (pNode)
			{
				todo (pNode->arg);
				pNode = pNode->prev;
			}
		}
	}
	return ;
}









/*************************************************************/
/*************************************************************/




VDataTree* vdatatree_create  ()
{
	VDataTree* pTree = vc_malloc (sizeof(VDataTree)); 
	if (pTree==NULL)
	{
		return NULL;
	}

	memset (pTree, 0, sizeof(VDataTree));
	/* method. */
	pTree->lock        = vdatatree_Lock;
	pTree->unlock      = vdatatree_Unlock;
	pTree->insert      = vdatatree_Insert;
	pTree->search      = vdatatree_Search;
	pTree->delete      = vdatatree_Delete;
	pTree->seek        = vdatatree_Seek;
	pTree->travel      = vdatatree_Travel;
	pTree->foreach     = vdatatree_Foreach;
	/* data. */
	pthread_mutex_init (&pTree->m_mutex, NULL);

	return (VDataTree*)pTree;
}

void vdatatree_destroy (VDataTree* pTree)
{
	vc_free (pTree, sizeof(VDataTree));
	return ;
}

void vdatatree_Lock (void* self)
{
	VDataTree* pTree = (VDataTree*) self;
	pthread_mutex_lock (&pTree->m_mutex);
}
void vdatatree_Unlock (void* self) 
{
	VDataTree* pTree = (VDataTree*) self;
	pthread_mutex_unlock (&pTree->m_mutex);
}





VDataNode* vdatatree_Insert (void* self, comp_ft comp, void* arg)
{
	if (!comp)
	{
		return NULL;
	}
	VDataTree* pTree = (VDataTree*) self;
	VDataNode* node = vdatanode_create (arg);
	if (!node)
	{
		return NULL;
	}
	pTree->size++;

	VDataNode* pNode = pTree->head;
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
			if(comp(pNode->arg, arg)==OK)
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

void vdatatree_Travel (void* self, todo_ft todo)
{
	VDataTree* pTree = (VDataTree*) self;
	if (todo)
	{
		VDataNode* pNode = _vdatatree_MostL (self, pTree->head);
		if (pNode)
		{
			todo (pNode->arg);
		}
		while (pNode = _vdatatree_Next(self, pNode))
		{
			todo (pNode->arg);
		}
	}
	return ;
}

VDataNode* _vdatatree_MostR (void* self, VDataNode* node)
{
	VDataNode* pNode=node;
	if (pNode)
	{
		while (pNode->R)
		{
			pNode = pNode->R;
		}
	}
	return pNode;
}

VDataNode* _vdatatree_MostL (void* self, VDataNode* node)
{
	VDataNode* pNode=node;
	if (pNode)
	{
		while (pNode->L)
		{
			pNode = pNode->L;
		}
	}
	return pNode;
}

VDataNode* _vdatatree_Next (void* self, VDataNode* node)
{
	VDataNode* pNode = node;
	if (!pNode)
	{
		return NULL;
	}

	if (pNode->R)
	{
		return _vdatatree_MostL (self, pNode->R);
	}
	else
	{
		VDataNode* parent = pNode->F;
		while (parent && pNode==parent->R)
		{
			pNode = parent;
			parent = parent->F;
		}
		return parent;
	}
}

VDataNode* _vdatatree_Prev (void* self, VDataNode* node)
{
	VDataNode* pNode=node;
	if (!pNode)
	{
		return NULL;
	}

	if (pNode->L)
	{
		return _vdatatree_MostR (self, pNode->L);
	}
	else
	{
		VDataNode* parent = pNode->F;
		while (parent && pNode==parent->L)
		{
			pNode = parent;
			parent = parent->F;
		}
		return parent;
	}
}

VDataNode* vdatatree_Search (void* self, comp_ft equal, void* arg)
{
	VDataTree* pTree = (VDataTree*) self;
	VDataNode* pNode=pTree->head;
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

int _vdatatree_IsLorR (void* self, VDataNode* node)
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

void vdatatree_Delete (void* self, VDataNode* node, dtor_ft del)
{
	VDataTree* pTree = (VDataTree*) self;
	VDataNode* pNode = node;
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
			if (_vdatatree_IsLorR(self, pNode)<0)
			{
				pNode->F->L = NULL;
			}
			else if (_vdatatree_IsLorR(self, pNode)>0)
			{
				pNode->F->R = NULL;
			}
			else if (_vdatatree_IsLorR(self, pNode)==0)
			{
				pTree->head = NULL;
			}
			vdatanode_destroy (pNode);
			pNode = NULL;
		}
		else if (pNode->L && pNode->R)
		{
			/* 2 children. */
			VDataNode* pCandidate = _vdatatree_MostR (self, pNode->L);
			pNode->arg = pCandidate->arg;
			pNode = pCandidate;
		}
		else
		{
			/* 1 child. */
			if (pNode->L)
			{
				if (_vdatatree_IsLorR(self, pNode)<0)
				{
					pNode->F->L = pNode->L;
					pNode->L->F = pNode->F;
				}
				else if (_vdatatree_IsLorR(self, pNode)>0)
				{
					pNode->F->R = pNode->L;
					pNode->L->F = pNode->F;
				}
				else if (_vdatatree_IsLorR(self, pNode)==0)
				{
					pTree->head = pNode->L;
					pNode->L->F = NULL;
				}
			}
			else if (pNode->R)
			{
				if (_vdatatree_IsLorR(self, pNode)<0)
				{
					pNode->F->L = pNode->R;
					pNode->R->F = pNode->F;
				}
				else if (_vdatatree_IsLorR(self, pNode)>0)
				{
					pNode->F->R = pNode->R;
					pNode->R->F = pNode->F;
				}
				else if (_vdatatree_IsLorR(self, pNode)==0)
				{
					pTree->head = pNode->R;
					pNode->R->F = NULL;
				}
			}
			vdatanode_destroy (pNode);
			pNode = NULL;
		}
	}

	if (del)
	{
		del(pArg);
	}
	return ;
}


void vdatatree_Seek (void* self, POS_e from)
{
	VDataTree* pTree = (VDataTree*) self;
	VDataNode* pNode = NULL;

	if (from==MIN)
	{
		pNode = _vdatatree_MostL (self, pTree->head);
		pTree->cur = pNode;
	}
	else if (from==MAX)
	{
		pNode = _vdatatree_MostR (self, pTree->head);
		pTree->cur = pNode;
	}
	return ;
}

VDataNode* vdatatree_Foreach (void* self, POS_e from, comp_ft filter, void* arg)
{
	VDataTree* pTree = (VDataTree*) self;
	VDataNode* pNode = NULL;
	
	if (filter && arg)
	{
		while (pNode = pTree->cur)
		{
			if (from == MIN)
			{
				pTree->cur = _vdatatree_Next(self, pNode);
			}
			else if (from == MAX)
			{
				pTree->cur = _vdatatree_Prev(self, pNode);
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
		if (pNode = pTree->cur)
		{
			if (from == MIN)
			{
				pTree->cur = _vdatatree_Next(self, pNode);
			}
			else if (from == MAX)
			{
				pTree->cur = _vdatatree_Prev(self, pNode);
			}
			else
			{
				pTree->cur = NULL;
			}
		}
	}
	return pNode;
}








