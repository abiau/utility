

#ifndef __VPARSER_H__
#define __VPARSER_H__

#include "vDefine.h"
#include "vCommon.h"



/* VParser */
typedef struct {
	/* method. */
	/* data. */
	vmutex_t         mutex;
} VParser;


VParser* vparser_create   ();
void   vparser_destroy  (VParser* pParser);


#endif  /* __VPARSER_H__ */
