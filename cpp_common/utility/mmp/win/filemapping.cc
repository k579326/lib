
#include "filemapping.h"


#include <Windows.h>
#include <WinBase.h>

#include <assert.h>

class FileMapping::MappingImpl
{
public:
    MappingImpl() = default;
    ~MappingImpl()
    {
        Close();
    }

    int OpenMapping(HANDLE fd, OprModel model, size_t size)
    {
        Close();

        DWORD page_protect = 0;
        if (model == OprModel::ReadOnly)
            page_protect = PAGE_READONLY;
        else if (model == OprModel::RdWr)
            page_protect = PAGE_READWRITE;
        else
            assert(false);

        // CreateFileMapping失败返回NULL，而非INVALID_HANDLE_VALUE
        HandleOfMapping_ = CreateFileMapping(fd, 
            NULL, 
            page_protect,
            0, 
            0,
            NULL
            );
        if (HandleOfMapping_ == INVALID_HANDLE_VALUE) {
            return -1;
        }

        offset_ = 0;
        filesize_ = size;
        offset_ = 0;

        return 0;
    }
    int DoMapping(size_t* mapped_size, OprModel model)
    {
        size_t count = (maxbytes_eachmap_ > filesize_ - offset_) ? 
            filesize_ - offset_  : maxbytes_eachmap_;
        if (address_ != NULL) {
            UnmapViewOfFile(address_);
            address_ = NULL;
        }

        if (filesize_ == offset_)
            return 0;

        DWORD operation_mode = FILE_MAP_READ;
        if (model == OprModel::RdWr)
            operation_mode |= FILE_MAP_WRITE;

        address_ = MapViewOfFile(HandleOfMapping_, operation_mode, 0, offset_, count);
        if (address_ != NULL) {
            offset_ += count;
            *mapped_size = count;
            return 0;
        }
        return -1;
    }
    
    void Close()
    {
        if (address_ != NULL)
        {
            UnmapViewOfFile(address_);
            address_ = NULL;
        }
        if (HandleOfMapping_ != 0)
        {
            CloseHandle(HandleOfMapping_);
            HandleOfMapping_ = 0;
        }
    }

    void* GetAddress() const { return address_; }

private:
    void* address_ = NULL;
    size_t offset_ = 0;
    size_t filesize_ = 0;
    HANDLE HandleOfMapping_ = 0;
    const size_t maxbytes_eachmap_ = kFileSizeLimit;   
};

FileMapping::FileMapping()
{
    mapper_ = new MappingImpl;
    filesize_ = 0;
}


FileMapping::~FileMapping()
{
    if (mapper_ != NULL)
        delete mapper_;
}

int FileMapping::OpenMapping(void* fd, OprModel model, size_t filesize)
{
    return mapper_->OpenMapping(fd, model, filesize);
}

int FileMapping::Mapping(MapInfo& info, OprModel model)
{
    info.len = 0;
    info.addr = NULL;

    int ret = mapper_->DoMapping(&info.len, model);
    if (ret != 0)
        return ret;

    info.addr = mapper_->GetAddress();
    return 0;
}

int FileMapping::CloseMapping()
{
    mapper_->Close();
    return 0;
}

