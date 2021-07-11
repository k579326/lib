
#include "filemapping.h"


#include <Windows.h>
#include <WinBase.h>

#include <sys/stat.h>

#include "file/util_dir.h"
#include "search_err.h"


class MappingID
{
public:
    MappingID() = default;
    ~MappingID() 
    {
        if (address_ != NULL)
            UnmapViewOfFile(address_);
        if (HandleOfMapping_ != INVALID_HANDLE_VALUE)
            CloseHandle(HandleOfMapping_);
        if (HandleOfFile_ != INVALID_HANDLE_VALUE)
            CloseHandle(HandleOfFile_);
    }

    int OpenMapping(const std::string& file, size_t size)
    {
        HandleOfFile_ = CreateFileA(file.c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL, 
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            0);
        if (HandleOfFile_ == INVALID_HANDLE_VALUE) {
            return SRH_FILEOPR;
        }

        HandleOfMapping_ = CreateFileMappingA(HandleOfFile_, 
            NULL, 
            PAGE_READONLY,
            0, 
            0,
            "SearchTools"
            );
        if (HandleOfMapping_ == INVALID_HANDLE_VALUE) {
            return SRH_FILEOPR;
        }

        offset_ = 0;
        filesize_ = size;
        return SRH_OK;
    }
    int DoMapping(size_t* mapped_size)
    {
        size_t count = (maxbytes_eachmap_ > filesize_ - offset_) ? 
            filesize_ - offset_  : maxbytes_eachmap_;
        if (address_ != NULL) {
            UnmapViewOfFile(address_);
            address_ = NULL;
        }

        if (filesize_ == offset_)
            return SRH_OK;

        address_ = MapViewOfFile(HandleOfMapping_, FILE_MAP_READ, 0, offset_, count);
        if (address_ != NULL) {
            offset_ += count;
            *mapped_size = count;
            return SRH_OK;
        }
        return SRH_FILEOPR;
    }
    
    void* GetAddress() const { return address_; }

private:
    void* address_ = NULL;
    size_t offset_ = 0;
    size_t filesize_ = 0;
    HANDLE HandleOfFile_ = INVALID_HANDLE_VALUE;
    HANDLE HandleOfMapping_ = INVALID_HANDLE_VALUE;
    const size_t maxbytes_eachmap_ = 1024 * 1024 * 10;   // 10M
};

FileMapping::FileMapping()
{
    pID_ = new MappingID;
    filepath_ = "";
    filesize_ = 0;
}


FileMapping::~FileMapping()
{
    if (pID_ != NULL)
        delete pID_;
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
    return pID_->OpenMapping(filepath_, filesize_);
}

int FileMapping::Mapping(MapInfo& info)
{
    info.len = 0;
    info.filepath = "";
    info.addr = NULL;

    int ret = pID_->DoMapping(&info.len);
    if (ret != SRH_OK)
        return ret;

    info.addr = pID_->GetAddress();
    info.filepath = filepath_;

    return SRH_OK;
}

int FileMapping::CloseMapping()
{
    delete pID_;
    pID_ = NULL;
    return SRH_OK;
}

