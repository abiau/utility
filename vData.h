#ifndef __VDATE_H__
#define __VDATE_H__

#include "vDefine.h"
#include "vCommon.h"


/*************************************************************************************/
/*************************************************************************************/


typedef int(*comp_ft)(void* item, void* arg);
typedef int(*dtor_ft)(void* arg);
typedef int(*todo_ft)(void* arg);

typedef enum {
	HEAD    = 1,
	TAIL    = 2,
	MOST_L  = 3,
	MOST_R  = 4,
} POS_e;

typedef struct VDataNode{
	u64t    id;
	void*   arg;
	struct  VDataNode* prev;
	struct  VDataNode* next;
	struct  VDataNode* F;
	struct  VDataNode* L;
	struct  VDataNode* R;
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
	void          (*lock)        (void* self);
	void          (*unlock)      (void* self);
	VDataNode*    (*insert)      (void* self, POS_e from, comp_ft less, void* arg);
	VDataNode*    (*search)      (void* self, POS_e from, comp_ft equal, void* arg);
	void          (*delete)      (void* self, VDataNode* node, dtor_ft del);
	void          (*seek)        (void* self, POS_e from);
	VDataNode*    (*foreach)     (void* self, POS_e from, comp_ft filter, void* arg);
    void          (*travel)      (void* self, POS_e from, todo_ft todo);

	/* data. */
	int           size;
	VDataNode*    cur;
	VDataNode*    head;
	VDataNode*    tail;
	vmutex_t      mutex;
} VDataList;


VDataList*    vdatalist_create         (void);
void          vdatalist_destroy        (VDataList* pList);
void          vdatalist_lock           (void* self);
void          vdatalist_unlock         (void* self);

VDataNode*    vdatalist_insert         (void* self, POS_e from, comp_ft less, void* arg);
VDataNode*    vdatalist_search         (void* self, POS_e from, comp_ft equal, void* arg);
void          vdatalist_delete         (void* self, VDataNode* node, dtor_ft del);
void          vdatalist_seek           (void* self, POS_e from);
VDataNode*    vdatalist_foreach        (void* self, POS_e from, comp_ft filter, void* arg);
void          vdatalist_travel         (void* self, POS_e from, todo_ft todo);

void          _vdatalist_seekNode      (void* self, VDataNode* node);
VDataNode*    _vdatalist_searchFalse   (void* self, POS_e from, comp_ft comp, void* arg);
void          _vdatalist_pushNode      (void* self, POS_e to, VDataNode* node);


/*************************************************************************************/
/*************************************************************************************/

typedef struct {
	/* method. */
	void          (*lock)        (void* self);
	void          (*unlock)      (void* self);
	VDataNode*    (*insert)      (void* self, comp_ft less, void* arg);
	VDataNode*    (*search)      (void* self, comp_ft equal, void* arg);
	void          (*delete)      (void* self, VDataNode* node, dtor_ft del);
	void          (*seek)        (void* self, POS_e from);
	VDataNode*    (*foreach)     (void* self, POS_e from, comp_ft filter, void* arg);
    void          (*travel)      (void* self, todo_ft todo);

	/* data. */
	int           size;
	VDataNode*    cur;
	VDataNode*    head;
	vmutex_t      mutex;
} VDataTree;

VDataTree*    vdatatree_create         (void);
void          vdatatree_destroy        (VDataTree* pTree);
void          vdatatree_lock           (void* self);
void          vdatatree_unlock         (void* self);

VDataNode*    vdatatree_insert         (void* self, comp_ft less, void* arg);
VDataNode*    vdatatree_search         (void* self, comp_ft equal, void* arg);
void          vdatatree_delete         (void* self, VDataNode* node, dtor_ft del);
void          vdatatree_travel         (void* self, todo_ft todo);
void          vdatatree_seek           (void* self, POS_e from);
VDataNode*    vdatatree_foreach        (void* self, POS_e from, comp_ft filter, void* arg);

void          _vdatatree_seekNode      (void* self, VDataNode* node);
int           _vdatatree_isLorR        (void* self, VDataNode* node);
VDataNode*    _vdatatree_mostL         (void* self, VDataNode* node);
VDataNode*    _vdatatree_mostR         (void* self, VDataNode* node);
VDataNode*    _vdatatree_next          (void* self, VDataNode* node);
VDataNode*    _vdatatree_prev          (void* self, VDataNode* node);

#endif  /* __VDATE_H__ */


//VDataList* pList = (VDataList*) self;
//VDataTree* pTree = (VDataTree*) self;
//VDataNode* pNode = node;
  
  
