#ifndef __VCOMMON_H__
#define __VCOMMON_H__

#include "vDefine.h"

void   vc_msleep          (int msec);
void*  vc_malloc          (int size);
int    vc_free            (void* p, int size);
n64t   vc_getMemUsage     (void);
n64t   vc_addMemUsage     (int size);

int    fd_isFileExist     (const char *path);
n64t   fd_getFileSize     (const char* path);
int    fd_isFolderExist   (const char *folder);
int    fd_mkdir           (const char *folder);

int    fd_open            (const char* path);
int    fd_close           (int fd);
void   fd_Backtrace_file  (const char* path);
void   fd_Backtrace       (char* buf, int len);




#endif  /* __VCOMMON_H__ */


