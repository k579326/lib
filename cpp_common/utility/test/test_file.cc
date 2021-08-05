


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

    if (file.Open(kCreate, kRdWr, 0600) != 0)
    {
        printf("open failed!\n");
        return -1;
    }
    if (file.Seek(1024, kSeekSet) != 0) {
        printf("seek failed! \n");
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
    printf("test pass!");

    return 0;
}