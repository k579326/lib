
#include <stdio.h>
#include <string.h>

#include <string>
#include <queue>
#include <map>

#include "encode/check_encode.h"
#include "dir/util_dir.h"

static std::map<EncodeType, const char*> s_encodemap =
{
    {UTF8, "UTF-8"}, 
    {UTF8_BOM, "UTF-8 BOM"},
    {UTF16_LE, "UTF16 Little Endian"},
    {UTF16_BE, "UTF16 Big Endian"},
    {ANSI, "GBK"},
    {BadStream, "Unknown"},
};



static void CheckFiles(std::queue<std::string>& dirlist)
{
    const char* dirpath = dirlist.front().c_str();
    DIR* dir = opendir(dirpath);
    if (!dir)
    {
        printf("open dir failed!\n");
        return ;
    }

    char filepath[256] = {};
    uint8_t buf[1024 * 10];

    while (true)
    {
        dirent* file = readdir(dir);
        if (!file) {
            return ;
        }

        if (strcmp(file->d_name, ".") == 0 ||
            strcmp(file->d_name, "..") == 0)
            continue;

        strcpy(filepath, dirpath);
        strcat(filepath, "\\");
        strcat(filepath, file->d_name);
        if (DT_DIR == file->d_type) {
            dirlist.push(filepath);
            continue;
        }
        else if (DT_REG != file->d_type)
            continue;

        FILE* fp = fopen(filepath, "rb");
        size_t readsize = fread(buf, 1, 1024 * 10, fp);
        if (readsize > 0) {
            EncodeType type = CheckEncode::Check(buf, readsize);
            printf("file %s is %s \n", filepath, s_encodemap[type]);
        }
        fclose(fp);
    }

    closedir(dir);
}

int main()
{
    char dirpath[] = "F:\\work\\sanguo_game\\helper";

    std::queue<std::string> dirlist;
    dirlist.push(dirpath);

    while (!dirlist.empty()) {
        CheckFiles(dirlist);
        dirlist.pop();
    }

    return 0;
}