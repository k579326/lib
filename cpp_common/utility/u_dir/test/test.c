
#include "util_dir.h"
#include <stdio.h>
#include <string.h>


int ListDir(const char* path)
{
    int ret = 0;
    struct dirent* info = NULL;
    DIR* dir = opendir(path);

    if (!dir)
    {
        return -1;
    }

    printf("List Directory %s\n", path);

    while (1)
    {
        char curpath[1024] = { 0 };

        info = readdir(dir);
        if (info == NULL)
        {
            break;
        }

        if (0 == strcmp(info->d_name, ".") || 0 == strcmp(info->d_name, "..")) {
            continue;
        }


        printf("    find %4s named %s\n", info->d_type == DT_DIR ? "DIR" : "FILE", info->d_name);

        if (info->d_type == DT_DIR)
        {
            strcat(curpath, path);
            strcat(curpath, "\\");
            strcat(curpath, info->d_name);
            ListDir(curpath);
        }

    }
    
    ret = closedir(dir);
    CommIsDir("E:\\restructur");
    return ret;
}


int main()
{
    int err = ListDir("E:\\restructure");
	
    err = CommRemoveDir("E:\\testRemove\\remove", true);
	
    return 0;
}