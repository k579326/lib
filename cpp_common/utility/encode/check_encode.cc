
#include <stdbool.h>

#include "check_encode.h"


namespace CheckEncode
{


#define CHECK_TIMES 5

static bool ValidUtf8String(const uint8_t* stream, size_t len, size_t* checkedsize)
{
    if (!stream || !len)
        return false;

    if ((stream[0] & U8_SUB_MASK) == U8_SUB_PREFIX)
        return false;
    
    int checksize = 0;
    
    if ((stream[0] & U8_MAIN_MASK3) == U8_MAIN_PREFIX3)
        checksize = 3;
    else if ((stream[0] & U8_MAIN_MASK2) == U8_MAIN_PREFIX2)
        checksize = 2;
    else if ((stream[0] & U8_MAIN_MASK1) == U8_MAIN_PREFIX1)
        checksize = 1;
    else
        return false;
    
    if (len < (size_t)checksize + 1)
        return false;

    // if return false, this paramter is not used
    if (checkedsize)
        *checkedsize = (size_t)checksize + 1;

    while (checksize) {
        if ((stream[checksize--] & U8_SUB_MASK) != U8_SUB_PREFIX)
            return false;
    }
    return true;
}


EncodeType Check(const uint8_t* stream, size_t len)
{
    if (!stream || !len) {
        // empty file
        return ANSI;
    }
    
    // for utf-8,utf-8-BOM,utf16, the two bits of stream header can not be U8_SUB_PREFIX
    if ((stream[0] & U8_SUB_MASK) == U8_SUB_PREFIX)
        return ANSI;
    
    if (len == 1) 
    {
        return ANSI;
    }
    
    // now, len is not less than 2
    
    if (*(uint16_t*)stream == 0xFFFE)
        return UTF16_BE;
    if (*(uint16_t*)stream == 0xFEFF)
        return UTF16_LE;
    
    
    if (len == 2) 
    {
        if ((stream[0] & U8_MAIN_MASK1) != U8_MAIN_PREFIX1)
            return ANSI;
        else if ((stream[0] & U8_SUB_MASK) == U8_SUB_PREFIX)
            return UTF8;
    }
    
    // now, len is not less than 3
    if (stream[0] == 0xEF && stream[1] == 0xBB && stream [2] == 0xBF)
        return UTF8_BOM;
    
    // on there, the encode of stream can't be utf16 and utf-8-BOM
    // how to check whether it is utf-8 or ANSI(GBK)?
    //

    int checktimes = 0;
    int index = 0;

    // we repeat checking five times, in case of characters like 'ÁªÍ¨' cause mistake.
    while (checktimes++ < CHECK_TIMES)
    {
        while (index < len)
        {
            if ((uint8_t)stream[index] >= 0x80)
                break;
            index++;
        }

        if (index >= len)
            return !checktimes ? ANSI : UTF8;

        size_t checkedsize = 0;
        if (!ValidUtf8String(stream + index, len - index, &checkedsize))
            return ANSI;

        index += checkedsize;
    }

    return UTF8;
}

  
};
