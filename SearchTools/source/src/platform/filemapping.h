

#pragma once

#include <string>

class FileMapping
{
public:
    static const size_t kFileSizeLimit = 1024 * 1024 * 512;  // 512M

public:

    struct MapInfo
    {
        std::string filepath;
        void*       addr;
        size_t      len;
    };
    
public:
    FileMapping();
    ~FileMapping();
    
    void SetFile(const std::string& filepath);
    
    int OpenMapping();
    int Mapping(MapInfo& info);
    int CloseMapping();

private:
    class MappingImpl;

    std::string filepath_;
    size_t filesize_;
    MappingImpl* mapper_;
};

