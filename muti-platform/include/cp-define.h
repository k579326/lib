#ifndef _CP_DEFINE_H_
#define _CP_DEFINE_H_

#define  TINYLEN   32
#define  SMALLLEN  256
#define  BIGLEN    1024
#define  LARGELEN  (1024 * 4)

#if defined __linux__ || defined __linux || (defined __APPLE__)
    #include <unistd.h>
    #include <errno.h>
    #include <sys/stat.h>
    #include <limits.h>
    #include <dlfcn.h>
    #include <dirent.h>

    typedef DIR*            dir_handle;
    typedef struct dirent*    dir_info;

    #define MAX_PATH PATH_MAX
#elif defined WIN32 || defined _MSC_VER || defined _WIN64
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>

    typedef HANDLE            dir_handle;
    typedef WIN32_FIND_DATA   dir_info;
#endif

typedef enum 
{
    FT_NORMAL,
    FT_DIR,
}file_type_em;


typedef struct  
{
    char            item_name[BIGLEN];
    file_type_em    item_type;
}file_info_t;



typedef FILE*   cp_fd;

typedef enum
{
    FOT_READ,          // only read, if file is not exist, function fails.
    FOT_WRITE,         // only write, if file is not exist, function fails.
    FOT_RW,            // read and write by the append mode, if file is not exist, file will be created.
    FOB_READ,          // binary mode
    FOB_WRITE,         // binary mode
    FOB_RW,            // binary mode
}file_op_em;

typedef enum
{
    FSK_BEGIN = SEEK_SET,                           // SEEK_SET
    FSK_CUR = SEEK_CUR,                           // SEEK_CUR
    FSK_END = SEEK_END                            // SEEK_END
}file_pos_em;

typedef enum
{
    FSF_WINDOWS,
    FSF_COMMON_APPDATA,
    FSF_DESKTOP,
    FSF_PROGRAM_FILESX86,
    FSF_COMMON_PROGRAMS,
    FSF_LOCAL_APPDATA,
	FSF_APPDATA,
    FSF_SYSTEM
}file_sf_em;

// windows isn't define this macro. it should be zero
#ifndef F_OK
#define F_OK 0
#endif

#ifndef MAX_PATH
#define MAX_PATH PATH_MAX
#endif

#ifndef __cplusplus

#ifndef bool
#define bool  int
#define true  1
#define false 0
#endif

#endif


// 
#if defined _WIN32 || defined _WIN64 
#include <windows.h>
#define PATH_SIZE MAX_PATH
#else
#include <limits.h>
#define PATH_SIZE PATH_MAX
#endif






#endif
