
/* this library can be used for c and c++ */

#ifndef _UTIL_DIR_H_
#define _UTIL_DIR_H_

#include "cpt-string.h"

#ifndef WIN32
	#include <unistd.h>
    #include <errno.h>
    #include <sys/stat.h>
    #include <limits.h>
    #include <dlfcn.h>
    #include <dirent.h>
#else
	/* mock linux dirent struct */
	typedef struct DIR DIR;
    struct dirent
	{
		int a;
		int b;
		unsigned short c;
		unsigned char d_type;
		CptChar d_name[256];
	};
	
	/* File types for `d_type'.  
	*	not use it for function stat
	*/
	enum
	{
		DT_UNKNOWN = 0,
	# define DT_UNKNOWN	DT_UNKNOWN
		DT_FIFO = 1,
	# define DT_FIFO	DT_FIFO
		DT_CHR = 2,
	# define DT_CHR		DT_CHR
		DT_DIR = 4,
	# define DT_DIR		DT_DIR
		DT_BLK = 6,
	# define DT_BLK		DT_BLK
		DT_REG = 8,
	# define DT_REG		DT_REG
		DT_LNK = 10,
	# define DT_LNK		DT_LNK
		DT_SOCK = 12,
	# define DT_SOCK	DT_SOCK
		DT_WHT = 14
	# define DT_WHT		DT_WHT
	};

	/* Convert between stat structure types and directory types.  */
	# define IFTODT(mode)	(((mode) & 0170000) >> 12)
	# define DTTOIF(dirtype)	((dirtype) << 12)
#endif


#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int 	CommCreateDir(const CptChar* path);

bool 	CommIsDir(const CptChar* path);

int 	CommRemoveDir(const CptChar* path, bool force);

int     CommClearDir(const CptChar* path, bool force);

bool 	CommIsEmptyDir(const CptChar* path);


#ifdef WIN32
	
DIR* 	opendir(const CptChar* path);

struct 
dirent* readdir(DIR* dir);

int 	closedir(DIR *dir);
#endif



#ifdef __cplusplus
}
#endif



#endif // _UTIL_DIR_H_

