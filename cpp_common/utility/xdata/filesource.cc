#include "filesource.h"

#include <assert.h>
#include <stdio.h>

#include "file/util_file.h"
#include "memory_mapping.h"
#include "dataencoder.h"


#define _XDATA_FILE_MAX_SIZE 1024 * 1024
#define _XDATA_FILE_HEADER_SIZE sizeof(DataEncoder::StreamHeader)


using namespace filemap;
using namespace fileutil;

FileSource::FileSource() 
{
    fmap_ = new filemap::PhyFileMap();
    cur_offset_ = 0;
    total_size_ = 0;
    base_ = nullptr;
    content_ = nullptr;
}

FileSource::~FileSource() { delete fmap_; }

bool FileSource::Init(const CptString& path)
{
    if (!fileutil::File::IsExist(path))
    {
        fileutil::File f(path.c_str());
        f.Open(OpenModel::kCreate, AccessModel::kRdWr);
        f.Close();
        fileutil::File::Truncate(path, _XDATA_FILE_MAX_SIZE);
    }

    file_path_ = path;
    total_size_ = File::Size(file_path_);
    return true;
}

bool FileSource::Open()
{
    if (!fmap_ || file_path_.empty()) return false;
    if (!fmap_->InitFromFile(file_path_.c_str(), (Permission)3)) return false;

    auto m = fmap_->MapView(0, total_size_, (Permission)3);
    if (!m.sz || !m.ptr) return false;

    assert(total_size_ == m.sz);
    base_ = m.ptr;
    content_ = ((char*)base_ + _XDATA_FILE_HEADER_SIZE);
    cur_offset_ = 0;

    return true;
}
bool FileSource::PrepareHeader()
{
    if (!base_) return false;

    memset(base_, 0, total_size_);

    DataEncoder::StreamHeader sh = {};
    sh.version = XDATA_STREMA_HEADER_VERSION;
    sh.length = 0;
    memcpy(base_, &sh, _XDATA_FILE_HEADER_SIZE);

    return true;
}

bool FileSource::UpdateHeader()
{
    DataEncoder::StreamHeader* header = (DataEncoder::StreamHeader*)base_;
    header->length = cur_offset_;
    return true;
}

bool FileSource::Writable(size_t size) const
{
    return cur_offset_ + size + _XDATA_FILE_HEADER_SIZE <= total_size_;
}
bool FileSource::Write(size_t pos, size_t offset, const void* data, size_t size)
{
    memcpy(content_ + pos + offset, data, size);

    size_t furture_pos = pos + offset + size;
    if (furture_pos > cur_offset_)
    {
        cur_offset_ += (furture_pos - cur_offset_);
        UpdateHeader();
    }
    return true;
}
bool FileSource::MoveData(size_t dst, size_t src, size_t bytes)
{
    memmove(content_ + dst, content_ + src, bytes);
    cur_offset_ += bytes;
    UpdateHeader();
    return true;
}
int32_t FileSource::ReadAsInt32(size_t pos, size_t bytes)
{
    return *(int32_t*)(content_ + pos);
}

size_t FileSource::ReadAll(void** data)
{
    DataEncoder::StreamHeader* h = (DataEncoder::StreamHeader*)base_;
    if (h->version != XDATA_STREMA_HEADER_VERSION) return 0;
    // if (h->length != total_size_ - _XDATA_FILE_HEADER_SIZE) return 0;

    total_size_ = h->length;

    *data = content_;
    return total_size_;
}

void FileSource::Close()
{
    fmap_->Close();
    base_ = nullptr;
    content_ = nullptr;
    cur_offset_ = 0;
}







