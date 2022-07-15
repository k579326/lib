
#include "util_dir.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#ifndef WIN32
#include <dirent.h>
#include <unistd.h>

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
#include <direct.h>

#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "shlwapi.lib")


#ifndef F_OK
#define F_OK 0
#endif

static bool _is_dir(const CptChar* dirpath)
{
	DWORD dwAttr = GetFileAttributes(dirpath);
	
	if (INVALID_FILE_ATTRIBUTES == dwAttr)
	{// function failed or file(folder) not exist
		return false;
	}
	
	return dwAttr & FILE_ATTRIBUTE_DIRECTORY;
}



static int _create_path(const CptChar* path)
{
	int err;
	bool condition;
	CptChar fullPath[MAX_PATH] = { 0 };
	CptChar* pcursor = NULL;
	
	condition = (path[0] > TEXT('A') && path[0] < TEXT('Z')) || (path[0] > TEXT('a') || path[0] < TEXT('z'));
	
    // if param is not a full path of windows
    if (path[1] != TEXT(':') || !condition)
    {
        // get full path of module
		GetModuleFileName(NULL, fullPath, MAX_PATH);
		PathRemoveFileSpec(fullPath);
		CptStringCat(fullPath, TEXT("\\"));
    }
	
	CptStringCat(fullPath, path);
	
	pcursor = fullPath;
    // 把路径中的'/'换成'\'
    while (*pcursor++ != 0)
    {
        if (*pcursor == TEXT('/'))
        {
            *pcursor = TEXT('\\');
        }
    }

	err = SHCreateDirectoryEx(NULL, fullPath, NULL);
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


DIR* opendir(const CptChar* path)
{
	DIR* dir = NULL;
	WIN32_FIND_DATA wd;
	CptChar tmp[MAX_PATH];
	CptChar* filename = NULL;
	HANDLE handle;
	
    if (path == NULL)
    {
        return dir;
    }

	CptStringCopy(tmp, path);
	CptStringCat(tmp, TEXT("\\*"));

    handle = FindFirstFile(tmp, &wd);
    if (handle == INVALID_HANDLE_VALUE)
    {
		return dir;
    }

	dir = (DIR*)malloc(sizeof(DIR));
	if (!dir)
	{
		FindClose(handle);
		return NULL;
	}

	memset(dir, 0, sizeof(DIR));
	dir->curDir = (struct dirent*)malloc(sizeof(struct dirent));
	CptStringCopy(dir->curDir->d_name, wd.cFileName);
	
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
	WIN32_FIND_DATA wd;
	
	if (dir == NULL)
		return NULL;
	
	if (dir->isFirst)
	{
		dir->isFirst = false;
		return dir->curDir;
	}
	
	ret = FindNextFile(dir->handle, &wd);
    if (!ret)
    {
        return NULL;
    }
	
	CptStringCopy(dir->curDir->d_name, wd.cFileName);
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

#endif	// windows directory operator 


// 支持创建相对路径的目录
int CommCreateDir(const CptChar* path)
{
	if (path == NULL || path[0] == TEXT('\0'))
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
bool CommIsDir(const CptChar* path)
{
	if (path == NULL || path[0] == TEXT('\0'))
	{
		assert(0);
		return false;
	}

	return _is_dir(path);
}

bool CommIsEmptyDir(const CptChar* path)
{
	int 	ret = 0;
    struct 	dirent* info = NULL;
    DIR* 	dir = NULL;
	bool 	empty = true;
	
	if (path == NULL || path[0] == TEXT('\0'))
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

        if (0 == CptStringCmp(info->d_name, TEXT(".")) 
			|| 0 == CptStringCmp(info->d_name, TEXT(".."))) 
		{
            continue;
        }
		
		empty = false;
		break;
    }
    
    closedir(dir);

	return empty;
}


int CommClearDir(const CptChar* path, bool force)
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

		if (0 == CptStringCmp(info->d_name, TEXT("."))
			|| 0 == CptStringCmp(info->d_name, TEXT("..")))
		{
			continue;
		}

        if (!force)
        {// 非强制删除模式
            empty = false;
            break;
        }

        int err = 0;
		CptChar path_will_remove[1024];
        CptStringCopy(path_will_remove, path);
        CptStringCat(path_will_remove, TEXT("/"));
		CptStringCat(path_will_remove, info->d_name);

        if (info->d_type == DT_REG)
        {
#if !defined WIN32 && !defined _WIN32
			err = remove(path_will_remove);
#else
			err = !DeleteFile(path_will_remove);
#endif
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


int CommRemoveDir(const CptChar* path, bool force)
{
	int ret = 0;
	
    ret = CommClearDir(path, force);
    
	if (ret == 0)
	{
#ifdef WIN32
		ret = (RemoveDirectory(path) == TRUE ? 0 : -1);
#else
		ret = rmdir(path);
#endif
	}

	return ret;
}
