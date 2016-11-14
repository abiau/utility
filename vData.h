#ifndef __VDATE_H__
#define __VDATE_H__

#include "vDefine.h"
#include "vCommon.h"
#include "vLock.h"


/*************************************************************************************/
/*************************************************************************************/

#define v_BASE \
	struct {\
		void      (*travel)      (void* self, todo_ft todo);\
		VNode*    (*insert)      (void* self, comp_ft less, void* arg);\
		VNode*    (*search)      (void* self, comp_ft equal, void* arg);\
		void      (*delete)      (void* self, VNode* node, dtor_ft del);\
		void      (*seek)        (void* self);\
		VNode*    (*foreach)     (void* self, comp_ft filter, void* arg);\
	}; 

/*************************************************************************************/
/*************************************************************************************/

typedef int(*comp_ft)(void* item, void* arg);
typedef int(*dtor_ft)(void* arg);
typedef int(*todo_ft)(void* arg);

typedef enum {
	BEGIN   = 1,
	END     = 2,
} POS_e;

/*************************************************************************************/
/*************************************************************************************/

typedef struct VNode{
	u64t    id;
	void*   arg;
	struct  VNode* F;
	struct  VNode* L;
	struct  VNode* R;
	struct  VNode* prev;
	struct  VNode* next;
} VNode;

int*      int_new           (int n);
void      int_del           (int* p);
u64t      vnode_newId       (void);
VNode*    vnode_create      (void* arg);
void      vnode_destroy     (VNode* node);

/*************************************************************************************/
/*************************************************************************************/

typedef struct {
	/* method. */
	VLOCK_BASE;
	v_BASE;

	/* data. */
	VLock*    Lock;
	int       size;
	VNode*    cur;
	VNode*    head;
	VNode*    tail;
} VList;


VList*    vlist_create         (void);
void      vlist_destroy        (VList* pList);
void      vlist_lock           (void* self);
void      vlist_unlock         (void* self);

void      vlist_travel         (void* self, todo_ft todo);
VNode*    vlist_insert         (void* self, comp_ft less, void* arg);
VNode*    vlist_search         (void* self, comp_ft equal, void* arg);
void      vlist_delete         (void* self, VNode* node, dtor_ft del);
void      vlist_seek           (void* self);
VNode*    vlist_foreach        (void* self, comp_ft filter, void* arg);

VNode*    vlist_insertFrom     (void* self, POS_e from, comp_ft less, void* arg);
VNode*    vlist_searchFrom     (void* self, POS_e from, comp_ft equal, void* arg);
void      vlist_seekFrom       (void* self, POS_e from);
void      vlist_seekNode       (void* self, VNode* node);
VNode*    vlist_foreachFrom    (void* self, POS_e from, comp_ft filter, void* arg);

VNode*    _vlist_searchFalse   (void* self, POS_e from, comp_ft comp, void* arg);
void      _vlist_pushNode      (void* self, POS_e to, VNode* node);


/*************************************************************************************/
/*************************************************************************************/

typedef struct {
	/* method. */
	VLOCK_BASE;
	v_BASE;

	/* data. */
	VLock*    Lock;
	int       size;
	VNode*    cur;
	VNode*    head;
} VTree;

VTree*    vtree_create         (void);
void      vtree_destroy        (VTree* pTree);
void      vtree_lock           (void* self);
void      vtree_unlock         (void* self);

void      vtree_travel         (void* self, todo_ft todo);
VNode*    vtree_insert         (void* self, comp_ft less, void* arg);
VNode*    vtree_search         (void* self, comp_ft equal, void* arg);
void      vtree_delete         (void* self, VNode* node, dtor_ft del);
void      vtree_seek           (void* self);
VNode*    vtree_foreach        (void* self, comp_ft filter, void* arg);

void      vtree_seekFrom       (void* self, POS_e from);
void      vtree_seekNode       (void* self, VNode* node);
VNode*    vtree_foreachFrom    (void* self, POS_e from, comp_ft filter, void* arg);

int       _vtree_isLorR        (void* self, VNode* node);
VNode*    _vtree_mostL         (void* self, VNode* node);
VNode*    _vtree_mostR         (void* self, VNode* node);
VNode*    _vtree_next          (void* self, VNode* node);
VNode*    _vtree_prev          (void* self, VNode* node);

#endif  /* __VDATE_H__ */


//VList* pList = (VList*) self;
//VTree* pTree = (VTree*) self;
//VNode* pNode = node;
  
  
