
#include <assert.h>

#include "u_path//util_path.h"
#include "u_file/util_file.h"

using namespace fileutil;
using namespace pathutil;

int main()
{
    std::string filename;
    std::string backupfile;
    File file("./test");
    filename = file.Name();

    if (file.Open(kCreateForce, kRdWr, 0600) != 0)
    {
        printf("open failed!\n");
        return -1;
    }
    
    if (file.Write((const unsigned char*)"123456789", 9) != 0) {
        printf("Write failed! \n");
        return -1;
    }
    if (file.Seek(-9, kSeekCur) != 0) {
        printf("Seek failed!\n");
        return -1;
    }

    std::string buf;
    if (file.Read(&buf, 9) != 0) {
        printf("Read failed!\n");
        return -1;
    }
    if (buf.compare(0, 9, "123456789") != 0) {
        printf("Read content isn't expect!\n");
        return -1;
    }
    file.Close();

    if (!File::IsNormalFile(filename))
    {
        printf("Check file failed!\n");
        return -1;
    }

    File::Rename(filename, "newtest");
    std::string dir = pathutil::GetDirOfPathName(filename);
    backupfile = PathCombines(dir, "newtest");
    if (!File::IsExist(backupfile)) {
        printf("Rename Or IsExist failed!\n");
        return -1;
    }

    File::Copy(backupfile, filename, true);
    if (!File::IsExist(filename)) {
        printf("Copy Or IsExist failed!\n");
        return -1;
    }

    File::Remove(filename);
    if (File::IsExist(filename)) {
        printf("Remove failed!\n");
        return -1;
    }

    uint64_t size = 1024 * 1024 * 1024 * 10llu;
    if (File::Truncate(backupfile, size) != 0) {
        printf("Truncate failed!\n");
        return -1;
    }
    if (File::Size(backupfile) != size) {
        printf("Size failed!\n");
        return -1;
    }

    File newfile(backupfile.c_str());
    if (newfile.Open(kCreateForce, kRdWr, 0600) != 0) {
        printf("Create file which is Existed failed!\n");
        return -1;
    }
    newfile.Close();
    if (File::Size(backupfile) != 0) {
        printf("Create file which is Existed exception!\n");
        return -1;
    }

    File::Remove(backupfile);

    // test create
    {
        File f(filename.c_str());
        assert(f.Open(kCreate, kRdWr, 0600) == 0);
        f.Write("aaaaaaaaaaaaaaaaaaaaaaaaaa", 27);
        f.Close();

        if (f.Open(kCreate, kRdWr) == 0)
        {
            printf("Create should not open file\n");
            return -1;
        }

        // openalways
        std::string content;
        if (f.Open(kOpenForce, kReadOnly, 0600) != 0) {
            printf("kOpenAlways failed!\n");
            return -1;
        }
        f.Read(&content, 128);
        if (content.size() != 27) {
            printf("kOpenAlways read failed!\n");
            return -1;
        }
        f.Close();


        // CreateForce
        if (f.Open(kCreateForce, kReadOnly, 0600) == 0) {
            printf("kCreateForce should not create file with readonly model!\n");
            return -1;
        }
        if (f.Open(kCreateForce, kRdWr, 0600) != 0) {
            printf("kCreateForce should not create file with readonly model!\n");
            return -1;
        }
        
        f.Read(&content, 27);
        if (content.size() != 0) {
            printf("kCreateForce read failed!\n");
            return -1;
        }
        f.Close();


        // Open Exist
        if (f.Open(kOpen, kWriteOnly, 0) != 0) {
            printf("kOpenExist failed! \n");
            return -1;
        }
        f.Close();
        if (File::Remove(filename) != 0) {
             printf("remove file failed!\n");
             return -1;
        }
    }

    // test append model
    {
        File file(filename.c_str());
        if (0 == file.Open(kAppend, kRdWr)) {
            printf("Append should not create file!\n");
            return -1;
        }

        if (0 != file.Open(kAppendForce, kRdWr)) {
            printf("AppendForce should create file always!\n");
            return -1;
        }
        if (file.Write("123456789", 9) != 0) {
            printf("Append model write failed!\n");
            return -1;
        }
        file.Close();

        if (0 != file.Open(kAppend, kRdWr)) {
            printf("Append model open failed!\n");
            return -1;
        }
        if (File::Size(filename) != 9) {
            printf("Append model open execption!\n");
            return -1;
        }

        if (file.Write("123456789", 9) != 0) {
            printf("Append model write failed!\n");
            return -1;
        }
        file.Close();

        std::string append_readbuf;
        if (0 != file.Open(kAppend, kRdWr)) {
            printf("Append model open failed!\n");
            return -1;
        }
        if (file.Read(&append_readbuf, 18) != 0) {
            printf("Append model read failed!\n");
            return -1;
        }
        if (append_readbuf != "123456789123456789") {
            printf("Append model check failed!\n");
            return -1;
        }
        file.Close();
        File::Remove(filename);
    }

    // test large file
    {
        File f(filename.c_str());
        f.Open(kCreate, kReadOnly);
        f.Close();

        if (0 != File::Truncate(filename, 1024 * 1024 *1024 * 5llu))
        {
            printf("Truncate large file error\n");
            return -1;
        }

        if (f.Open(kOpen, kReadOnly) != 0) {
            printf("Open Large file error!\n");
            return -1;
        }
        if (f.Seek(1024*1024*1024*4ll, kSeekSet) != 0) {
            printf("Seek Large file error\n");
            return -1;
        }
        f.Close();

        if (File::Size(filename) != 1024 * 1024 *1024 * 5llu) {
            printf("large file size is not correct!\n");
            return -1;
        }
        File::Remove(filename);
    }

    printf("all test pass!\n");

    return 0;
}