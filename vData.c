
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

VDataNode* vdatalist_Insert (void* self, POS_e from, comparator_ft comp, void* arg)
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
		VDataNode* p = _vdatalist_SearchFalse (pList, from, comp, arg);

			if (from==HEAD)
			{
				if (p)
				{
					if (p->prev)
					{
						/*  ...qNp...  */
						node->prev = p->prev;
						node->next = p;
						p->prev->next = node;
						p->prev = node;
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
				if (p)
				{
					if (p->next)
					{
						/*  ...pNq...  */
						node->next = p->next;
						node->prev = p;
						p->next->prev = node;
						p->next = node;
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

VDataNode* _vdatalist_SearchFalse (void* self, POS_e from, comparator_ft comp, void* arg)
{
	VDataList* pList = (VDataList*) self;
	if (!comp)
	{
		return NULL;
	}
	VDataNode* p;
	if (from==HEAD)
	{
		p = pList->head;
	}
	else if (from==TAIL)
	{
		p = pList->tail;
	}
	while (p)
	{
		if (comp(p->arg, arg)==OK)
		{
			if (from==HEAD)
			{
				p = p->next;
			}
			else if (from==TAIL)
			{
				p = p->prev;
			}
		}
		else
		{
			break;
		}
	}
	return p;
}

VDataNode* vdatalist_Search (void* self, POS_e from, comparator_ft equal, void* arg)
{
	VDataList* pList = (VDataList*) self;
	if (!equal)
	{
		return NULL;
	}
	
	VDataNode* p;
	if (from==HEAD)
	{
		p = pList->head;
	}
	else if (from==TAIL)
	{
		p = pList->tail;
	}

	while (p)
	{
		if (equal(p->arg, arg)==OK)
		{
			break;
		}
		else
		{
			if (from==HEAD)
			{
				p = p->next;
			}
			else if (from==TAIL)
			{
				p = p->prev;
			}
		}
	}

	return p;
}

void vdatalist_Delete (void* self, VDataNode* node, destructor_ft del)
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

VDataNode* vdatalist_Foreach (void* self, POS_e from, comparator_ft filter, void* arg)
{
	VDataList* pList = (VDataList*) self;
	VDataNode* p;

	if (filter && arg)
	{
		while (p = pList->cur)
		{
			if (from == HEAD)
			{
				pList->cur = pList->cur->next;
			}
			else
			{
				pList->cur = pList->cur->prev;
			}
			if (filter(p->arg, arg)==OK)
			{
				break;
			}
		}
		return p;
	}
	else
	{
		if (p = pList->cur)
		{
			if (from == HEAD)
			{
				pList->cur = pList->cur->next;
			}
			else
			{
				pList->cur = pList->cur->prev;
			}
		}
		return p;
	}

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





VDataNode* vdatatree_Insert (void* self, comparator_ft comp, void* arg)
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

	VDataNode* p = pTree->head;
	if (!p)
	{
		pTree->head = node;
	}
	else
	{
		while (p)
		{
			/* |                            |
			 * |            Node            |
			 * |          /      \          |
			 * |     L:False     R:True     |
			 * |                            |
			 */
			if(comp(p->arg, arg)==OK)
			{
				if (!p->R)
				{
					p->R = node;
					node->P = p;
					break;
				}
				else
				{
					p = p->R;
					continue;
				}
			}
			else
			{
				if (!p->L)
				{
					p->L = node;
					node->P = p;
					break;
				}
				else
				{
					p = p->L;
					continue;
				}
			}
		}
	}
	return node;
}

void vdatatree_Travel (void* self, VDataNode* node, todo_ft todo)
{
	VDataTree* pTree = (VDataTree*) self;

	if (node && todo)
	{
		vdatatree_Travel (pTree, node->L, todo);
		todo (node->arg);
		vdatatree_Travel (pTree, node->R, todo);
	}
	return ;
}

VDataNode* vdatatree_TravelFind (void* self, VDataNode* node, comparator_ft equal, void* arg)
{
	VDataTree* pTree = (VDataTree*) self;
	if (!node)
	{
		return NULL;
	}
	if (equal)
	{
		if (equal (node->arg, arg)==OK)
		{
			return node;
		}
		else
		{
			VDataNode* p=NULL;
			p = vdatatree_TravelFind (pTree, node->L, equal, arg);
			if (p)
			{
				return p;
			}
			p = vdatatree_TravelFind (pTree, node->R, equal, arg);
			if (p)
			{
				return p;
			}
		}
	}
	return NULL;
}

VDataNode* vdatatree_Search (void* self, comparator_ft comp, void* arg)
{
	if (!comp)
	{
		return NULL;
	}
	VDataTree* pTree = (VDataTree*) self;
	VDataNode* p = pTree->head;
	while (p)
	{
		break;
	}
	return p;
}

void vdatatree_Delete (void* self, VDataNode* node, destructor_ft del)
{
	VDataTree* pTree = (VDataTree*) self;
	if (node)
	{
		if (del)
		{
			del(node->arg);
		}
		vdatanode_destroy (node);
	}
	return ;
}










