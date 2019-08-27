#include <errno.h>
#include <string.h>
#include "file-common.h"

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#if TARGET_OS_IPHONE

#elif TARGET_OS_MAC
#include <sys/proc_info.h>
#include <libproc.h>

#endif

int cp_path_exist(const char *path)
{
    char convert_path[PATH_MAX] = { 0 };

    if (path == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    strcpy(convert_path, path);
    cp_path_convert(convert_path);

    if (access(convert_path, F_OK) == -1)
    {
        if (errno != ENOENT)
        {
            return CP_ERROR_FILE_CHECKEXIST_FAILED;
        }

        return CP_ERROR_FILE_NOT_EXIST;
    }

    return CP_ERROR_OK;
}


int cp_path_predir(const char *spath, char *predir)
{
    char* str = NULL; 
    char convert_path[PATH_MAX] = { 0 };

    if (spath == NULL || predir == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    strcpy(convert_path, spath);
    cp_path_convert(convert_path);

    // root dir have no parent dir.
    if (0 == strcmp(convert_path, "/"))
    {
        predir[0] = 0;
        return CP_ERROR_OK;
    }

    str = strrchr(convert_path, '/');

    if (str == NULL)
    {
        // no parent dir, set path empty. 
        predir[0] = 0;
    }
    else if (str == convert_path)
    {
        // parent dir is root, set path as "/"
        strcpy(predir, "/");
    }
    else 
    {
        *str = '\0';
        strcpy(predir, convert_path);
    }
    
    return CP_ERROR_OK;
}

int cp_path_lastname(const char *path, char *lastname)
{
    const char* tmp = path;
    char convert_path[PATH_MAX] = { 0 };

    if (path == NULL || lastname == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    strcpy(convert_path, path);
    cp_path_convert(convert_path);

    tmp = strrchr(convert_path, '/');

    if (tmp == NULL)
    {
        strcpy(lastname, convert_path);
        return CP_ERROR_OK;
    }

    strcpy(lastname, tmp + 1);
    return CP_ERROR_OK;
}

int cp_path_getlibpath(const char* function, char* path)
{
    Dl_info dlinfo;
    int ret = CP_ERROR_OK;

    if (function == NULL || path == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    ret = dladdr(function, &dlinfo);
    if (ret == 0)
    {
        return CP_ERROR_PATH_GETLIBPATH_FAILED;
    }

    strcpy(path, dlinfo.dli_fname);

    return CP_ERROR_OK;
}


int cp_path_getexepath(char* path, uint32_t len)
{
    int ret = 0;       // length
    char temp[PATH_MAX] = {0};
    pid_t proc_id;
    
    if (path == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }
    
    proc_id = getpid();
#if !TARGET_OS_IPHONE && TARGET_OS_MAC
    
    ret = proc_pidpath(proc_id, path, len);
    if (ret == 0)
    {
        return CP_ERROR_PATH_GETEXEPATH_FAILED;
    }
    
#else
    sprintf(temp, "/proc/%d/exe", proc_id);
    
    ret = readlink(temp, path, len);
    if (ret < 0 || ret >= len)
    {
        return CP_ERROR_PATH_GETEXEPATH_FAILED;
    }
    
    // readlink拷贝内容，但不设置结束符，需要根据返回有效长度自行设置。
    path[ret] = '\0';
    
#endif
    return CP_ERROR_OK;
}



int cp_path_convert(char* path)
{
    int i = 0;

    if (path == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    while (path[i] != '\0')
    {
        if (path[i] == '\\')
        {
            path[i] = '/';
        }
        i++;
    }

    return CP_ERROR_OK;
}
