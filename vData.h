#ifndef __VDATE_H__
#define __VDATE_H__

#include "vDefine.h"
#include "vCommon.h"
#include "vLock.h"


/*************************************************************************************/
/*************************************************************************************/

#define VDATA_BASE \
	struct {\
		void          (*travel)      (void* self, todo_ft todo);\
		VDataNode*    (*insert)      (void* self, comp_ft less, void* arg);\
		VDataNode*    (*search)      (void* self, comp_ft equal, void* arg);\
		void          (*delete)      (void* self, VDataNode* node, dtor_ft del);\
		void          (*seek)        (void* self);\
		VDataNode*    (*foreach)     (void* self, comp_ft filter, void* arg);\
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

typedef struct VDataNode{
	u64t    id;
	void*   arg;
	struct  VDataNode* F;
	struct  VDataNode* L;
	struct  VDataNode* R;
	struct  VDataNode* prev;
	struct  VDataNode* next;
} VDataNode;

int*          int_new               (int n);
void          int_del               (int* p);
u64t          vdatanode_newId       (void);
VDataNode*    vdatanode_create      (void* arg);
void          vdatanode_destroy     (VDataNode* node);

/*************************************************************************************/
/*************************************************************************************/

typedef struct {
	/* method. */
	VLOCK_BASE;
	VDATA_BASE;

	/* data. */
	VLock*        pLock;
	int           size;
	VDataNode*    cur;
	VDataNode*    head;
	VDataNode*    tail;
} VDataList;


VDataList*    vdatalist_create         (void);
void          vdatalist_destroy        (VDataList* pList);
void          vdatalist_lock           (void* self);
void          vdatalist_unlock         (void* self);

void          vdatalist_travel         (void* self, todo_ft todo);
VDataNode*    vdatalist_insert         (void* self, comp_ft less, void* arg);
VDataNode*    vdatalist_search         (void* self, comp_ft equal, void* arg);
void          vdatalist_delete         (void* self, VDataNode* node, dtor_ft del);
void          vdatalist_seek           (void* self);
VDataNode*    vdatalist_foreach        (void* self, comp_ft filter, void* arg);

VDataNode*    vdatalist_insertFrom    (void* self, POS_e from, comp_ft less, void* arg);
VDataNode*    vdatalist_searchFrom    (void* self, POS_e from, comp_ft equal, void* arg);
void          vdatalist_seekFrom      (void* self, POS_e from);
void          vdatalist_seekNode      (void* self, VDataNode* node);
VDataNode*    vdatalist_foreachFrom   (void* self, POS_e from, comp_ft filter, void* arg);

VDataNode*    _vdatalist_searchFalse   (void* self, POS_e from, comp_ft comp, void* arg);
void          _vdatalist_pushNode      (void* self, POS_e to, VDataNode* node);


/*************************************************************************************/
/*************************************************************************************/

typedef struct {
	/* method. */
	VLOCK_BASE;
	VDATA_BASE;

	/* data. */
	VLock*        pLock;
	int           size;
	VDataNode*    cur;
	VDataNode*    head;
} VDataTree;

VDataTree*    vdatatree_create         (void);
void          vdatatree_destroy        (VDataTree* pTree);
void          vdatatree_lock           (void* self);
void          vdatatree_unlock         (void* self);

void          vdatatree_travel         (void* self, todo_ft todo);
VDataNode*    vdatatree_insert         (void* self, comp_ft less, void* arg);
VDataNode*    vdatatree_search         (void* self, comp_ft equal, void* arg);
void          vdatatree_delete         (void* self, VDataNode* node, dtor_ft del);
void          vdatatree_seek           (void* self);
VDataNode*    vdatatree_foreach        (void* self, comp_ft filter, void* arg);

void          vdatatree_seekFrom      (void* self, POS_e from);
void          vdatatree_seekNode      (void* self, VDataNode* node);
VDataNode*    vdatatree_foreachFrom   (void* self, POS_e from, comp_ft filter, void* arg);

int           _vdatatree_isLorR        (void* self, VDataNode* node);
VDataNode*    _vdatatree_mostL         (void* self, VDataNode* node);
VDataNode*    _vdatatree_mostR         (void* self, VDataNode* node);
VDataNode*    _vdatatree_next          (void* self, VDataNode* node);
VDataNode*    _vdatatree_prev          (void* self, VDataNode* node);

#endif  /* __VDATE_H__ */


//VDataList* pList = (VDataList*) self;
//VDataTree* pTree = (VDataTree*) self;
//VDataNode* pNode = node;
  
  
