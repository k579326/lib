
#include "util_file.h"

#include <windows.h>
#include <shlwapi.h>
#include <direct.h>

#include <assert.h>

#include <map>
#include <memory>
#include <atomic>

#include "u_path/util_path.h"

/*

1、对于fopen追加模式的测试结果：
    write总是写在文件末尾，即使使用fseek也无法改变写入位置
    read执行完后，再write，发生崩溃
    wirte之后read，读取内容异常，即使使用fseek，fflush等方法，无法读取目标位置的内容
2、对fopen的rb+模式进行测试：
    read与write混合执行，不使用fflush，结果依然混乱，最后文件的部分内容也出现随机值
    如果在写入操作完成后执行fflush，测试流程结果正常。
3、对Windows API进行测试：
    OPEN_ALWAYS模式，读取与写入完全按照文件指针位置返回结果

*/


namespace fileutil {
    // ...
    // OPEN_ALWAYS: open or create file, and file pointer is 0 offset
    // OPEN_EXISTING: open exist file, and file pointer is 0 offset
    // CREATE_ALWAYS: if file exist, set the file to 0 size
    const static std::map<OpenModel, DWORD> cs_openmodel_map = 
    {   
        { kCreate, CREATE_NEW },
        { kCreateForce, CREATE_ALWAYS},
        { kOpen, OPEN_EXISTING },
        { kOpenForce, OPEN_ALWAYS },
        { kAppend, OPEN_EXISTING },
        { kAppendForce, OPEN_ALWAYS },
    };
    const static std::map<AccessModel, DWORD> cs_accessmodel_map = 
    {   { kReadOnly, GENERIC_READ },
        { kWriteOnly, GENERIC_WRITE },
        { kRdWr, GENERIC_READ | GENERIC_WRITE } 
    };
    const static std::map<SeekPosition, DWORD> cs_seekpos_map = 
    { 
        {kSeekSet, FILE_BEGIN}, 
        {kSeekCur, FILE_CURRENT}, 
        {kSeekEnd, FILE_END} 
    };


    class File::FilePtr
    {
    public:
        HANDLE hFile_ = INVALID_HANDLE_VALUE;
        DWORD creation_model;
        DWORD access_model;
    };

    File::File() : file_(nullptr)
    {
        file_ = new FilePtr;
    }
    File::File(const char* path) : path_(path), file_(nullptr)
    {
        char dir[256];
        size_t len = 256;

        if (path[0] == '.') {
            if (path[1] != '/' && path[1] != '\\') {
                assert(false);
                path_ = "";
            }
            else if (_getcwd(dir, (int)len)) 
            {
                path_ = dir + path_.substr(1);
            }
        }
        pathutil::PathStyleConvert(path_);
        file_ = new FilePtr;
    }
    File::~File()
    {
        Close();
        delete file_;
    }

    int File::Open(OpenModel om, AccessModel am, int permission)
    {
        if (path_.empty()) {
            return -1;
        }
        
        // 强制创建必须在WrrteOnly 或者 RdWr 模式下
        if (kCreateForce == om && kReadOnly == am)
            return -1;

        std::string dir = pathutil::GetDirOfPathName(path_);
        if (!PathFileExistsA(dir.c_str())) {
            return -1;
        }

        BOOL exist = PathFileExistsA(path_.c_str());
        if (om == kCreate && exist)
            return -1;
        if (om == kOpen && !exist)
            return -1;
        if (om == kAppend && !exist)
            return -1;

        if (IsOpened()) {
            return -1;
        }
        
        file_->hFile_ = CreateFileA(path_.c_str(),
                                    cs_accessmodel_map.find(am)->second,
                                    FILE_SHARE_READ,  // only share read
                                    NULL,
                                    cs_openmodel_map.find(om)->second,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL);

        if (file_->hFile_ == INVALID_HANDLE_VALUE)
        {
            return -1;
        }

        file_->access_model = am;
        file_->creation_model = om;

        return 0;
    }


    int File::Seek(int64_t off, SeekPosition where) const
    {
        int32_t hword, lword;
        int32_t* phword;

        hword = off >> 32;
        lword = off & (0xFFFFFFFFll);

        if (!hword)
            phword = &hword;
        else
            phword = NULL;

        DWORD ret = SetFilePointer(file_->hFile_,
                                    lword, 
                                    (PLONG)phword, 
                                    cs_seekpos_map.find(where)->second);
        if (ret == INVALID_SET_FILE_POINTER) {
            if (GetLastError() != 0)
                return -1;
        }

        /*if (!phword && ret != lword)
            return -1;
        
        if (phword && ((int64_t)(*phword) << 32) + lword != off)
            return -1;
            */
        return 0;
    }

    void File::Close()
    {
        if (file_ && file_->hFile_ != INVALID_HANDLE_VALUE) {
            Flush();
            CloseHandle(file_->hFile_);
            file_->hFile_ = INVALID_HANDLE_VALUE;
        }
        return;
    }

    int File::Read(std::string* out, uint32_t expected_size) const
    {
        int ret = 0;
        DWORD readsize = 0;
        BOOL success;

        out->clear();
        if (!IsOpened()) {
            return -1;
        }

        std::shared_ptr<char> str_array(new char[expected_size], [](char* ptr) { delete[] ptr; });

        success = ReadFile(file_->hFile_, str_array.get(), expected_size, &readsize, NULL);
        if (!success)
        {
            return -1;
        }

        //assert(readsize == expected_size);
        out->assign(str_array.get(), readsize);
        return 0;
    }


    int File::Write(const void* indata, uint32_t insize) const
    {
        DWORD writesize = 0;
        BOOL success = FALSE;

        if (!IsOpened()) {
            return -1;
        }

        /*
            windows API do not support append model,
            we simulate it by moving file pointer.
        */
        if (file_->creation_model == OpenModel::kAppend ||
            file_->creation_model == OpenModel::kAppendForce)
        {
            if (SetFilePointer(file_->hFile_, 0, NULL, FILE_END) 
                == INVALID_SET_FILE_POINTER) {
                if (GetLastError() != 0)
                    return -1;
            }
        }

        success = WriteFile(file_->hFile_, indata, insize, &writesize, NULL);
        if (!success)
        {
            return -1;
        }

        return 0;
    }

    int File::ReadLine(std::string* out) const
    {
        if (!IsOpened()) {
            return -1;
        }

        out->clear();

        DWORD readsize = 0;
        std::shared_ptr<char> strptr(new char[256], [](char* p) { delete[] p; });

        while (true)
        {
            if (!ReadFile(file_->hFile_, strptr.get(), 256, &readsize, NULL)) {
                break;
            }

            char* pos = strptr.get();
            while (*pos != '\n' && pos < strptr.get() + readsize) {
                pos++;
            }

            if (pos < strptr.get() + readsize) {
                out->append(strptr.get(), pos);
                break;
            }
            else {
                out->append(strptr.get(), readsize);
            }
        }

        return out->size();
    }

    std::string File::Name() const
    {
        return path_;
    }

    void File::Flush()
    {
        if (IsOpened())
            FlushFileBuffers(file_->hFile_);
        return;
    }


    void File::Reset(const char* path)
    {
        path_ = path;

        if (IsOpened()) {
            Close();
        }

        pathutil::PathStyleConvert(path_);
        return;
    }

    bool File::IsOpened(void) const
    {
        return file_ != nullptr && file_->hFile_ != INVALID_HANDLE_VALUE;
    }
    

    // static 
    uint64_t File::Size(const std::string& filepath)
    {
        uint64_t size;
        if (filepath.empty()) {
            return 0;
        }

        WIN32_FILE_ATTRIBUTE_DATA wfa;
        if (!GetFileAttributesExA(filepath.c_str(), GetFileExInfoStandard, &wfa))
        {
            return 0;
        }
        size = ((uint64_t)wfa.nFileSizeHigh << 32) + wfa.nFileSizeLow;
        return size;
    }

    // static 
    int File::Remove(const std::string& filepath)
    {
        return DeleteFileA(filepath.c_str()) == TRUE ? 0 : -1;
    }
    // static 
    int File::Copy(const std::string& srcpath, const std::string& dstpath, bool overlay)
    {
        return CopyFileA(srcpath.c_str(), dstpath.c_str(), !overlay) == TRUE? 0:-1;
    }
    // static 
    int File::Truncate(const std::string& filename, uint64_t length)
    {
        if (!File::IsExist(filename)) {
            return -1;
        }

        HANDLE filehandle = CreateFileA(filename.c_str(),
            GENERIC_WRITE | GENERIC_READ,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (filehandle == INVALID_HANDLE_VALUE) {
            return -1;
        }

        LARGE_INTEGER li;
        li.u.HighPart = length >> 32;
        li.u.LowPart = length & 0x0ffffffffllu;

        if (SetFilePointerEx(filehandle, li, NULL, FILE_BEGIN) == FALSE) {
            CloseHandle(filehandle);
            return -1;
        }

        if (SetEndOfFile(filehandle) == FALSE) {
            CloseHandle(filehandle);
            return -1;
        }

        CloseHandle(filehandle);
        return 0;
    }
    // static 
    bool File::IsNormalFile(const std::string& filename)
    {
        DWORD attr = GetFileAttributesA(filename.c_str());
        return attr & FILE_ATTRIBUTE_ARCHIVE;
    }
    // static 
    bool File::IsExist(const std::string& path)
    {
        return PathFileExistsA(path.c_str());
    }
    // static 
    int File::Rename(const std::string& oldname, const std::string& newname)
    {
        return MoveFileA(oldname.c_str(), newname.c_str()) ? 0 : -1;
    }
};


