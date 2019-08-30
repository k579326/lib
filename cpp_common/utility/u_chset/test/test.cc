

#include <iostream>

#include "util_chset_cvt.h"


static std::string AnsiConvertToUtf8(const std::string& ansi)
{
    std::wstring wstr = chsetutil::LocalToWideString(ansi, "chs");

    return chsetutil::WideStringToUTF8(wstr);
}


static std::string Utf8ConvertToAnsi(const std::string& utf8)
{
    std::wstring wstr = chsetutil::UTF8ToWideString(utf8);
    return chsetutil::WideStringToLocal(wstr, "chs");
}

int main()
{
    std::string ansi = "²âÊÔÊı¾İ×Ö·û´®£¬£¿ÁùÁùÁù";

    std::string utf8 = AnsiConvertToUtf8(ansi);
    std::string restore = Utf8ConvertToAnsi(utf8);

    if (ansi != restore) {
        std::cout << "" << std::endl;
        return -1;
    }


    return 0;
}