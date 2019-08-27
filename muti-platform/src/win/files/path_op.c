#include <errno.h>
#include <string.h>
#include <windows.h>
#include <Shlwapi.h>
#include <assert.h>
#include "other-tmp.h"
#include "file-common.h"


int cp_path_exist(const char *path)
{
    wchar_t* w_path = NULL;

    if (path == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    w_path = mp_utf8_to_widechar(path);

    if (!PathFileExistsW(w_path))
    {
        free(w_path);
        return CP_ERROR_FILE_NOT_EXIST;
    }

    free(w_path);
    return CP_ERROR_OK;
}


int cp_path_predir(const char *spath, char *predir)
{
    char strtmp[MAX_PATH] = { 0 }; 
    BOOL bret = FALSE; 
    wchar_t* w_path = NULL;
    char* tmpdir = NULL;

    if (spath == NULL || predir == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }
    
    w_path = mp_utf8_to_widechar(spath);


    bret = PathRemoveFileSpecW(w_path);
    if (!bret)
    {
        free(w_path);
        return CP_ERROR_PATH_PARSE_FAILED;
    }

    tmpdir = mp_widechar_to_utf8(w_path);
    strcpy(predir, tmpdir);
    
    free(w_path);
    free(tmpdir);

    return CP_ERROR_OK;
}

int cp_path_lastname(const char *path, char *lastname)
{
    const wchar_t* tmp = NULL;
    wchar_t* w_path = NULL;
    char* filename = NULL;

    if (path == NULL || lastname == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    w_path = mp_utf8_to_widechar(path);


    tmp = PathFindFileNameW(w_path);
    if (tmp == w_path)
    {
        free(w_path);
        return CP_ERROR_PATH_PARSE_FAILED;
    }
        
    filename = mp_widechar_to_utf8(tmp);
    if (!filename)
    {
        free(w_path);
        return CP_ERROR_PATH_PARSE_FAILED;
    }

    strcpy(lastname, filename);
    free(w_path);
    free(filename);

    return CP_ERROR_OK;
}

int cp_path_getlibpath(const char* function, char* path)
{
    int ret = CP_ERROR_OK;
     
    assert(0);                  // windwos 不要调用这个函数了 
     
    // if (function == NULL || path == NULL)
    // {
    //     return CP_ERROR_INVALID_PARAM;
    // }

    // ret = dladdr(function, &dlinfo);
    // if (ret == 0)
    // {
    //     return CP_ERROR_PATH_GETLIBPATH_FAILED;
    // }

    // strcpy(path, dlinfo.dli_fname);

    return CP_ERROR_OK;
}

int cp_path_getexepath(char* path, uint32_t len)
{
    int ret = 0;       // length
    wchar_t w_path[MAX_PATH];
    char* m_path = NULL;

    if (path == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    ret = GetModuleFileNameW(NULL, w_path, MAX_PATH);
    if (ret == 0)
    {
        return CP_ERROR_PATH_GETEXEPATH_FAILED;
    }
    if (ret >= len)
    {
        return CP_ERROR_BUFFLEN_NOT_ENOUGH;
    }

    m_path = mp_widechar_to_utf8(w_path);
    //m_path = mp_widechar_to_ansi(w_path);
    if (!m_path)
    {
        return CP_ERROR_PATH_PARSE_FAILED;
    }

    strcpy(path, m_path);
    free(m_path);

    return CP_ERROR_OK;
}


int cp_path_convert(char* path)
{
    int i = 0;
    wchar_t* w_path = NULL;
    char* tmp_path = NULL;

    if (path == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    //w_path = malloc(sizeof(wchar_t) * (strlen(path) + 1));
    
    w_path = mp_utf8_to_widechar(path);

    while (w_path[i] != L'\0')
    {
        if (w_path[i] == L'/')
        {
            w_path[i] = L'\\';
        }
        i++;
    }

    tmp_path = mp_widechar_to_utf8(w_path);
    if (!tmp_path)
    {
        free(w_path);
        return CP_ERROR_PATH_PARSE_FAILED;
    }

    strcpy(path, tmp_path);
    
    free(w_path);
    free(tmp_path);

    return CP_ERROR_OK;
}