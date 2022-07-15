

#pragma once

#include <string>

class FileMapping
{
public:
    static const size_t kFileSizeLimit = 1024 * 1024 * 512;  // 512M

public:
    enum class OprModel
    {
        ReadOnly = 0,
        RdWr,
    };
    struct MapInfo
    {
        void*       addr;
        size_t      len;
    };
    
public:
    FileMapping();
    ~FileMapping();
#if defined WIN32 || defined _WIN32
    int OpenMapping(void* fd, OprModel model, size_t filesize);
#else
    int OpenMapping(int fd, size_t filesize);
#endif
    int Mapping(MapInfo& info, OprModel model);
    int CloseMapping();

private:
    class MappingImpl;

    size_t filesize_;
    MappingImpl* mapper_;
};

