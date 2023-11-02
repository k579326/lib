
#pragma once

#include <string>

#include <datasource.h>
#include "cpt-string.h"

namespace filemap
{
    class PhyFileMap;
}

class FileSource : public IDataSource
{
public:
    FileSource();
    virtual ~FileSource();

    bool Init(const CptString& path);

    virtual bool Open() override;
    virtual bool PrepareHeader() override;
    virtual bool Writable(size_t size) const override;
    virtual bool Write(size_t pos, size_t offset, const void* data, size_t size) override;
    virtual bool MoveData(size_t dst, size_t src, size_t bytes) override;
    virtual int32_t ReadAsInt32(size_t pos, size_t bytes) override;
    virtual size_t ReadAll(void** data) override;
    virtual void Close() override;

private:
    virtual bool UpdateHeader() override;

private:

    filemap::PhyFileMap* fmap_;

    CptString file_path_;
    void*   base_;
    char*   content_;
    size_t  cur_offset_;
    size_t  total_size_;
};






