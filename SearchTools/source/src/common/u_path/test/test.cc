
#include <iostream>

#include "util_path.h"




static void PathConvertTest()
{
    std::string win_path = "C:\\Local Test\\test1\\test";
    std::string unix_path = "/usr/local/bin/test";

    pathutil::PathStyleConvert(win_path, pathutil::PathStyle::kUnix);
    pathutil::PathStyleConvert(unix_path, pathutil::PathStyle::kAuto);

    std::cout << "windows path convert to unix style: " << win_path << std::endl;
    std::cout << "unix style path convert to current platform style: " << unix_path << std::endl;
}


static void GetDirOfPath()
{
    std::string test_path = "/usr/local/bin/test";

    std::string dir = pathutil::GetDirOfPathName(test_path);

    std::cout << "path: " << test_path << " dir: " << dir << std::endl;

    return;
}




int main()
{
    PathConvertTest();
    GetDirOfPath();

    return 0;
}






