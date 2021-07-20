

#pragma once

#include <string>

class MappingID;

class FileMapping
{
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
    std::string filepath_;
    size_t filesize_;
    MappingID* pID_;
};

