#pragma once


#include <memory>
#include <map>


#define XDATA_STREMA_HEADER_VERSION 1

class DataEncoder
{
public:
    using XDataStream = std::pair<char*, size_t>;
    static int encode(const class XData* data, XDataStream& stream);
    static int decode(const XDataStream& stream, XData** data);
    static size_t Size(const XData* data);

    class StreamHeader
    {
    public:
        uint32_t version;
        uint32_t length;
        uint8_t  reserved[16];
    };

private:
    static XDataStream _decode(const XDataStream& stream, XData** data);

};



