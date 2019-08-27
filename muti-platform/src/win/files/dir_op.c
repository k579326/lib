#include <Shlobj.h>
#include "file-common.h"
#include "other-tmp.h"

typedef struct  
{
    file_sf_em  sf;
    int         win_sf_id;
}fsf_map_t;

const static fsf_map_t cs_sf_list[] = 
{
    FSF_WINDOWS,            CSIDL_WINDOWS,
    FSF_COMMON_APPDATA,     CSIDL_COMMON_APPDATA,
    FSF_DESKTOP,            CSIDL_DESKTOP,
    FSF_PROGRAM_FILESX86,   CSIDL_PROGRAM_FILESX86,
    FSF_COMMON_PROGRAMS,    CSIDL_COMMON_PROGRAMS,
    FSF_LOCAL_APPDATA,      CSIDL_LOCAL_APPDATA,
	FSF_APPDATA,			CSIDL_APPDATA,
    FSF_SYSTEM,             CSIDL_SYSTEM
};


int cp_dir_create(const char* path)
{
    int retcode = CP_ERROR_OK;
    char tmp[MAX_PATH] = { 0 };
    wchar_t* w_path = NULL;
    char* pcursor = NULL;
    BOOL condition = FALSE;

    if (path == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    condition = (path[0] > 'A' && path[0] < 'Z') || (path[0] > 'a' || path[0] < 'z');
    // if param is not a full path of windows
    if (path[1] != ':' || !condition)
    {
        // get full path of module
        retcode = cp_path_getexepath(tmp, MAX_PATH);
        if (retcode != CP_ERROR_OK)
        {
            return retcode;
        }

        // get the parent directory of this module
        retcode = cp_path_predir(tmp, tmp);
        if (retcode != CP_ERROR_OK)
        {
            return retcode;
        }

        strcat(tmp, "\\");
    }

    strcat(tmp, path);

    pcursor = tmp;
    // 把路径中的'/'换成'\'
    while (*pcursor++ != 0)
    {
        if (*pcursor == '/')
        {
            *pcursor = '\\';
        }
    }

    if (CP_ERROR_OK == cp_path_exist(tmp))
    {
        return CP_ERROR_OK;
    }

    w_path = mp_utf8_to_widechar(tmp);
    retcode = SHCreateDirectoryExW(NULL, (LPWSTR)w_path, NULL);
    free(w_path);

    if (retcode == ERROR_SUCCESS)
    {
        return CP_ERROR_OK;
    }
    
    return CP_ERROR_DIR_CREATE_FAILED;
}


int cp_dir_specialfolder(char* path, file_sf_em folder_type, bool fcreate)
{
    int index = 0;
    wchar_t w_path[MAX_PATH];
    char* tmp_path = NULL;

    if (path == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    for (index = 0; index < sizeof(cs_sf_list) / sizeof(fsf_map_t); index++)
    {
        if (folder_type == cs_sf_list[index].sf)
        {
            break;
        }
    }

    if (index == sizeof(cs_sf_list) / sizeof(fsf_map_t))
    {
        return CP_ERROR_INVALID_PARAM;
    }

    if (!SHGetSpecialFolderPathW(NULL, w_path, cs_sf_list[index].win_sf_id, fcreate))
    {
        return CP_ERROR_DIR_GETSPECIALDIR_FAILED;
    }

    tmp_path = mp_widechar_to_utf8(w_path);
    strcpy(path, tmp_path);
    free(tmp_path);

    return CP_ERROR_OK;
}

int cp_dir_open(const char* path, dir_handle* handle, file_info_t* di)
{
    WIN32_FIND_DATAW wd;
	char tmp[MAX_PATH];
    wchar_t* w_path = NULL;
    char* filename = NULL;

    if (path == NULL || handle == NULL || di == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

	strcpy(tmp, path);
    cp_path_convert(tmp);
    strcat(tmp, "\\*");        // 搜索整个目录的文件。linux 不需要这个东西，只能放这里了

    w_path = mp_utf8_to_widechar(tmp);
    *handle = FindFirstFileW(w_path, &wd);
    free(w_path);

    if (*handle == INVALID_HANDLE_VALUE)
    {
        *handle = NULL;
        return CP_ERROR_DIR_OPEN_FAILED;
    }

    filename = mp_widechar_to_utf8(wd.cFileName);
    strcpy(di->item_name, filename);
    free(filename);

    if (wd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        di->item_type = FT_DIR;
    }
    else
    {
        di->item_type = FT_NORMAL;                  // 不是目录就是普通文件，暂时先这样。 guok
    }

    return CP_ERROR_OK;
}

int cp_dir_read(dir_handle* handle, file_info_t* di)
{
    bool bret = false;
    char* filename = NULL;
    WIN32_FIND_DATAW wd;

    if (handle == NULL || di == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    bret = FindNextFileW(*handle, &wd);
    if (!bret)
    {
        return CP_ERROR_DIR_READ_FAILED;
    }

    filename = mp_widechar_to_utf8(wd.cFileName);
    strcpy(di->item_name, filename);
    free(filename);

    if (wd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        di->item_type = FT_DIR;
    }
    else
    {
        di->item_type = FT_NORMAL;                  // 不是目录就是普通文件，暂时先这样。 guok
    }


    return CP_ERROR_OK;
}


int cp_dir_close(dir_handle handle)
{
    if (handle == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    FindClose(handle);

    return CP_ERROR_OK;
}