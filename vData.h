#ifndef __VDATE_H__
#define __VDATE_H__

#include "vDefine.h"
#include "vCommon.h"


/*************************************************************************************/
/*************************************************************************************/


typedef int(*comp_ft)(void* item, void* arg);
typedef int(*dtor_ft)(void* arg);
typedef int(*todo_ft)(void* arg);

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

typedef enum {
	HEAD    = 1,
	TAIL    = 2,
	MOST_L  = 3,
	MOST_R  = 4,
} POS_e;

typedef struct {
	/* method. */
	void          (*lock)        (void* self);
	void          (*unlock)      (void* self);
	VDataNode*    (*insert)      (void* self, POS_e from, comp_ft less, void* arg);
	VDataNode*    (*search)      (void* self, POS_e from, comp_ft equal, void* arg);
	void          (*delete)      (void* self, VDataNode* node, dtor_ft del);
	void          (*seek)        (void* self, VDataNode* node);
	VDataNode*    (*foreach)     (void* self, POS_e from, comp_ft filter, void* arg);
    void          (*travel)      (void* self, POS_e from, todo_ft todo);

	/* data. */
	int           size;
	VDataNode*    cur;
	VDataNode*    head;
	VDataNode*    tail;
	vmutex_t      mutex;
} VDataList;


VDataList*    vdatalist_create      (void);
void          vdatalist_destroy     (VDataList* pList);
void          vdatalist_Lock        (void* self);
void          vdatalist_Unlock      (void* self);

VDataNode*    vdatalist_Insert      (void* self, POS_e from, comp_ft less, void* arg);
VDataNode*    vdatalist_Search      (void* self, POS_e from, comp_ft equal, void* arg);
void          vdatalist_Delete      (void* self, VDataNode* node, dtor_ft del);
void          vdatalist_Seek        (void* self, VDataNode* node);
VDataNode*    vdatalist_Foreach     (void* self, POS_e from, comp_ft filter, void* arg);
void          vdatalist_Travel      (void* self, POS_e from, todo_ft todo);

VDataNode*    _vdatalist_SearchFalse (void* self, POS_e from, comp_ft comp, void* arg);
void          _vdatalist_PushNode   (void* self, POS_e to, VDataNode* node);


/*************************************************************************************/
/*************************************************************************************/

typedef struct {
	/* method. */
	void          (*lock)        (void* self);
	void          (*unlock)      (void* self);
	VDataNode*    (*insert)      (void* self, comp_ft comp, void* arg);
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

VDataTree*    vdatatree_create      (void);
void          vdatatree_destroy     (VDataTree* pTree);
void          vdatatree_Lock        (void* self);
void          vdatatree_Unlock      (void* self);

VDataNode*    vdatatree_Insert      (void* self, comp_ft comp, void* arg);
VDataNode*    vdatatree_Search      (void* self, comp_ft equal, void* arg);
void          vdatatree_Delete      (void* self, VDataNode* node, dtor_ft del);
void          vdatatree_Travel      (void* self, todo_ft todo);
void          vdatatree_Seek        (void* self, POS_e from);
VDataNode*    vdatatree_Foreach     (void* self, POS_e from, comp_ft filter, void* arg);

int           _vdatatree_IsLorR      (void* self, VDataNode* node);
VDataNode*    _vdatatree_MostR       (void* self, VDataNode* node);
VDataNode*    _vdatatree_MostL       (void* self, VDataNode* node);
VDataNode*    _vdatatree_Next        (void* self, VDataNode* node);
VDataNode*    _vdatatree_Prev        (void* self, VDataNode* node);

#endif  /* __VDATE_H__ */


//VDataList* pList = (VDataList*) self;
//VDataTree* pTree = (VDataTree*) self;
//VDataNode* pNode = node;
  
  
