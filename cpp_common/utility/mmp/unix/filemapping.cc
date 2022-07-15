

#include "filemapping.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>


class FileMapping::MappingImpl
{
public:
    MappingImpl() = default;
    ~MappingImpl() 
    {
        if (address_ || address_ == MAP_FAILED) {
            munmap(address_, last_mappint_size_);
        }
        if (fd_ >= 0)
            close(fd_);
    }
    
    int OpenMapping(int fd, size_t size)
    {
        fd_ = fd;
        filesize_ = size;
        offset_ = 0;
        
        return 0;
    }
    
    int DoMapping(size_t* mapped_size, OprModel model)
    {
        size_t realsize = 0;
        if (filesize_ == offset_) {
            munmap(address_, filesize_);
            *mapped_size = 0;
            address_ = nullptr;
            return 0;
        }
        
        if (filesize_ - offset_ > maxbytes_eachmap_) {
            realsize = maxbytes_eachmap_;
        }
        else
            realsize = filesize_ - offset_;
       
        int operation_mode = PROT_READ;
        if (model == OprModel::RdWr)
            operation_mode |= PROT_WRITE;

        void* address = mmap(0, realsize, operation_mode, MAP_SHARED, fd_, offset_);
        if (address == MAP_FAILED) {
            return -1;
        }
        
        address_ = address;
        *mapped_size = realsize;
        last_mappint_size_ = realsize;
        offset_ += realsize;
        
        return 0;
    }
    
    void Close()
    {
        if (address_)
        {
            munmap(address_, last_mappint_size_);
            last_mappint_size_ = 0;
            fd_ = -1;
            filesize_ = 0;
            offset_ = 0;
            address_ = nullptr;
        }
    }
    
    void* GetAddress() const { return address_; }
    
private:
    void* address_ = NULL;
    size_t offset_ = 0;
    size_t filesize_ = 0;
    size_t last_mappint_size_ = 0;   // recode last length of mapping
    int fd_ = -1;
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

int FileMapping::OpenMapping(int fd, size_t filesize)
{
    return mapper_->OpenMapping(fd, filesize);
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
    return 0;
}




