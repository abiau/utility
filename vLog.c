
#include "vLog.h"

VLog* LOG_ERRNO=NULL;

/***********************************************************************************/
/***********************************************************************************/


static int        _vlog_WriteFile       (char* path, char* buf);
static int        _vlog_NeedBackupFile  (void* self, u64t ts);
static int        _vlog_BackupFile      (char* path, u64t ts);

VTimer*           vtimer_create     ();
void              vtimer_destroy    (VTimer* pTimer);
static void       vtimer_resume     (void* self);
static u64t       vtimer_diffms     (void* self);
static u64t       vtimer_diffus     (void* self);
static u64t       vtimer_now        (void);
static char*      vtimer_nowString  (char* buf, int bufLen, char* fmt);
static char*      vtimer_tsString   (char* buf, int bufLen, char* fmt, u64t ts);

static int        _nDigit           (const char* str);
static DateTime   _TS_to_DT         (u64t ts);
static u64t       _STR_to_SIZE      (char* str);
static u64t       _STR_to_MS        (char* str);
static char*      _FMT_to_STR       (char* out, int len, const char* fmt, u64t ts, const char* szFunc, int nLine, va_list ap);
static char*      _FMT_to_STRxColor (char* str, int size);


/***********************************************************************************/
/***********************************************************************************/

VTimer* vtimer_create  ()
{
	VTimer* pTimer = vc_malloc (sizeof(VTimer)); 
	if (pTimer==NULL)
	{
		return NULL;
	}

	memset (pTimer, 0, sizeof(VTimer));
	pTimer->resume    = vtimer_resume;
	pTimer->diffus    = vtimer_diffus;
	pTimer->diffms    = vtimer_diffms;
	pTimer->now       = vtimer_now;
	pTimer->nowStr    = vtimer_nowString;
	pTimer->tsStr     = vtimer_tsString;
	pthread_mutex_init (&pTimer->mutex, NULL);

	return (VTimer*)pTimer;
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

	return _FMT_to_STR (buf, bufLen, fmt, ts, NULL, 0, NULL);
}

static char* vtimer_tsString (char* buf, int bufLen, char* fmt, u64t ts)
{
	return _FMT_to_STR (buf, bufLen, fmt, ts, NULL, 0, NULL);
}


static void vtimer_resume (void* self)
{
	VTimer* pTimer = (VTimer*) self;
    gettimeofday (&pTimer->tv , NULL);
	return ;
}

static u64t vtimer_diffms (void* self)
{
	VTimer* pTimer = (VTimer*) self;
	struct timeval  now;
    u64t           sec=0;
    u64t           usec=0;

    gettimeofday (&now, NULL);
    sec = (now.tv_sec) - ((pTimer->tv).tv_sec);
    usec = (now.tv_usec) - ((pTimer->tv).tv_usec);
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
	VTimer* pTimer = (VTimer*) self;
	struct timeval  now;
    u64t           sec=0;
    u64t           usec=0;

    gettimeofday (&now, NULL);
    sec = (now.tv_sec) - ((pTimer->tv).tv_sec);
    usec = (now.tv_usec) - ((pTimer->tv).tv_usec);
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

VLog* vlog_create  (int mode, char* folder, char* file, char* FmtScreen, char* FmtFile)
{
	if (file==NULL   && strlen(file)==0)
	{
		return NULL;
	}
	if (folder==NULL && strlen(folder)==0)
	{
		return NULL;
	}

	const char* DFL_FmtScreen = "V";
	const char* DFL_FmtFile   = "V";

	VLog* pLog = vc_malloc (sizeof(VLog)); 
	if (pLog==NULL)
	{
		return NULL;
	}
	memset (pLog, 0, sizeof(VLog));

	/* method. */
	pLog->print     = vlog_print;
	pLog->set       = vlog_set;
	pLog->setSize   = vlog_setSize;
	pLog->setPath   = vlog_setPath;
	pLog->setRotate = vlog_setRotate;
	
	/* data. */
	pLog->mode      = mode;
	pthread_mutex_init (&pLog->mutex, NULL);
	if (FmtScreen==NULL || strlen(FmtScreen)==0)
	{
		memcpy (pLog->FmtScreen, DFL_FmtScreen, strlen(DFL_FmtScreen)+1);
	}
	else
	{
		memcpy (pLog->FmtScreen, FmtScreen, strlen(FmtScreen)+1);
	}
	if (FmtFile==NULL || strlen(FmtFile)==0)
	{
		memcpy (pLog->FmtFile, DFL_FmtFile, strlen(DFL_FmtFile)+1);
	}
	else
	{
		memcpy (pLog->FmtFile, FmtFile, strlen(FmtFile)+1);
	}
	vlog_setPath   (pLog, folder, file);
	vlog_setSize   (pLog, "1G");
	vlog_setRotate (pLog, "U");
	
	if (fd_isFileExist (pLog->path))
	{
		pLog->nFileSize = fd_getFileSize (pLog->path);
	}

	return (VLog*)pLog;
}



void vlog_destroy (VLog* pLog)
{
	vc_free (pLog, sizeof(VLog));
	return;
}



/***********************************************************************************/
/***********************************************************************************/

void vlog_set (void* self, SetVLogType_e type, ...)
{
	VLog* pLog = (VLog*) self;
	va_list ap;
	char*   file;
	char*   folder;
	char*   str;
	
	va_start (ap, type);
	switch (type)
	{
	case PATH:
		folder=va_arg (ap, char*);
		file  =va_arg (ap, char*);
		vlog_setPath (pLog, folder, file);
		break;
	case SIZE:
		str  =va_arg (ap, char*);
		vlog_setSize (pLog, str);
		break;
	case ROTATE:
		str  =va_arg (ap, char*);
		vlog_setRotate (pLog, str);
		break;
	default:
		break;
	}
	va_end (ap);

	return ;
}

void vlog_setSize (void* self, char* str)
{
	VLog* pLog = (VLog*) self;

	pthread_mutex_lock (&pLog->mutex);
	pLog->MaxFileSize = _STR_to_SIZE(str);
	if (pLog->MaxFileSize <= (1<<10))
	{
		pLog->MaxFileSize = 1 << 30;

	}
	pthread_mutex_unlock (&pLog->mutex);

	return ;
}

void vlog_setRotate (void* self, char* str)
{
	VLog* pLog = (VLog*) self;
	if (!str || strlen(str)==0)
	{
		return ;
	}
	pthread_mutex_lock (&pLog->mutex);
	pLog->MaxRotateMs = _STR_to_MS (str);
	pthread_mutex_unlock (&pLog->mutex);
	return ;
}

void vlog_setPath (void* self, char* folder, char* file)
{
	VLog* pLog = (VLog*) self;
	vzero  (pLog->folder, sizeof(pLog->folder));
	vzero  (pLog->path,   sizeof(pLog->path));

	pthread_mutex_lock (&pLog->mutex);
	if ((!folder)||strlen(folder)==0)
	{
		snprintf (pLog->folder, sizeof(pLog->folder), "./");
	}
	else
	{
		snprintf (pLog->folder, sizeof(pLog->folder), "%s", folder);
	}
	if ((!file)||strlen(file)==0)
	{
		snprintf (pLog->path,   sizeof(pLog->path),   "%s/%s", pLog->folder, "out.log");
	}
	else
	{
		snprintf (pLog->path,   sizeof(pLog->path),   "%s/%s", pLog->folder, file);
	}
	pthread_mutex_unlock (&pLog->mutex);
	
	fd_mkdir (folder);
	return ;
}

void vlog_print (void* self, const char* szFunc, int nLine, ...)
{
	VLog* pLog = (VLog*) self;
	va_list ap;
	
	pthread_mutex_lock (&pLog->mutex);

	if (pLog->mode & 0x0003)
	{
		u64t ts = vtimer_now ();

		va_start (ap, nLine);
		_FMT_to_STR (pLog->buf, sizeof(pLog->buf), pLog->FmtScreen, ts, szFunc, nLine, ap);
		va_end (ap);
	
		if (pLog->mode & 0x0001)
		{
			printf ("%s", pLog->buf);
		}
	
		if (pLog->mode & 0x0002)
		{
			if (_vlog_NeedBackupFile (self, ts))
			{
				_vlog_BackupFile (pLog->path, ts);
				pLog->nFileSize=0;
			}

			_FMT_to_STRxColor (pLog->buf, sizeof(pLog->buf));
			pLog->nFileSize += _vlog_WriteFile (pLog->path, pLog->buf);
		}
		pLog->lastTs = ts;
	}

	pthread_mutex_unlock (&pLog->mutex);

	return;
}

static int _vlog_NeedBackupFile (void* self, u64t ts)
{
	VLog* pLog = (VLog*) self;

	/* Check size. */
	if ((pLog->nFileSize >= pLog->MaxFileSize))
	{
		return 1;
	}

	/* Check rotate. */
	if (pLog->MaxRotateMs!=0)
	{
		if ((ts % pLog->MaxRotateMs) < (pLog->lastTs % pLog->MaxRotateMs))
		{
			return 1;
		}
	}
	return 0;
}

static int _vlog_BackupFile (char* path, u64t ts)
{
	char bakName[LEN256+64];
	char Time[64];

	vtimer_tsString (Time, sizeof(Time), "YMDhms", ts);
	snprintf (bakName, sizeof(bakName), "%s.%s", path, Time);
	
	return fd_rename (path, bakName);
}

static int _vlog_WriteFile (char* path, char* buf)
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


static DateTime _TS_to_DT (u64t ts)
{
    DateTime    DT;
	u64t        sec_u64   = ts/1000;
	time_t      sec_timet = sec_u64;
	struct tm   stTimeInfo;

	gmtime_r (&sec_timet, &stTimeInfo);
	DT.ts    = ts;
	DT.Y     = stTimeInfo.tm_year +1900;
	DT.M     = stTimeInfo.tm_mon +1;
	DT.D     = stTimeInfo.tm_mday;
	DT.h     = stTimeInfo.tm_hour;
	DT.m     = stTimeInfo.tm_min;
	DT.s     = stTimeInfo.tm_sec;

	return DT;
}

static u64t _STR_to_MS (char* str)
{
	u64t ms=0;
	char* p = str;
	while (1)
	{
		n64t nNum = atoi (p);
		int nDigit = _nDigit (p);
		char fmt = *(p+nDigit);
		if (fmt=='\0')
		{
			break;
		}
		if (nDigit==0)
		{
			nNum = 1;
		}

		switch (fmt)
		{
		case 'U':
			ms = LLONG_MAX;
			break;
		case 'Y':
			ms += nNum * 1000 * 86400 * 365;
			break;
		case 'M':
			ms += nNum * 1000 * 86400 * 30;
			break;
		case 'W':
			ms += nNum * 1000 * 86400 * 7;
			break;
		case 'D':
			ms += nNum * 1000 * 86400;
			break;
		case 'h':
			ms += nNum * 1000 * 3600;
			break;
		case 'm':
			ms += nNum * 1000 * 60;
			break;
		case 's':
			ms += nNum * 1000;
			break;
		default:
			break;
		}
		p = (p+nDigit)+1;
	}

	return ms;
}

u64t _STR_to_SIZE (char* str)
{
	u64t size=0;
	char* p = str;
	while (1)
	{
		n64t nNum = atoi (p);
		int nDigit = _nDigit (p);
		char fmt = *(p+nDigit);
		if (fmt=='\0')
		{
			break;
		}
		if (nDigit==0)
		{
			nNum = 1;
		}

		switch (fmt)
		{
		case 'G':
			size += nNum << 30;
			break;
		case 'M':
			size += nNum << 20;
			break;
		case 'K':
			size += nNum << 10;
			break;
		case 'B':
			size += nNum;
			break;
		default:
			break;
		}
		p = (p+nDigit)+1;
	}

	return size;
}


static int _nDigit (const char* str)
{
	int i=0;
	while ( isdigit(*(str+i)) != 0 )
	{
		i++;
	}
	return i;
}


static char* _FMT_to_STRxColor (char* src, int size)
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

static char* _FMT_to_STR (char* buf, int bufLen, const char* fmt, u64t ts, const char* szFunc, int nLine, va_list ap)
{
	/* fmt=[Y/M/D h:m:s.u3 | F10():L4 | S | X | V] */

	if (buf==NULL || fmt==NULL)
	{
		return NULL;
	}

	char*        p = buf;
	int          space = bufLen;
	int          fmtLen = strlen(fmt);
    DateTime     DT = _TS_to_DT (ts);
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
					nDigit = _nDigit (fmt+i+1);
					i = i + nDigit;
					_p=va_arg (ap, char*);
					snprintf (p, space, "%*s", nWidth, _p);
				}
				break;
		/* Func & Line */
			case 'F':
				nWidth = atoi (fmt+i+1);
				nDigit = _nDigit (fmt+i+1);
				i = i + nDigit;
				snprintf (p, space, "%*s", nWidth, szFunc);
				break;
			case 'L':
				nWidth = atoi (fmt+i+1);
				nDigit = _nDigit (fmt+i+1);
				i = i + nDigit;
				snprintf (p, space, "%*d", nWidth, nLine);
				break;
		/* Time */
			case 'Y':
				snprintf (p, space, "%04d", DT.Y);
				break;
			case 'M':
				snprintf (p, space, "%02d", DT.M);
				break;
			case 'D':
				snprintf (p, space, "%02d", DT.D);
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
				nDigit = _nDigit (fmt+i+1);
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




