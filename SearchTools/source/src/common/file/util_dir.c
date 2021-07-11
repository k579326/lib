
#include "util_dir.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#ifndef WIN32
#include <dirent.h>


static bool _is_dir(const char* dirpath)
{
	struct stat st;
	stat(dirpath, &st);
	
	return S_ISDIR(st.st_mode);
}

static int _create_path(const char* path)
{
    char cur_path[PATH_MAX] = { 0 };
    int i = 0;
	
	while (1)
	{
        if (strlen(path) <= i || path[i] == '\0') {
            break;
        }

		while (path[i] != '/' && path[i] != '\0')
		{
			i++;
		}
		
        // 再向前移动一个位置
        i++;

		// 注意以根目录开始的路径
		strncpy(cur_path, path, i);
		
		if (access(cur_path, F_OK) == 0)
		{
			continue;
		}
		
		umask(0);
		if (mkdir(cur_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)        // chmod 775
		{
			return -1;
		}
	}
	
    if (access(path, F_OK) != 0)
	{
		return -1;
	}
	
	return 0;
}


#else // windows implement



#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Shlobj.h>
#include <Shlwapi.h>

#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "shlwapi.lib")


#ifndef F_OK
#define F_OK 0
#endif

static bool _is_dir(const char* dirpath)
{
	DWORD dwAttr = GetFileAttributesA(dirpath);
	
	if (INVALID_FILE_ATTRIBUTES == dwAttr)
	{// function failed or file(folder) not exist
		return false;
	}
	
	if (0 == (dwAttr & FILE_ATTRIBUTE_DIRECTORY)){
		return false;
	}
	
	return true;
}



static int _create_path(const char* path)
{
	int err;
	bool condition;
	char fullPath[MAX_PATH] = { 0 };
	char* pcursor = NULL;
	
	condition = (path[0] > 'A' && path[0] < 'Z') || (path[0] > 'a' || path[0] < 'z');
	
    // if param is not a full path of windows
    if (path[1] != ':' || !condition)
    {
        // get full path of module
		GetModuleFileNameA(NULL, fullPath, MAX_PATH);
		PathRemoveFileSpecA(fullPath);
		
        strcat(fullPath, "\\");
    }
	
	strcat(fullPath, path);
	
	pcursor = fullPath;
    // 把路径中的'/'换成'\'
    while (*pcursor++ != 0)
    {
        if (*pcursor == '/')
        {
            *pcursor = '\\';
        }
    }

	err = SHCreateDirectoryExA(NULL, fullPath, NULL);
	if (err == ERROR_SUCCESS || err == ERROR_ALREADY_EXISTS)
		return 0;
	
	return -1;
}

typedef struct DIR
{
	HANDLE handle;		// windows dir handle
	bool isFirst;
	struct dirent* curDir;
}DIR;


DIR* opendir(const char* path)
{
	DIR* dir = NULL;
	WIN32_FIND_DATAA wd;
	char tmp[MAX_PATH];
    char* filename = NULL;
	HANDLE handle;
	
    if (path == NULL)
    {
        return dir;
    }

	strcpy(tmp, path);
    strcat(tmp, "\\*");

    handle = FindFirstFileA(tmp, &wd);

    if (handle == INVALID_HANDLE_VALUE)
    {
		return dir;
    }

	dir = (DIR*)malloc(sizeof(DIR));
	memset(dir, 0, sizeof(DIR));
	dir->curDir = (struct dirent*)malloc(sizeof(struct dirent));
	
	strcpy(dir->curDir->d_name, wd.cFileName);
	
    if (wd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        dir->curDir->d_type = DT_DIR;
    }
    else
    {
        dir->curDir->d_type = DT_REG;                  // 不是目录就是普通文件，暂时先这样。
    }
	
	dir->isFirst = true;
    dir->handle = handle;

    return dir;
}

struct dirent* readdir(DIR* dir)
{
	DWORD ret;
	WIN32_FIND_DATAA wd;
	
	if (dir == NULL)
		return NULL;
	
	if (dir->isFirst)
	{
		dir->isFirst = false;
		return dir->curDir;
	}
	
	ret = FindNextFileA(dir->handle, &wd);
    if (!ret)
    {
        return NULL;
    }
	
	strcpy(dir->curDir->d_name, wd.cFileName);
    if (wd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        dir->curDir->d_type = DT_DIR;
    }
    else
    {
        dir->curDir->d_type = DT_REG;                  // 不是目录就是普通文件，暂时先这样。
    }
	
	return dir->curDir;
}

int closedir(DIR *dir)
{
	if (!dir)
		return -1;
	
	FindClose(dir->handle);
	
	free(dir->curDir);
	free(dir);
	return 0;
}

#endif


// 支持创建相对路径的目录
int CommCreateDir(const char* path)
{
	if (path == NULL || path[0] == '\0')
	{
		assert(0);
		return -1;
	}
	
	if (CommIsDir(path))
	{// 目录存在
		return 0;
	}
	
	return _create_path(path);
}

// 不支持相对路径
bool CommIsDir(const char* path)
{
	if (path == NULL || path[0] == '\0')
	{
		assert(0);
		return false;
	}

	return _is_dir(path);
}

bool CommIsEmptyDir(const char* path)
{
	int 	ret = 0;
    struct 	dirent* info = NULL;
    DIR* 	dir = NULL;
	bool 	empty = true;
	
	if (path == NULL || path[0] == '\0')
	{
		assert(0);
		return true;
	}
	
    dir = opendir(path);
    if (!dir){
        return true;
    }

    while (1)
    {
        info = readdir(dir);
        if (info == NULL){
            break;
        }

        if (0 == strcmp(info->d_name, ".") || 0 == strcmp(info->d_name, "..")) {
            continue;
        }
		
		empty = false;
		break;
    }
    
    closedir(dir);

	return empty;
}

int CommClearDir(const char* path, bool force)
{
    int ret = 0;
    struct dirent* info = NULL;
    bool empty = false;

    DIR* dir = opendir(path);

    if (!dir) {
        return -1;
    }

    while (1)
    {
        info = readdir(dir);
        if (info == NULL)
        {
            empty = true;
            break;
        }

        if (0 == strcmp(info->d_name, ".") || 0 == strcmp(info->d_name, "..")) {
            continue;
        }

        if (!force)
        {// 非强制删除模式
            empty = false;
            break;
        }

        int err = 0;
        char path_will_remove[1024];
        strcpy(path_will_remove, path);
        strcat(path_will_remove, "/");
        strcat(path_will_remove, info->d_name);

        if (info->d_type == DT_REG)
        {
            err = remove(path_will_remove);
        }
        else
        {// DIR
            err = CommRemoveDir(path_will_remove, force);
        }

        if (err != 0)
        {
            empty = false;
            break;
        }
    }

    closedir(dir);

    return empty == true ? 0 : -1;
}


int CommRemoveDir(const char* path, bool force)
{
	int ret = 0;
	
    ret = CommClearDir(path, force);
    
	if (ret == 0)
	{
#ifdef WIN32
		ret = (RemoveDirectoryA(path) == TRUE ? 0 : -1);
#else
		ret = rmdir(path);
#endif
	}

	return ret;
}
