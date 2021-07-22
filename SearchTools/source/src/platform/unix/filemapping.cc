

#include "filemapping.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "u_dir/util_dir.h"
#include "search_err.h"


class FileMapping::MappingImpl
{
public:
    MappingImpl() = default;
    ~MappingImpl() 
    {
        if (address_ || address_ == MAP_FAILED) {
            munmap(address_, last_mappint_size);
        }
        if (fd_ > 0)
            close(fd_);
    }
    
    int OpenMapping(const std::string& file, size_t size)
    {
        // 暂时不分段处理大文件，后续再说
        if (size > maxbytes_eachmap_)
            return SRH_INVAL;
        
        int fd = open64(file.c_str(), 
            O_RDONLY | O_EXCL | O_LARGEFILE | O_BINARY,
            0400);
        if (fd < 0) {
            return SRH_FILEOPR;
        }
        
        fd_ = fd;
        filesize_ = size;
        return SRH_OK;
    }
    
    int DoMapping(size_t* mapped_size)
    {
        size_t realsize = 0;
        if (filesize_ == offset_) {
            munmap(address_, filesize_);
            *mapped_size = 0;
            address_ = nullptr;
            return SRH_OK;
        }
        
        if (filesize_ - offset_ > maxbytes_eachmap_) {
            realsize = maxbytes_eachmap_;
        }
        else
            realsize = filesize_ - offset_;
       
        void* address = mmap(0, realsize, PROT_READ, MAP_PRIVATE, fd, offset_);
        if (address == MAP_FAILED) {
            return SRH_FILEOPR;
        }
        
        address_ = address;
        *mapped_size = realsize;
        last_mappint_size = realsize;
        offset_ += realsize;
        
        return SRH_OK;
    }
    
    
    void* GetAddress() const { return address_; }
    
private:
    void* address_ = NULL;
    size_t offset_ = 0;
    size_t filesize_ = 0;
    size_t last_mappint_size = 0;   // recode last length of mapping
    int fd_ = -1;
    const size_t maxbytes_eachmap_ = kFileSizeLimit;
}



FileMapping::FileMapping()
{
    mapper = new MappingImpl;
    filepath_ = "";
    filesize_ = 0;
}

~FileMapping::FileMapping()
{
    if (mapper_ != NULL)
        delete mapper_;
}

void FileMapping::SetFile(const std::string& filepath)
{
    if (filepath.empty())
        return;

    if (CommIsDir(filepath.c_str()))
        return;

    struct stat info;
    if (stat(filepath.c_str(), &info)) {
        return;
    }

    filesize_ = info.st_size;
    filepath_ = filepath;
    return;
}

int FileMapping::OpenMapping()
{
    return mapper_->OpenMapping(filepath_, filesize_);
}

int FileMapping::Mapping(MapInfo& info)
{
    info.len = 0;
    info.filepath = "";
    info.addr = NULL;

    int ret = mapper_->DoMapping(&info.len);
    if (ret != SRH_OK)
        return ret;

    info.addr = mapper_->GetAddress();
    info.filepath = filepath_;

    return SRH_OK;
}

int FileMapping::CloseMapping()
{
    delete mapper_;
    mapper_ = NULL;
    return SRH_OK;
}




