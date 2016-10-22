#ifndef __VDATE_H__
#define __VDATE_H__

#include "vDefine.h"
#include "vCommon.h"

#if 0
typedef enum {
	VDATA_STACK=1, 
	VDATA_TREE=2, 
	VDATA_LIST=3, 
} VDataType_e;

typedef struct {
	/* method. */
	VDataNode_st* (*insert)  (void* self, validator v, void* arg);
	VDataNode_st* (*search)  (void* self, validator v, void* arg);
	int           (*delete)  (void* self, validator v, void* arg);

	/* data. */
	VDataType_e  m_type;
	vmutex_t      m_mutex;
} VData_st;


VData_st* vdata_create  (VDataType_e type);
void      vdata_destroy (VData_st* pData);
#endif


/*************************************************************************************/
/*************************************************************************************/


typedef int(*validator)(void* a, void* b);

typedef struct VDataNode_st{
	u64t    id;
	void*  arg;
	struct VDataNode_st* prev;
	struct VDataNode_st* next;
} VDataNode_st;

int*          int_new               (int n);
void          int_del               (int* p);
u64t          vdatanode_newId       ();
VDataNode_st* vdatanode_create      (void* arg);
void          vdatanode_destroy     (VDataNode_st* node);


/*************************************************************************************/
/*************************************************************************************/


typedef struct {
	/* method. */
	void          (*lock)        (void* self);
	void          (*unlock)      (void* self);
	VDataNode_st* (*insert)      (void* self, validator v, void* arg);
	VDataNode_st* (*search)      (void* self, validator v, void* arg);
	void          (*delete)      (void* self, VDataNode_st* node, void_fn todo);
	void          (*seek)        (void* self, VDataNode_st* node);
	VDataNode_st* (*foreach)     (void* self);

	/* data. */
	int size;
	VDataNode_st* cur;
	VDataNode_st* head;
	VDataNode_st* tail;
	vmutex_t      m_mutex;
} VDataList_st;


VDataList_st* vdatalist_create      ();
void          vdatalist_destroy     (VDataList_st* pDataList);
void          vdatalist_Lock        (void* self);
void          vdatalist_Unlock      (void* self);

VDataNode_st* vdatalist_Insert      (void* self, validator v, void* arg);
VDataNode_st* vdatalist_Search      (void* self, validator v, void* arg);
void          vdatalist_Delete      (void* self, VDataNode_st* node, void_fn todo);
void          vdatalist_Seek        (void* self, VDataNode_st* node);
VDataNode_st* vdatalist_Foreach     (void* self);




#endif  /* __VDATE_H__ */

//VDataList_st* pList = (VDataList_st*) self;
  
  
