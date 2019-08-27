#include "file-common.h"

#define TEST_FILE_NAME "testdir/testdir1/testdir2"

#ifndef PATH_MAX 
#define PATH_MAX MAX_PATH
#endif

int test_getexepath()
{
    char path[PATH_MAX];
    int ret = cp_path_getexepath(path, PATH_MAX);
    printf("test get exe path return code = %d\n", ret);
    if (ret != CP_ERROR_OK)
    {
        return ret;
    }

    printf("%s\n", path);
  
    return ret;
}

int test_file_operation()
{
    cp_fd fd;
    
    char buff[1024];
    int filesize;
    int readsize; 
    int ret = cp_file_open(TEST_FILE_NAME, FOB_RW, &fd);
    printf("create file return code = %d\n", ret);
    
    if (ret == CP_ERROR_FILE_NOT_EXIST)
    {
        ret = cp_dir_create("testdir/testdir1");
        printf("create dir return code = %d\n", ret);
        if (ret != CP_ERROR_OK)
        {
            return ret;
        }

        ret = cp_file_open(TEST_FILE_NAME, FOB_RW, &fd);
        printf("open file return code = %d\n", ret);
        if (ret != CP_ERROR_OK)
        {
            return ret; 
        }
    }
    
    if (ret != CP_ERROR_OK)
    {
        return ret; 
    }
    
    ret = cp_file_write(fd, (void*)"xxxxxxxxxxxxxxxxxxyxx", 21, &readsize);
    printf("write file return code = %d\n", ret);
    if (ret != CP_ERROR_OK)
    {
        return ret; 
    }
    
    ret = cp_file_size(fd, &filesize);
    printf("cal file size return code = %d\n", ret);
    if (ret != CP_ERROR_OK)
    {
        return ret; 
    }
    
    // if (filesize != 21)
    // {
    //     printf("cal file size error ! code = %d\n", ret);
    //     return CP_ERROR_FILE_READ_FAILED;
    // }
    
    ret = cp_file_seek(fd, 0, FSK_BEGIN);
    printf("seek file return code = %d\n", ret);
    if (ret != CP_ERROR_OK)
    {
        return ret; 
    }
    
    ret = cp_file_read(fd, (void*)buff, 19, &readsize);
    printf("read file return code = %d\n", ret);
    if (ret != CP_ERROR_OK)
    {
        return ret; 
    }
    
    ret = cp_file_close(fd);
    printf("close file return code = %d\n", ret);
    if (ret != CP_ERROR_OK)
    {
        return ret; 
    }
    
    ret = cp_file_remove(TEST_FILE_NAME);
    printf("remove file return code = %d\n", ret);
    
    return ret;
}


int test_parse_path()
{
    int ret = CP_ERROR_OK;
    char lastname[PATH_MAX];
    char predir[PATH_MAX];
    
    ret = cp_path_lastname(TEST_FILE_NAME, lastname);
    if (ret != CP_ERROR_OK)
    {
        printf("cp_path_lastname error ! code = %d\n", ret);
        return ret; 
    }
    
    printf("%s\n", lastname);
    
    ret = cp_path_predir(TEST_FILE_NAME, predir);
    if (ret != CP_ERROR_OK)
    {
        printf("cp_path_predir error ! code = %d\n", ret);
        return ret; 
    }
   
    printf("%s\n", predir);
    return ret;
}

int test_filecopy()
{
    char content[] = "12345678987654321";
    char srcpath[PATH_MAX];
    char despath[PATH_MAX];
    cp_fd fd;
    int ret = 0;

    cp_path_getexepath(srcpath, PATH_MAX);

    cp_path_predir(srcpath, srcpath);

    strcpy(despath, srcpath);

    strcat(srcpath, "/");
    strcat(srcpath, "testfile.txt");

    ret = cp_file_open(srcpath, FOB_RW, &fd);
    if (ret != CP_ERROR_OK)
    {
        printf("function: test_filecopy, error: %d\n", ret);
        return ret;
    }

    cp_file_write(fd, "12345678987654321", strlen(content), NULL);
    cp_file_close(fd);
    

    strcat(despath, "/");
    strcat(despath, "desfile.txt");
    ret = cp_file_copy(srcpath, despath, true);
    printf("test file copy return code %d\n", ret);

    return ret;
}

int test_listdir()
{
    char path[PATH_MAX];
    dir_handle handle;
    file_info_t fi;
    int ret = 0;



    cp_path_getexepath(path, PATH_MAX);
    cp_path_predir(path, path);


    ret = cp_dir_open(path, &handle, &fi);
    if (ret != CP_ERROR_OK)
    {
        printf("dir open failed!\n");
        return -1;
    }

    while (1)
    {
        if (fi.item_type == FT_DIR)
        {
            printf("file: %s, type: DIRECTORY\n", fi.item_name);
        }
        else
        {
            printf("file: %s, type: normal\n", fi.item_name);
        }

        ret = cp_dir_read(&handle, &fi);
        if (ret != CP_ERROR_OK)
        {
            break;
        }
    }

    cp_dir_close(handle);

    return CP_ERROR_OK;
}


int main()
{
    printf("\n");
    
    test_getexepath();
    test_file_operation();
    test_parse_path();
    test_filecopy();
    test_listdir();
    
    return 0;
}