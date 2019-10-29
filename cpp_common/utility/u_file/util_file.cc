

#include "util_file.h"


#ifndef WIN32
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#else
#include <windows.h>
#include <io.h>

#ifndef F_OK
#define F_OK 0
#endif

#endif


#include <stdio.h>
#include <memory>
#include <algorithm>

/*
#if !defined(S_ISFIFO)
#   define S_ISFIFO(mode) (((mode) & S_IFMT) == S_IFIFO)
#endif
*/

#if !defined(S_ISDIR)
#   define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif
#if !defined(S_ISREG)
#   define S_ISREG(mode) (((mode) & S_IFMT) == S_IFREG)
#endif

/*
#if !defined(S_ISLNK)
#   define S_ISLNK(mode) (((mode) & S_IFMT) == S_IFLNK)
#endif
#if !defined(S_ISSOCK)
#   define S_ISSOCK(mode) (((mode) & S_IFMT) == S_IFSOCK)
#endif
*/

#if !defined(S_ISCHR)
#   define S_ISCHR(mode) (((mode) & S_IFMT) == S_IFCHR)
#endif

/*
#if !defined(S_ISBLK)
#   define S_ISBLK(mode) (((mode) & S_IFMT) == S_IFBLK)
#endif
*/

namespace fileutil
{
    static std::string _ParentDirOfPath(const char* path)
    {
        std::string ret_str = path;

        auto it = std::find_if(ret_str.rbegin(), ret_str.rend(), [](char c)->bool { return c == '\\' || c == '/'; });
        if (it == ret_str.rend())
        {
            return "";
        }

        return std::string(ret_str.begin(), it.base());
    }


    File::File() : file_(nullptr)
    {
    }
    File::~File()
    {
        Close();
    }

    File::File(const char* path) : path_(path), file_(nullptr)
    {
    }


    int File::Open(const std::string& mode)
    {
        std::string dir = _ParentDirOfPath(path_.c_str());

        /* 先判断文件的目录是否存在， 不存在则不允许直接创建目录*/
        if (access(dir.c_str(), F_OK)) {
            return -1;
        }

        if (path_.empty()) {
            return -1;
        }

        if (IsOpened()) {
            Close();
        }

        file_ = fopen(path_.c_str(), mode.c_str());
        if (!file_) {
            return errno;
        }

        return 0;
    }

    int File::Seek(int32_t off, int where)
    {
        if (!file_) { 
            return -1;
        }

        return fseek(file_, off, where);
    }

    int File::Close()
    {
        if (file_) {
            fclose(file_);
            file_ = nullptr;
        }

        return 0;
    }

    int File::Read(std::string* out, uint32_t expected_size)
    {
        int ret = 0;
        uint32_t readsize = 0;

        if (!IsOpened()) {
            return -1;
        }

        std::shared_ptr<char> str_array(new char[expected_size], [](char* ptr) { delete[] ptr; });

        readsize = fread(str_array.get(), 1, expected_size, file_);
        if (expected_size == readsize)
        {
            out->assign(str_array.get(), readsize);
            ret = 0;
        }
        else if (readsize > 0) /* readsize > 0 && readsize != expected_size */
        {
            if (IsEOF())
            {
                out->assign(str_array.get(), readsize);
                ret = 0;
            }
            else {  /* 读取发生异常，读的数据长度不等于期望长度，也没有到达文件尾 */
                ret = ferror(file_);
            }
        }
        else {
            ret = ferror(file_);
        }

        return ret;
    }

    int File::ReadLine(std::string* out)
    {
        if (!file_) {
            return -1;
        }

        while (true)
        {
            std::shared_ptr<char> strptr(new char[256], [](char* p) { delete[] p; });

            if (!fgets(strptr.get(), 256, file_)) {

                return ferror(file_);
            }

            *out += strptr.get();

            if (*(out->end() - 1) == '\n')
            {
                break;
            }
        }
        

        return 0;
    }

    int File::Write(const unsigned char* indata, uint32_t insize)
    {
        int ret = 0;
        uint32_t writesize = 0;

        if (!file_) {
            return -1;
        }

        writesize = fwrite(indata, 1, insize, file_);
        if (insize != writesize) {
            ret = ferror(file_);
        }
        else {
            ret = 0;
        }

        return ret;
    }

    bool File::IsEOF()
    {
        if (!file_) {
            return false;
        }

        return feof(file_) == 0 ? false : true;
    }


    void File::Reset(const char* path)
    {
        path_ = path;
        
        if (file_) {
            fclose(file_);
            file_ = nullptr;
        }

        return;
    }

    int File::Flush()
    {
        if (file_)
            fflush(file_);
        return 0;
    }

    uint32_t File::Size(const std::string& filepath)
    {
        struct stat filestat;

        if (filepath.empty()) {
            return 0;
        }

        if (stat(filepath.c_str(), &filestat) != 0)
        {
            return 0;
        }

        return filestat.st_size;
    }

    std::string File::Name() const
    {
        return path_;
    }

    bool File::IsOpened(void) const
    {
        return file_ != nullptr && !path_.empty();
    }

    /* ============= static function =============== */
    int File::Remove(const std::string& filepath)
    {
        return remove(filepath.c_str()) == 0 ? 0 : errno;
    }

    int File::Copy(const std::string& srcpath, const std::string& dstpath, bool overlay)
    {
        if (!IsExist(srcpath)) {
            return -1;
        }

        if (!overlay && IsExist(dstpath)) {
            return -1;
        }

        auto fp_free = [](FILE* fp) { if (fp) fclose(fp); };
        std::shared_ptr<FILE> srcpath_ptr(fopen(srcpath.c_str(), "rb"), fp_free);
        std::shared_ptr<FILE> dstpath_ptr(fopen(dstpath.c_str(), "wb"), fp_free);

        if (!srcpath_ptr || !dstpath_ptr)
        {
            return -1;
        }


        const int roundsize = 1024 * 1024;
        std::shared_ptr<char> buf_ptr(new char[roundsize], [](char* p) { delete[] p; });

        while (true) {

            int rs = fread(buf_ptr.get(), 1, roundsize, srcpath_ptr.get());
            
            if (rs > 0)
                if (fwrite(buf_ptr.get(), 1, rs, dstpath_ptr.get()) < rs) {
                    // exception occur
                    dstpath_ptr.reset();
                    remove(dstpath.c_str());
                    return -1;
                }

            if (feof(srcpath_ptr.get())) {
                // success
                break;
            }
        }

        return 0;
    }
    int File::Rename(const std::string& oldname, const std::string& newname)
    {
        return rename(oldname.c_str(), newname.c_str()) == 0 ? 0 : errno;
    }

    bool File::Truncate(const std::string& filename, uint32_t length)
    {
        if (!File::IsExist(filename)) {
            return false;
        }

#ifdef WIN32
        HANDLE filehandle = CreateFileA(filename.c_str(), 
            GENERIC_WRITE | GENERIC_READ, 
            0,
            NULL, 
            OPEN_EXISTING, 
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (filehandle == INVALID_HANDLE_VALUE) {
            return false;
        }

        LARGE_INTEGER li;
        li.QuadPart = length;

        if (SetFilePointerEx(filehandle, li, NULL, FILE_BEGIN) == FALSE) {
            CloseHandle(filehandle);
            return false;
        }

        if (SetEndOfFile(filehandle) == FALSE) {
            CloseHandle(filehandle);
            return false;
        }

        CloseHandle(filehandle);
        return true;
#else
        return truncate(filename.c_str(), length) == 0;
#endif
    }

    bool File::IsExist(const std::string& path)
    {
        int ret = 0;
        struct stat filestat;

        if (path.empty()) {
            return false;
        }

        ret = stat(path.c_str(), &filestat);
        if (ret != 0)
        {
            return false;
        }

        return S_ISREG(filestat.st_mode);
    }
};


