#ifndef __VDATE_H__
#define __VDATE_H__

#include "vDefine.h"
#include "vCommon.h"


/*************************************************************************************/
/*************************************************************************************/


typedef int(*comparator_ft)(void* a, void* b);
typedef int(*destructor_ft)(void* arg);
typedef int(*todo_ft)(void* arg);

typedef struct VDataNode{
	u64t    id;
	void*   arg;
	struct  VDataNode* prev;
	struct  VDataNode* next;
	struct  VDataNode* P;
	struct  VDataNode* L;
	struct  VDataNode* R;
} VDataNode;

int*          int_new               (int n);
void          int_del               (int* p);
u64t          vdatanode_newId       ();
VDataNode*    vdatanode_create      (void* arg);
void          vdatanode_destroy     (VDataNode* node);


/*************************************************************************************/
/*************************************************************************************/

typedef enum {
	HEAD = 1,
	TAIL = 2,
	DLR  =3, /* pre-order */
	LDR = 4, /* middle-order */
	LRD = 5, /* post-order */
} POS_e;

typedef struct {
	/* method. */
	void          (*lock)        (void* self);
	void          (*unlock)      (void* self);
	VDataNode*    (*insert)      (void* self, POS_e from, comparator_ft comp, void* arg);
	VDataNode*    (*search)      (void* self, POS_e from, comparator_ft equal, void* arg);
	void          (*delete)      (void* self, VDataNode* node,   destructor_ft del);
	void          (*seek)        (void* self, VDataNode* node);
	VDataNode*    (*foreach)     (void* self, POS_e from, comparator_ft filter, void* arg);

	/* data. */
	int size;
	VDataNode* cur;
	VDataNode* head;
	VDataNode* tail;
	vmutex_t   m_mutex;
} VDataList;


VDataList*    vdatalist_create      ();
void          vdatalist_destroy     (VDataList* pList);
void          vdatalist_Lock        (void* self);
void          vdatalist_Unlock      (void* self);

VDataNode*    vdatalist_Insert      (void* self, POS_e from, comparator_ft comp, void* arg);
VDataNode*    vdatalist_Search      (void* self, POS_e from, comparator_ft equal, void* arg);
void          vdatalist_Delete      (void* self, VDataNode* node,   destructor_ft del);
void          vdatalist_Seek        (void* self, VDataNode* node);
VDataNode*    vdatalist_Foreach     (void* self, POS_e from, comparator_ft filter, void* arg);

VDataNode*    _vdatalist_SearchFalse (void* self, POS_e from, comparator_ft comp, void* arg);
void          _vdatalist_PushNode   (void* self, POS_e to, VDataNode* node);


/*************************************************************************************/
/*************************************************************************************/

typedef struct {
	/* method. */
	void          (*lock)        (void* self);
	void          (*unlock)      (void* self);
	VDataNode*    (*insert)      (void* self, comparator_ft comp, void* arg);
	VDataNode*    (*search)      (void* self, comparator_ft comp, void* arg);
	void          (*delete)      (void* self, VDataNode* node, destructor_ft del);

	/* data. */
	int size;
	VDataNode* cur;
	VDataNode* head;
	vmutex_t   m_mutex;
} VDataTree;

VDataTree*    vdatatree_create      ();
void          vdatatree_destroy     (VDataTree* pTree);
void          vdatatree_Lock        (void* self);
void          vdatatree_Unlock      (void* self);

VDataNode*    vdatatree_Insert      (void* self, comparator_ft comp, void* arg);
VDataNode*    vdatatree_Search      (void* self, comparator_ft comp, void* arg);
void          vdatatree_Delete      (void* self, VDataNode* node, destructor_ft del);

void vdatatree_Travel (void* self, VDataNode* node, todo_ft todo);
VDataNode* vdatatree_TravelFind (void* self, VDataNode* node, comparator_ft equal, void* arg);

#endif  /* __VDATE_H__ */


//VDataList* pList = (VDataList*) self;
//VDataTree* pTree = (VDataTree*) self;
  
  
