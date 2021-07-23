#pragma once



enum EncodeType : uint8_t
{
    UTF8,
    UTF8_BOM,               // EF BB BF as header
    UTF16_LE,
    UTF16_BE,
    ANSI,

    BadStream = 255,
};




#define U8_SUB_PREFIX   0x80
#define U8_SUB_MASK     0xC0

#define U8_MAIN_PREFIX1 0xC0
#define U8_MAIN_MASK1   0xE0

#define U8_MAIN_PREFIX2 0xE0
#define U8_MAIN_MASK2   0xF0

#define U8_MAIN_PREFIX3 0xF0
#define U8_MAIN_MASK3   0xF8








