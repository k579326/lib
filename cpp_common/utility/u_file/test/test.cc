
#include <iostream>
#include "util_file.h"



static int testrw()
{
    int err;
    fileutil::File f("E:\\Test\\testrwfile");


    err = f.Open("wb");
    if (err != 0) {

        return err;
    }

    unsigned char testdata[] = "\x1\x2\x3\x4\x5\x6\x7\x8\x9\x10\x11\x12\x13\x14\x15\x16\x17\x18";
    uint32_t size = sizeof(testdata);
    err = f.Write(testdata, size);
    if (err != 0 || size != sizeof(testdata)) {

        return -1;
    }

    f.Close();

    if (fileutil::File::Size("E:\\Test\\testrwfile") != sizeof(testdata))
    {
        return -1;
    }

    std::string readdata;
    err = f.Open("rb");
    if (err != 0)
        return err;
    err = f.Read(&readdata, 100);
    if (err != 0)
        return err;

    if (readdata != std::string((char*)testdata, sizeof(testdata)))
    {
        return -1;
    }

    return 0;
}


static int testcopy()
{
    
    int err;

    fileutil::File::Copy("E:\\Test\\testrwfile", "E:\\Test\\testrwfile1", false);
    fileutil::File::Copy("E:\\Test\\testrwfile1", "E:\\Test\\testrwfile2", false);
    fileutil::File::Copy("E:\\Test\\testrwfile2", "E:\\Test\\testrwfile", false);

    fileutil::File::Remove("E:\\Test\\testrwfile");
    fileutil::File::Remove("E:\\Test\\testrwfile1");
    fileutil::File::Remove("E:\\Test\\testrwfile2");

    if (fileutil::File::IsExist("E:\\Test\\testrwfile") ||
        !fileutil::File::IsExist("E:\\Test\\testrwfile1") ||
        !fileutil::File::IsExist("E:\\Test\\testrwfile2")
        )
    {
        return -1;
    }

    return 0;
}


static int TestTruncate()
{ 
    
    fileutil::File f("E:\\Test\\empty");

    f.Open("wb");
    f.Close();

    if (!fileutil::File::Truncate("E:\\Test\\empty", 1024 * 1024 * 100)) {
        return -1;
    }
    if (!fileutil::File::Truncate("E:\\Test\\empty", 1024 * 1024 * 10)) {
        reutrn -1;
    }

    return 0;
}

int main()
{
    if (testrw()) {
        return -1
    }
    if (testcopy()) {
        return -1;
    }
    if (TestTruncate()) {
        return -1;
    }

    return 0;
}