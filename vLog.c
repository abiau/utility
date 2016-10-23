
#include "vLog.h"

VLog* LOG_ERRNO=NULL;

/***********************************************************************************/
/***********************************************************************************/

VLog*             vlog_create   (int mode, char* file, char* folder, char* fmtScreen, char* fmtFile);
void                 vlog_destroy  (VLog* pLog);
static void          vlog_print    (void* self, const char* szFunc, int nLine, ...);
static int           _vlog_nDigit  (const char* str);
static int           _vlog_WriteToFile       (char* path, char* buf);
static char*         _vlog_FmtToString       (char* out, int len, const char* fmt, u64t ts, const char* szFunc, int nLine, va_list ap);
static char*         _vlog_FmtToStringXColor (char* str, int size);

VTimer*           vtimer_create    ();
void                 vtimer_destroy   (VTimer* pTimer);
static void          vtimer_resume    (void* self);
static u64t          vtimer_diffms    (void* self);
static u64t          vtimer_diffus    (void* self);
static u64t          vtimer_now       (void);
static char*         vtimer_nowString (char* buf, int bufLen, char* fmt);
static char*         vtimer_tsString  (char* buf, int bufLen, char* fmt, u64t ts);
static DateTime   _vtimer_TS_to_DT (u64t ts);


/***********************************************************************************/
/***********************************************************************************/

VTimer* vtimer_create  ()
{
	VTimer* p = vc_malloc (sizeof(VTimer)); 
	if (p==NULL)
	{
		return NULL;
	}

	memset (p, 0, sizeof(VTimer));
	p->resume    = vtimer_resume;
	p->diffus    = vtimer_diffus;
	p->diffms    = vtimer_diffms;
	p->now       = vtimer_now;
	p->nowString = vtimer_nowString;
	p->tsString  = vtimer_tsString;
	pthread_mutex_init (&p->m_mutex, NULL);

	return (VTimer*)p;
}

void vtimer_destroy (VTimer* pTimer)
{
	vc_free (pTimer, sizeof(VTimer));
	return ;
}


static u64t vtimer_now (void)
{
	struct timeval tv;

	gettimeofday (&tv, NULL);
	u64t ms = (tv.tv_sec)*1000 + (tv.tv_usec)/1000 ;

	return ms;
}

static char* vtimer_nowString (char* buf, int bufLen, char* fmt)
{
	u64t ts = vtimer_now ();

	return _vlog_FmtToString (buf, bufLen, fmt, ts, NULL, 0, NULL);
}

static char* vtimer_tsString (char* buf, int bufLen, char* fmt, u64t ts)
{
	return _vlog_FmtToString (buf, bufLen, fmt, ts, NULL, 0, NULL);
}

static DateTime _vtimer_TS_to_DT (u64t ts)
{
    DateTime  DT;
	u64t        sec_u64   = ts/1000;
	time_t       sec_timet = sec_u64;
	struct tm    stTimeInfo;

	gmtime_r (&sec_timet, &stTimeInfo);
	DT.ts    = ts;
	DT.yy    = stTimeInfo.tm_year +1900;
	DT.mm    = stTimeInfo.tm_mon +1;
	DT.dd    = stTimeInfo.tm_mday;
	DT.h     = stTimeInfo.tm_hour;
	DT.m     = stTimeInfo.tm_min;
	DT.s     = stTimeInfo.tm_sec;

	return DT;
}


static void vtimer_resume (void* self)
{
	VTimer* p = (VTimer*) self;
    gettimeofday (&p->m_tv , NULL);
	return ;
}

static u64t vtimer_diffms (void* self)
{
	VTimer* p = (VTimer*) self;
	struct timeval  now;
    u64t           sec=0;
    u64t           usec=0;

    gettimeofday (&now, NULL);
    sec = (now.tv_sec) - ((p->m_tv).tv_sec);
    usec = (now.tv_usec) - ((p->m_tv).tv_usec);
    if ( usec <0 )
	{
        sec--;
        usec += 1000000;
    }
    u64t totalMsec = (usec + (sec*1000000)) >> 10;  // u.sec change to m.sec
    return totalMsec;
}

static u64t vtimer_diffus (void* self)
{
	VTimer* p = (VTimer*) self;
	struct timeval  now;
    u64t           sec=0;
    u64t           usec=0;

    gettimeofday (&now, NULL);
    sec = (now.tv_sec) - ((p->m_tv).tv_sec);
    usec = (now.tv_usec) - ((p->m_tv).tv_usec);
    if ( usec <0 )
	{
        sec--;
        usec += 1000000;
    }
    u64t totalUsec = (usec + (sec*1000000)) ;  // u.sec change to m.sec
    
	return totalUsec;
}





/***********************************************************************************/
/***********************************************************************************/

VLog* vlog_create  (int mode, char* file, char* folder, char* fmtScreen, char* fmtFile)
{
	if (file==NULL   && strlen(file)==0)
	{
		return NULL;
	}
	if (folder==NULL && strlen(folder)==0)
	{
		return NULL;
	}

	const char* DFL_fmtScreen = "V";
	const char* DFL_fmtFile   = "V";

	VLog* p = vc_malloc (sizeof(VLog)); 
	if (p==NULL)
	{
		return NULL;
	}
	memset (p, 0, sizeof(VLog));
	p->m_mode = mode;
	p->print  = vlog_print;
	pthread_mutex_init (&p->m_mutex, NULL);
	if (fmtScreen==NULL || strlen(fmtScreen)==0)
	{
		memcpy (p->m_fmtScreen, DFL_fmtScreen, strlen(DFL_fmtScreen));
	}
	else
	{
		memcpy (p->m_fmtScreen, fmtScreen, strlen(fmtScreen));
	}
	if (fmtFile==NULL || strlen(fmtFile)==0)
	{
		memcpy (p->m_fmtFile, DFL_fmtFile, strlen(DFL_fmtFile));
	}
	else
	{
		memcpy (p->m_fmtFile, fmtFile, strlen(fmtFile));
	}
	fd_mkdir (folder);
	snprintf (p->m_path, sizeof(p->m_path), "%s/%s", folder, file);


	return (VLog*)p;
}



void vlog_destroy (VLog* pLog)
{
	VLog*  p = (VLog*) pLog;
	vc_free (p, sizeof(VLog));
	return;
}



/***********************************************************************************/
/***********************************************************************************/


static void vlog_print (void* self, const char* szFunc, int nLine, ...)
{
	VLog*  p = (VLog*) self;
	va_list  ap;
	
	pthread_mutex_lock (&p->m_mutex);
	{
		if (p->m_mode & 0x0003)
		{
			u64t ts = vtimer_now ();
			va_start (ap, nLine);
			_vlog_FmtToString (p->m_buf, sizeof(p->m_buf), p->m_fmtScreen, ts, szFunc, nLine, ap);
			va_end (ap);
			if (p->m_mode & 0x0001)
			{
				printf ("%s", p->m_buf);
			}
			if (p->m_mode & 0x0002)
			{
				_vlog_FmtToStringXColor (p->m_buf, sizeof(p->m_buf));
				_vlog_WriteToFile (p->m_path, p->m_buf);
			}
		}
	}
	pthread_mutex_unlock (&p->m_mutex);

	return;
}

static int _vlog_WriteToFile (char* path, char* buf)
{
	int fd = fd_open (path);
	if (fd>=0)
	{
		ssize_t size = write (fd, buf, strlen(buf));
		if (size<0)
		{
			/*ERROR*/;
		}
		fd_close (fd);
		return size;
	}
	else
	{
		return fd;
	}
}
/***********************************************************************************/
/***********************************************************************************/


static int _vlog_nDigit (const char* str)
{
	int i=0;
	while ( isdigit(*(str+i)) != 0 )
	{
		i++;
	}
	return i;
}


static char* _vlog_FmtToStringXColor (char* src, int size)
{
	const char* Color    = "\033[";
	const char* ColorEnd = "\033[m";
	char*       pRead;
	char*       pReadNext;
	char*       pWrite;
	int         lenSrc = strlen(src);
	int         lenColor = strlen(Color);
	int         i;
	int         ret;
	int         offset;
	
	pRead = strstr(src, Color);
	if (pRead==NULL)
	{
		return src;
	}
	pWrite = pRead;
	while (*pRead != '\0')
	{
		pRead = pRead + lenColor;
		while (1)
		{
			if (*(pRead)=='m')
			{
				pRead++;
				break;
			}

			if (pRead>=(src+size-1))
			{
				break;
			}

			pRead++;
		}
		pReadNext = strstr(pRead, Color);
		offset = (pReadNext) ? (pReadNext - pRead) : strlen(pRead);
		for (i=0; i<offset; i++)
		{
			*(pWrite) = *(pRead);
			pWrite++;
			pRead++;
		}
		*(pWrite) = '\0';
	}

	return src;
}

static char* _vlog_FmtToString (char* buf, int bufLen, const char* fmt, u64t ts, const char* szFunc, int nLine, va_list ap)
{
	/* fmt=[Y/M/D h:m:s.u3 | F10():L4 | S | X | V] */

	if (buf==NULL || fmt==NULL)
	{
		return NULL;
	}

	char*        p = buf;
	int          space = bufLen;
	int          fmtLen = strlen(fmt);
    DateTime  DT = _vtimer_TS_to_DT (ts);
	int          offset;
	int          i;
	int          nWidth;
	int          nDigit;
	char*        _p;

	memset (buf, 0, bufLen);
	for (i=0; i<fmtLen; i++)
	{
		switch (fmt[i])
		{
		/* User string */
			case 'X':
				if (ap)
				{
					_p=va_arg (ap, char*);
					(void)(_p);
				}
				break;
			case 'V':
				if (ap)
				{
					_p=va_arg (ap, char*);
					vsnprintf (p, space, _p, ap);
				}
				break;
			case 'S':
				if (ap)
				{
					nWidth = atoi (fmt+i+1);
					nDigit = _vlog_nDigit (fmt+i+1);
					i = i + nDigit;
					_p=va_arg (ap, char*);
					snprintf (p, space, "%*s", nWidth, _p);
				}
				break;
		/* Func & Line */
			case 'F':
				nWidth = atoi (fmt+i+1);
				nDigit = _vlog_nDigit (fmt+i+1);
				i = i + nDigit;
				snprintf (p, space, "%*s", nWidth, szFunc);
				break;
			case 'L':
				nWidth = atoi (fmt+i+1);
				nDigit = _vlog_nDigit (fmt+i+1);
				i = i + nDigit;
				snprintf (p, space, "%*d", nWidth, nLine);
				break;
		/* Time */
			case 'Y':
				snprintf (p, space, "%04d", DT.yy);
				break;
			case 'M':
				snprintf (p, space, "%02d", DT.mm);
				break;
			case 'D':
				snprintf (p, space, "%02d", DT.dd);
				break;
			case 'h':
				snprintf (p, space, "%02d", DT.h);
				break;
			case 'm':
				snprintf (p, space, "%02d", DT.m);
				break;
			case 's':
				snprintf (p, space, "%02d", DT.s);
				break;
			case 'u':
				nWidth = atoi (fmt+i+1);
				nDigit = _vlog_nDigit (fmt+i+1);
				i = i + nDigit;
				if (nWidth==1)
				{
					snprintf (p, space, "%0*llu", nWidth, (ts%1000)/100);
				}
				else if (nWidth==2)
				{
					snprintf (p, space, "%0*llu", nWidth, (ts%1000)/10);
				}
				else
				{
					snprintf (p, space, "%0*llu", 3, (ts%1000));
				}
				break;
			default:
				snprintf (p, space, "%c", fmt[i]);
				break;
		}
		offset = strlen(p);
		space = space - offset; 
		p = p + offset;
	}
	buf[bufLen-1] = '\0';

	return buf;
}


/***********************************************************************************/
/***********************************************************************************/




