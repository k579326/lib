#include <Shlwapi.h>
#include "file-common.h"
#include "other-tmp.h"

typedef struct  
{
    file_op_em  mode;
    wchar_t        param[TINYLEN];
}fo_map_t;


const static fo_map_t mod_map[] =
{
    FOB_READ,   L"rb",
    FOB_WRITE,  L"wb",
    FOB_RW,     L"ab+",
    FOT_READ,   L"rt",
    FOT_WRITE,  L"wt",
    FOT_RW,     L"at+"
};



int cp_file_open(IN const char* path, IN file_op_em mode, cp_fd* fd)
{
    int i = 0;
    wchar_t* w_path = NULL;

    if (path == NULL || fd == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    for (i = 0; i < sizeof(mod_map) / sizeof(fo_map_t); i++)
    {
        if (mod_map[i].mode == mode)
        {
            break;
        }
    }
    
    if (i == sizeof(mod_map) / sizeof(fo_map_t))
    {
        // mode which input is not valid param.
        return CP_ERROR_INVALID_PARAM;
    }
    
    // if (mod_map[i].mode == FOB_WRITE && (cp_path_exist(path) != 0))
    // {
    //     return CP_ERROR_FILE_NOT_EXIST;
    // }
    // else if (mod_map[i].mode == FOT_WRITE && (cp_path_exist(path) != 0))
    // {
    //     return CP_ERROR_FILE_NOT_EXIST;
    // }
    
    w_path = mp_utf8_to_widechar(path);
    *fd = _wfopen(w_path, mod_map[i].param);
    free(w_path);

    if (*fd == NULL)
    {
        if (errno == 2)
        {
            return CP_ERROR_FILE_NOT_EXIST;
        }
        else
        {
            return CP_ERROR_FILE_OPEN_FAILED;
        }
    }

    return CP_ERROR_OK;
}


int cp_file_close(IN cp_fd fd)
{
    if (fd == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    return fclose(fd) == 0 ? CP_ERROR_OK : CP_ERROR_FILE_CLOSE_FAILED;
}

int cp_file_read(IN cp_fd fd, INOUT uint8_t* buff, IN int size, OUT int* readsize)
{
    const uint8_t* p = buff;
    int tmpsize = 0;
    int ret = CP_ERROR_OK;

    if (fd == NULL || buff == NULL || size < 0 || readsize == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    if (size == 0)
    {
        return CP_ERROR_OK;
    }

    // read data
    ret = fread(buff, 1, size, fd);
    
    if (ret != 0)
    {
        *readsize = ret;
    }
    else if (feof(fd))
    {
        return CP_ERROR_FILE_EOF;
    }
    else
    {
        return CP_ERROR_FILE_READ_FAILED;
    }
    
    return CP_ERROR_OK;
}

int cp_file_write(IN cp_fd fd, IN uint8_t* buff, IN int size, OUT int* readsize)
{
    int ret = 0;
    int wsize = 0;

    if (fd == NULL || buff == NULL || size < 0)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    if (size == 0)
    {
        ret = CP_ERROR_OK;
        wsize = 0;
        goto exit;
    }

    wsize = fwrite(buff, 1, size, fd);
    if (wsize != size)
    {
        ret = CP_ERROR_FILE_WRITE_FAILED;
        goto exit;
    }
    
    ret = CP_ERROR_OK;
    
exit:    
    if (readsize != NULL)
    {
        *readsize = wsize;
    }
    
    return ret;
}

int cp_file_seek(IN cp_fd fd, IN long offset, IN file_pos_em pos)
{
    int seek_pos = 0;

    if (fd == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    if (-1 == fseek(fd, offset, pos))
    {
        return CP_ERROR_FILE_SEEK_FAILED;
    }

    return CP_ERROR_OK;
}


int cp_file_size(IN cp_fd fd, OUT int* size)
{
    if (fd == NULL || size == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    if (fseek(fd, 0, SEEK_END) == -1)
    {
        return CP_ERROR_FILE_SEEK_FAILED;
    }

    *size = ftell(fd);

    if (fseek(fd, 0, SEEK_SET) == -1)
    {
        return CP_ERROR_FILE_SEEK_FAILED;
    }

    if (*size == -1)
    {
        return CP_ERROR_FILE_TELL_FAILED;
    }

    return CP_ERROR_OK;
}


int cp_file_rename(IN const char* oldname, IN const char* newname)
{
    wchar_t* w_oldname = NULL;
    wchar_t* w_newname = NULL;

    if (oldname == NULL || newname == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    w_oldname = mp_utf8_to_widechar(oldname);
    w_newname = mp_utf8_to_widechar(newname);
    
    if (!MoveFileW(w_oldname, w_newname))
    {
        free(oldname);
        free(newname);
        return CP_ERROR_FILE_RENAME_FAILED;
    }

    free(oldname);
    free(newname);

    return CP_ERROR_OK;
}


int cp_file_remove(IN const char* path)
{
    wchar_t* w_path = NULL;
    if (path == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    w_path = mp_utf8_to_widechar(path);

    if (_wremove(w_path) == -1)
    {
        free(w_path);
        return CP_ERROR_FILE_REMOVE_FAILED;
    }

    free(w_path);
    return CP_ERROR_OK;
}


int cp_file_copy(IN const char* srcpath, IN const char* dstpath, IN bool no_overwrite)
{
    int ret = CP_ERROR_OK;
    wchar_t* w_srcpath = NULL;
    wchar_t* w_dstpath = NULL;

    w_srcpath = mp_utf8_to_widechar(srcpath);
    w_dstpath = mp_utf8_to_widechar(dstpath);

    if (PathFileExistsW(w_dstpath))
    {
        if (no_overwrite)
        {
            ret = CP_ERROR_OK;
            goto ret;
        }
    }

    if (!CopyFileW(w_srcpath, w_dstpath, FALSE))
    {
        ret = -1;
        goto ret;
    }

ret:
    free(w_srcpath);
    free(w_dstpath);

    return ret;
}


int cp_file_iseof(IN cp_fd fd, OUT bool* flag)
{
    if (fd == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    if (feof(fd))
    {
        *flag = true;
    }
    else
    {
        *flag = false;
    }

    return CP_ERROR_OK;
}


int cp_file_flush(IN cp_fd fd)
{
    if (fd == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    return -1 == fflush(fd) ? CP_ERROR_FILE_FLUSH_FAILED : CP_ERROR_OK;
}
