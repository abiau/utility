#ifndef __VDEFINE_H__
#define __VDEFINE_H__

#include <pthread.h>

typedef unsigned long long  u64t;
typedef          long long  n64t;
typedef unsigned int        u32t;
typedef          int        n32t;
typedef pthread_mutex_t     vmutex_t;

typedef void (*void_fn)();
typedef  int (*i_fn)();



/* Error Code Definition. */
#define OK          (   0)
#define FAIL        (  -1)
#define GG          ( -56) 
#define LEN32       (  32) 
#define LEN64       (  64) 
#define LEN128      ( 128) 
#define LEN256      ( 256) 
#define LEN512      ( 512) 
#define LEN1024     (1024) 
#define LEN2048     (2048) 
#define LEN4096     (4096) 
#define LEN8192     (8192) 


/* Macro Definition. */
#define max(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define min(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
#define DDD      do{printf("@@@ %s(),%d \n", __func__, __LINE__);}while(0)



/* Color. */
#define DDLINE "@@@ %s(),%d"

#define CCr  "\033[0;31m"
#define CCg  "\033[0;32m"
#define CCy  "\033[0;33m" 
#define CCb  "\033[0;34m"
#define CCp  "\033[0;35m"
#define CCc  "\033[0;36m"
#define CCw  "\033[0;37m"

#define CCR  "\033[1;31m"
#define CCG  "\033[1;32m"
#define CCY  "\033[1;33m"
#define CCB  "\033[1;34m"
#define CCP  "\033[1;35m"
#define CCC  "\033[1;36m"
#define CCW  "\033[1;37m"

#define BBr  "\033[41m"
#define BBg  "\033[42m"
#define BBy  "\033[43m"
#define BBb  "\033[44m"
#define BBp  "\033[45m"
#define BBc  "\033[46m"
#define BBw  "\033[47m"

#define CCe  "\033[m"

#define EEE  do{printf(CCe);}while(0)

#define DDr  do{printf(CCr  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)
#define DDg  do{printf(CCg  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)
#define DDy  do{printf(CCy  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)
#define DDb  do{printf(CCb  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)
#define DDp  do{printf(CCm  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)
#define DDc  do{printf(CCc  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)
#define DDw  do{printf(CCw  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)

#define DDR  do{printf(CCR  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)
#define DDG  do{printf(CCG  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)
#define DDY  do{printf(CCY  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)
#define DDB  do{printf(CCB  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)
#define DDP  do{printf(CCM  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)
#define DDC  do{printf(CCC  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)
#define DDW  do{printf(CCW  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)

#define EEr  do{printf(CCr);}while(0)  //red
#define EEg  do{printf(CCg);}while(0)  //green
#define EEy  do{printf(CCy);}while(0)  //yellow
#define EEb  do{printf(CCb);}while(0)  //blue
#define EEp  do{printf(CCm);}while(0)  //purple
#define EEc  do{printf(CCc);}while(0)  //green-blue
#define EEw  do{printf(CCw);}while(0)  //white

#define EER  do{printf(CCR);}while(0)
#define EEG  do{printf(CCG);}while(0)
#define EEY  do{printf(CCY);}while(0)
#define EEB  do{printf(CCB);}while(0)
#define EEP  do{printf(CCM);}while(0)
#define EEC  do{printf(CCC);}while(0)
#define EEW  do{printf(CCW);}while(0)


#endif  /* __VDEFINE_H__ */

