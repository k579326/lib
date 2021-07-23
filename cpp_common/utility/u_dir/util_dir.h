
/* this library can be used for c and c++ */

#ifndef _UTIL_DIR_H_
#define _UTIL_DIR_H_

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
		char d_name [256];
	};
	
	#ifndef S_IFMT
		#define S_IFMT  00170000  
		#define S_IFSOCK 0140000  
		#define S_IFLNK  0120000  
		#define S_IFREG  0100000  
		#define S_IFBLK  0060000  
		#define S_IFDIR  0040000  
		#define S_IFCHR  0020000  
		#define S_IFIFO  0010000  
		// #define S_ISUID  0004000  
		// #define S_ISGID  0002000  
		// #define S_ISVTX  0001000  
	#endif
	//#define S_ISLNK(m)      (((m) & S_IFMT) == S_IFLNK)  
	#define S_ISREG(m)      (((m) & S_IFMT) == S_IFREG)  
	#define S_ISDIR(m)      (((m) & S_IFMT) == S_IFDIR)  
	#define S_ISCHR(m)      (((m) & S_IFMT) == S_IFCHR)  
	//#define S_ISBLK(m)      (((m) & S_IFMT) == S_IFBLK)  
	//#define S_ISFIFO(m)     (((m) & S_IFMT) == S_IFIFO)  
	//#define S_ISSOCK(m)     (((m) & S_IFMT) == S_IFSOCK) 
#endif


#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int 	CommCreateDir(const char* path);

bool 	CommIsDir(const char* path);

int 	CommRemoveDir(const char* path, bool force);

int     CommClearDir(const char* path, bool force);

bool 	CommIsEmptyDir(const char* path);


#ifdef WIN32
	
DIR* 	opendir(const char* path);

struct 
dirent* readdir(DIR* dir);

int 	closedir(DIR *dir);
#endif



#ifdef __cplusplus
}
#endif



#endif // _UTIL_DIR_H_

