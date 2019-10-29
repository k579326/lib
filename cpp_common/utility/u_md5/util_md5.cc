
#include "util_md5.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory>

#include "u_string/util_string.h"
#include "md5/md5.h"



namespace md5util
{

// md5
std::string Md5OfBlock(const std::string& content, bool upper)
{
    std::string byte_md5;
    MD5_CTX ctx;
    unsigned char digest[16];
    MD5Init(&ctx);
    MD5Update(&ctx, (unsigned char*)content.c_str(), content.size());
    MD5Final(&ctx, digest);
    
    byte_md5.assign((char*)digest, 16);
    return stringutil::BinToHexstr(byte_md5, upper);
}

std::string Md5OfFile(const std::string& filepath, bool upper)
{
    const int kBlockSize = 1024 * 1024;

    FILE* fp = fopen(filepath.c_str(), "rb");
    if (!fp) {
        return "";
    }
    
    std::string byte_md5;
    MD5_CTX ctx;
    unsigned char digest[16];

    std::shared_ptr<char> buf(new char[kBlockSize], [](char* p)->void{ delete[] p; });
    int actulsize;
    
    MD5Init(&ctx);
    while (1)
    {
        actulsize = fread(buf.get(), 1, kBlockSize, fp);
        if (actulsize != kBlockSize && !feof(fp)) 
        {
            fclose(fp);
            return "";
        }
        
        MD5Update(&ctx, (unsigned char*)buf.get(), actulsize);
        
        if (feof(fp)) {
            break;
        }
    }
    
    MD5Final(&ctx, digest);
    fclose(fp);
    
    byte_md5.assign((char*)digest, 16);
    return stringutil::BinToHexstr(byte_md5, upper);
}

    
};