#pragma once

#include <stdint.h>

class IDataSource
{
public:    
    IDataSource() {}
    virtual ~IDataSource() {}
    
    virtual bool Open() = 0;

    virtual bool PrepareHeader() = 0;

    virtual bool UpdateHeader() = 0;

    virtual bool Writable(size_t size) const = 0;
    
    virtual bool Write(size_t pos, size_t offset, const void* data, size_t size) = 0;
    
    virtual bool MoveData(size_t dst, size_t src, size_t bytes) = 0;
    
    virtual int32_t ReadAsInt32(size_t pos, size_t bytes) = 0;

    virtual size_t ReadAll(void** data) = 0;

    virtual void Close() = 0;

};




