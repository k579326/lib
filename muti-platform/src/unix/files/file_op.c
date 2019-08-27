#include <assert.h>
#include "file-common.h"

typedef struct  
{
    file_op_em  mode;
    char        param[TINYLEN];
}fo_map_t;


const static fo_map_t mod_map[] =
{
    FOB_READ,   "rb",
    FOB_WRITE,  "wb",
    FOB_RW,     "ab+",
    FOT_READ,   "rt",
    FOT_WRITE,  "wt",
    FOT_RW,     "at+"
};


int cp_file_open(IN const char* path, IN file_op_em mode, cp_fd* fd)
{
    int i = 0;

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
    
    // if (mod_map[i].mode == FOB_WRITE && (access(path, F_OK) != 0))
    // {
    //     return CP_ERROR_FILE_NOT_EXIST;
    // }
    // else if (mod_map[i].mode == FOT_WRITE && (access(path, F_OK) != 0))
    // {
    //     return CP_ERROR_FILE_NOT_EXIST;
    // }
    
    *fd = fopen(path, mod_map[i].param);

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
    if (oldname == NULL || newname == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    if (rename(oldname, newname) == -1)
    {
        return CP_ERROR_FILE_RENAME_FAILED;
    }

    return CP_ERROR_OK;
}


int cp_file_remove(IN const char* path)
{
    if (path == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    if (remove(path) == -1)
    {
        return CP_ERROR_FILE_REMOVE_FAILED;
    }

    return CP_ERROR_OK;
}


int cp_file_copy(IN const char* srcpath, IN const char* despath, IN bool no_overwrite)
{
    cp_fd fd_src = NULL;
    cp_fd fd_des = NULL;
    int ret = CP_ERROR_OK;
    int readsize = 0;
    int flag_eof = 0;
    char buff[LARGELEN] = { 0 };


    if (srcpath == NULL || despath == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    if (cp_path_exist(srcpath) != CP_ERROR_OK)
    {
        return CP_ERROR_FILE_NOT_EXIST;     // 源文件不存在
    }

    if (cp_path_exist(despath) == CP_ERROR_OK)
    {
        if (no_overwrite == true)
        {
            return CP_ERROR_OK;             // 目的文件存在，而且选择了不覆盖，直接退出。
        }
        else
        {
            ret = cp_file_remove(despath);  // 目的文件存在，选择了覆盖，则先删除已存在的文件。
            if (ret != CP_ERROR_OK)
            {
                return CP_ERROR_FILE_REMOVE_FAILED; 
            }
        }
    }

    ret = cp_file_open(srcpath, FOB_READ, &fd_src);
    if (ret != CP_ERROR_OK)
    {
        goto step_out;
    }
    ret = cp_file_open(despath, FOB_RW, &fd_des);
    if (ret != CP_ERROR_OK)
    {
        goto step_out;
    }

    if (!fd_des || !fd_src)
    {
        goto step_out;
    }

    while (cp_file_read(fd_src, (uint8_t*)buff, BIGLEN, &readsize) == CP_ERROR_OK)
    {
        cp_file_write(fd_des, buff, readsize, NULL);
        cp_file_iseof(fd_src, &flag_eof);
        if (flag_eof == true)
        {
            break;
        }
    }

    
step_out:

    if (fd_src)
    {
        cp_file_close(fd_src);
    }
    if (fd_des)
    {
        cp_file_close(fd_des);
    }

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
