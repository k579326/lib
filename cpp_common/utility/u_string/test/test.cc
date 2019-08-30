

#include <iostream>
#include "util_string.h"



int HexConvertTest()
{
    std::string bin = "\x01\x02\x03\x04\x05\x06\xA1\xa1\xFF\xDd";

    std::string lowerhex = stringutil::BinToHexstr(bin, false);
    std::string upperhex = stringutil::BinToHexstr(bin, true);

    stringutil::ToLower(upperhex);
    if (upperhex != lowerhex) {
        std::cout << "BinToHexstr match case error!" << std::endl;
    }

    std::string bin_cvt = stringutil::HexstrToBin(lowerhex);

    if (bin != bin_cvt) {
        std::cout << "BinToHexstr and  HexstrToBin function error!" << std::endl;
        return -1;
    }


    std::string invalidHex0 = "1A2B3C4D5F6E1";   // ÆæÊý¸öhex×Ö·û
    std::string invalidHex1 = "123asfee"; // ÓÐÎÞÐ§×Ö·û
    if (!stringutil::HexstrToBin(invalidHex0).empty())
    {
        std::cout << "ÆæÊý¸öhex×Ö·û²âÊÔÊ§°Ü" << std::endl;
        return -1;
    }
    if (!stringutil::HexstrToBin(invalidHex1).empty())
    {
        std::cout << "ÎÞÐ§hex×Ö·û²âÊÔÊ§°Ü" << std::endl;
        return -1;
    }


    std::string matchcase = "aA1B8C9c66E3f6";
    if (stringutil::HexstrToBin(matchcase).empty())
    {
        std::cout << "ºöÂÔ´óÐ¡Ð´²âÊÔÊ§°Ü" << std::endl;
        return -1;
    }


    return 0;
}


static int SpliteTest()
{
    char separator1 = ',';
    std::string teststr1 = "1asd,w234,ooerw,,wqeq,";

    std::string separator2 = "==";
    std::string teststr2 = "1asd==w234==ooerw====wqeq==";

    auto strlist1 = stringutil::Split(teststr1, separator1);
    auto strlist2 = stringutil::Split(teststr2, separator2);

    if (strlist1 != strlist2 || strlist1.size() != 6) {
        std::cout << "Split test failed!" << std::endl;
        return -1;
    }

    return -1;
}

static int ReplaceTest()
{
    std::string sourcestr = "ab4a85asdf7asdresrd5sdfasf4asddfa";
    std::string expectstr = "ab4a85???f7???resrd5sdfasf4???dfa";
    std::string largestr = sourcestr;
    std::string target = "asd";
    std::string replacement = "???";

    stringutil::ReplaceAll(largestr, target, replacement);

    if (expectstr != largestr) {
        std::cout << "ReplaceAll test error:" << std::endl;
        std::cout << "source string " << sourcestr << std::endl;
        std::cout << "string be replace " << largestr << std::endl;
    }

    return 0;
}

static int LowerAndUpperConvert()
{
    std::string test_str("aAbBCCdDddPIsoSIsdoDoasDPsAPSo");
    std::string backup_str(test_str);

    std::string upper_str("AABBCCDDDDPISOSISDODOASDPSAPSO");
    std::string lower_str("aabbccddddpisosisdodoasdpsapso");

    stringutil::ToLower(test_str);
    stringutil::ToUpper(backup_str);

    if (test_str != lower_str || backup_str != upper_str) {
        std::cout << "upper or lower test failed!" << std::endl;
        return -1;
    }


    return 0;
}


int main()
{
    HexConvertTest();

    SpliteTest();

    ReplaceTest();

    LowerAndUpperConvert();


    return 0;
}