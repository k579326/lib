#include <dirent.h>

#include "file-common.h"



static int create_dir_recursively(const char* path)
{
    char cur_path[PATH_MAX] = { 0 };
    char* next_begin = NULL;

    int i = 0;
    while (path[i] != '/' && path[i] != '\0')
    {
        i++;
    }

    // 注意以根目录开始的路径
    memcpy(cur_path, path, i == 0 ? 1 : i);

    umask(0);
    if (access(cur_path, F_OK) != 0)
    {
        if (mkdir(cur_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)        // chmod 775
        {
            return CP_ERROR_DIR_CREATE_FAILED;
        }
    }

    if (chdir(cur_path) == -1)
    {
        return CP_ERROR_DIR_ENTER_FAILED;
    }

    // 此处就是递归完成的返回点。
    if (path[i] == '\0')
    {
        return CP_ERROR_OK;
    }

    // 对于这样的路径 /usr/local////test, 应该也是可以创建的，所以进行处理。
    while (path[i] != '\0' && path[i] == '/')
    {
        i++;
    }

    // 此处就是递归完成的返回点。
    if (path[i] == '\0')
    {
        return CP_ERROR_OK;
    }

    // 保证每次递归的入参路径总是以路径名开始（不带路径分隔符，但是可以是根目录开头）
    next_begin = path + i;

    return create_dir_recursively(next_begin);
}


int cp_dir_create(const char* path)
{
    int ret = CP_ERROR_OK;
    char cur_path[PATH_MAX];
    
    if (path == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    if (access(path, F_OK) == 0)
    {
        return CP_ERROR_OK;
    }
    
    // 记录当前路径，在create_dir_recursively中会修改当前路径
    if (NULL == getcwd(cur_path, PATH_MAX))
    {
        return CP_ERROR_DIR_CREATE_FAILED;
    }
    
    // create dir
    ret = create_dir_recursively(path);
    
    // 返回到当前路径
    if (chdir(cur_path) == -1)
    {
        return CP_ERROR_DIR_CREATE_FAILED;
    }
        
    return ret;
}

int cp_dir_specialfolder(char* path, file_sf_em folder_type, bool fcreate)
{
    int index = 0;
    int retcode = CP_ERROR_OK;

    if (path == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    retcode = cp_path_getexepath(path, PATH_MAX);       // 这个长度需要再商榷,应该外面传进来 guok
    if (retcode)
    {
        return CP_ERROR_PATH_GETEXEPATH_FAILED;
    }

    return cp_path_predir(path, path);
}

int cp_dir_open(const char* path, dir_handle* handle, file_info_t* di)
{
    struct dirent* fattr;

    if (path == NULL || handle == NULL || di == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    *handle = opendir(path);

    if (*handle == NULL)
    {
        return CP_ERROR_DIR_OPEN_FAILED;
    }

    // 为了和win接口一致，所以第一次打开的时候就读一次
    fattr = readdir(*handle);
    if (fattr == NULL)
    {
        closedir(*handle);                 // 读取失败，要当做打开失败来对待，要注意关闭句柄。
        *handle = NULL;
        return CP_ERROR_DIR_OPEN_FAILED;
    }

    strcpy(di->item_name, fattr->d_name);
    if (fattr->d_type == DT_DIR)
    {
        di->item_type = FT_DIR;
    }
    else /*if (fattr->d_type == S_IFREG)*/
    {
        di->item_type = FT_NORMAL;
    }


    return CP_ERROR_OK;
}

int cp_dir_read(dir_handle* handle, file_info_t* di)
{
    bool bret = false;
    struct dirent* fattr;

    if (handle == NULL || di == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    fattr = readdir(*handle);
    if (fattr == NULL)
    {
        // closedir(*handle);           // 留给调用者关闭
        // *handle = NULL;
        return CP_ERROR_DIR_READ_FAILED;
    }

    strcpy(di->item_name, fattr->d_name);
    if (fattr->d_type == DT_DIR)
    {
        di->item_type = FT_DIR;
    }
    else /*if (fattr->d_type == S_IFREG)*/
    {
        di->item_type = FT_NORMAL;
    }

    return CP_ERROR_OK;
}

int cp_dir_close(dir_handle handle)
{
    if (handle == NULL)
    {
        return CP_ERROR_INVALID_PARAM;
    }

    closedir(handle);

    return CP_ERROR_OK;
}