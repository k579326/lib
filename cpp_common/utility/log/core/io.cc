
#include <core/io.h>

#include "path/util_path.h"
#include "dir/util_dir.h"
#include "file/util_file.h"
#include "encode/platform_codecvt.h"
#include "mmap/memory_mapping.h"

using namespace fileutil;

#define _LOG_TIP_STRING     TEXT("\ntip: don't change anything in this file by manual.")
#define _LOG_INFO_VER       '\1\0\0\0'
#define _LOG_INFO_MGC       0x12345678
namespace
{
    class _F_Tail
    {
    public:
        _F_Tail() : magic(_LOG_INFO_MGC) {
            CptStringCopy(line, _LOG_TIP_STRING);
            version = _LOG_INFO_VER;
            size = 0;
        }
        CptChar line[64];
        int  version;
        const int  magic;
        uint64_t size;
    };
}


UniversalFactory::UniversalFactory()
{
    fm_.insert(std::make_pair(LogOutput::kFileModel, std::make_shared<FileIOFactory>()));
    fm_.insert(std::make_pair(LogOutput::kConsoleModel, std::make_shared<ConsoleIOFactory>()));
}

UniversalFactory::~UniversalFactory()
{

}

UniversalFactory* UniversalFactory::GetInstance()
{
    static UniversalFactory inst;
    return &inst;
}


std::shared_ptr<IOInterface> UniversalFactory::CreateIO(LogOutput type)
{
    auto it = fm_.find(type);
    if (it == fm_.end()) {
        return nullptr;
    }

    return it->second->CreateIO();
}



bool FileIO::Prepare(const CptString& path) const
{
    int result = 0;

    CptString dir = pathutil::GetDirOfPathName(path);
    if (CommCreateDir(dir.c_str())) {
        return false;
    }

    // exist?
    if (!fileutil::File::IsExist(path)) {
        File futil(path.c_str());
        result = futil.Open(kCreate, kRdWr);
    }
    else {
        uint64_t fsz = fileutil::File::Size(path);
        if (fsz != 0 && fsz != file_max_size_)
        {
            File::Remove(path);
            File futil(path.c_str());
            result = futil.Open(kCreate, kRdWr);
        }
    }

    if (result != 0) {
        return false;
    }

    return File::Truncate(path, file_max_size_) == 0;
}

uint64_t FileIO::GetRealSize(const void* ptr)
{
    char* local_ptr = (char*)ptr + file_max_size_ - file_info_size_;
    _F_Tail* ft = (_F_Tail*)local_ptr;
    if (ft->magic != _LOG_INFO_MGC) {
        return -1;
    }
    
    if (ft->version != _LOG_INFO_VER) {
        return -1;
    }

    return ft->size;
}

void FileIO::RecordRealSize(uint64_t inc)
{
    char* local_ptr = (char*)pointer_ + file_max_size_ - file_info_size_;
    _F_Tail* ft = (_F_Tail*)local_ptr;
    offset_ += inc;
    ft->size = offset_;
}

void FileIO::InitFile()
{
    _F_Tail ft;
    memset(pointer_, 0x20, file_max_size_);
#if defined _UNICODE || defined UNICODE
    for (int i = 0; i < file_max_size_; i += 2)
        *(reinterpret_cast<char*>(pointer_) + i + 1) = 0;
    *reinterpret_cast<int*>(pointer_) = 0xfeff;
    offset_ = 2;
#else
    offset_ = 0;
#endif
    memcpy((char*)pointer_ + file_max_size_ - file_info_size_, &ft, sizeof(_F_Tail));
}


int FileIO::Open(const CptString& path)
{
    if (!Prepare(path))
        return -1;

    auto map = std::make_shared<filemap::PhyFileMap>();
    if (!map) {
        return -1;
    }

    bool result = map->InitFromFile(path.c_str(), (filemap::Permission)(filemap::Permission::Read | filemap::Permission::Write));
    if (!result) {
        return -1;
    }

    auto memory = map->MapView(0, file_max_size_, (filemap::Permission)(filemap::Permission::Read | filemap::Permission::Write));
    if (memory.ptr == 0) {
        return -1;
    }

    if (file_max_size_ != map->FileSize())
    {
        map.reset();
        fileutil::File::Remove(path);
        return -1;
    }

    map_ = map;
    pointer_ = memory.ptr;

    offset_ = GetRealSize(memory.ptr);
    if (offset_ == -1)
    {
        InitFile();
    }
    return 0;
}

int FileIO::Write(const CptString& content)
{
    if (!pointer_) {
        return -1;
    }

    uint64_t wr_sz = content.size() * sizeof(CptString::value_type);
    if (wr_sz + offset_ > (uint64_t)file_max_size_ - file_info_size_)
    {
        return -1;
    }

    memcpy((unsigned char*)pointer_ + offset_, content.c_str(), wr_sz);
    RecordRealSize(wr_sz);
    return 0;
}

void FileIO::Close()
{
    if (map_) {
        map_->Close();
        map_.reset();
    }
    offset_ = 0;
    pointer_ = 0;
}


/* console io */
ConsoleIO::~ConsoleIO()
{
}

int ConsoleIO::Open(const CptString& path)
{
    console_ = GetConsole();
    return (int)console_ == -1 ? -1 : 0;
}

int ConsoleIO::Write(const CptString& content)
{
    int ret;
    if ((int)console_ == -1)
        return -1;

    SetConsoleTextColor(console_, _GetColor(), true);
#if defined _UNICODE || defined UNICODE
    std::string localstring = U16StringToAnsiString(content);
    ret = WriteConsole_(console_, GetLevel(), localstring.c_str(), localstring.size());
#else
    ret = WriteConsole_(console_, GetLevel(), content.c_str(), content.size());
#endif
    RestoreConsole(console_);
    return ret;
}

void ConsoleIO::Close()
{
    console_ = (ConsoleID)-1;
    return;
}










