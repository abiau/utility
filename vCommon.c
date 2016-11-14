
#include "vCommon.h"
 


void fd_Backtrace (char* buf, int len)
{
    void*   bt[1024];
    int     bt_size;
    char**  bt_syms;
    int     i;

    bt_size = backtrace(bt, 1024);
    bt_syms = backtrace_symbols(bt, bt_size);
    
    for (i=1; i<bt_size; i++)
	{
		//snprintf (buf, sizeof(buf), "%s\n", bt_syms[i]);
		printf ("%s\n", bt_syms[i]);
	}
	free(bt_syms);
}


void fd_Backtrace_file (const char* path)
{
	int      fd;
    void*    bt[1024];
    int      bt_size;
	ssize_t  size;

	bt_size = backtrace(bt, 1024);
	fd = fd_open (path);
	if (fd<0)
	{
		return ;
	}
	backtrace_symbols_fd (bt, bt_size, fd);
	fd_close (fd);

	return ;
}



int fd_open (const char* path)
{
	int flags = O_WRONLY | O_APPEND;
	int fd;
	
	fd = open (path, flags);
	if (fd<0)
	{
		if (errno==ENOENT)
		{
		    fd = creat (path, 0644); // fd = open (path, O_WRONLY | O_CREAT| O_TRUNK, 0644);
			if (fd<0)
			{
				return fd;
			}
		}
		else
		{
			return fd;
		}
	}

	return fd;
}

int fd_close (int fd)
{
	return close (fd);
}

int fd_rename (const char *oldPath, const char* newPath)
{
	return rename (oldPath, newPath);
}

int fd_isFileExist (const char *path)
{
	struct stat s;

	return (stat(path , &s)==0)?1:0 ;
}


int fd_isFolderExist (const char *folder)
{
	struct stat s;
	int ret;

	ret = stat(folder , &s);
	if (ret==-1)
	{
		if (errno == ENOENT)
		{
			/* does not exist. */
			return 0;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		if(S_ISDIR(s.st_mode))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
}

n64t fd_getFileSize (const char* path)
{
	struct stat st;
	memset (&st, 0, sizeof(struct stat));
	stat (path, &st);
	
	return st.st_size;
}

int fd_mkdir(const char *folder)
{
	mode_t mode = (S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	char *q, *r = NULL, *path = NULL, *up = NULL;
	int ret;

	ret = -1;
	if (strcmp(folder, ".") == 0 || strcmp(folder, "/") == 0)
		return (0);

	if ((path = strdup(folder)) == NULL)
		exit(1);

	if ((q = strdup(folder)) == NULL)
		exit(1);

	if ((r = dirname(q)) == NULL)
		goto out;

	if ((up = strdup(r)) == NULL)
		exit(1);

	if ((fd_mkdir(up) == -1) && (errno != EEXIST))
		goto out;

	if ((mkdir(path, mode) == -1) && (errno != EEXIST))
		ret = -1;
	else
		ret = 0;

out:
	if (up != NULL)
		free(up);
	free(q);
	free(path);

	return (ret);
}










#if 0
void vzero (void* dest, int size)
{
	memset (dest, 0, size);
	return ;
}
#endif


void vmsleep (int msec)
{
	int sec;
	int usec;

	sec = msec/1000;
	usec = (msec%1000)*1000;
	sleep (sec);
	usleep (usec);

	return ;
}

n64t vc_addMemUsage (int size)
{
	static n64t gMemUsage=0;
	return  __sync_add_and_fetch (&gMemUsage, size);
}

n64t vc_getMemUsage (void)
{
	return vc_addMemUsage(0);
}

void* vc_malloc (int size)
{
	if (size<=0)
	{
		return NULL;
	}
	
	void* p;
	p = malloc (size); 
	if (p!=NULL)
	{
		vc_addMemUsage (size);
		vzero (p, size);
	}
	return p;
}

int vc_free (void* p, int size)
{
	if (p==NULL || size<=0)
	{
		return -1;
	}

	vc_addMemUsage ((-1)*size);
	free (p);
	p = NULL;
	return 0;
}





