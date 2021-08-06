

#include "util_file.h"

#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <memory>


#include "u_path/util_path.h"

namespace fileutil
{

    class File::FilePtr
    {
    public:
        int fd_ = -1;
    };

    File::File() : file_(nullptr)
    {
        file_ = new FilePtr;
    }


    File::File(const char* path) : path_(path), file_(nullptr)
    {
        char dir[256];
        size_t length = 256;

        if (path[0] == '.')
        {
            if (path[1] != '/' && path[1] != '\\') {
                assert(false);
                path_ = "";
            }
            else if (getcwd(dir, length)) 
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

        std::string dir = pathutil::GetDirOfPathName(path_);
        if (access(dir.c_str(), F_OK) != 0) {
            return -1;
        }
        
        int exist = access(path_.c_str(), F_OK);
        if (om == OpenModel::kCreate && !exist)
            return -1;
        if (om == OpenModel::kOpenExist && exist)
            return -1;

        int flag = 0;
        switch (om)
        {
            case OpenModel::kCreate:
                flag |= O_CREAT;
                break;
            case OpenModel::kCreateForce:
                flag |= (O_CREAT | O_TRUNC);
                break;
            default:
                break; 
        }
        switch (am)
        {
            case AccessModel::kReadOnly:
                flag |= O_RDONLY;
                break;
            case AccessModel::kWriteOnly:
                flag |= O_WRONLY;
                break;
            case AccessModel::kRdWr:
                flag |= O_RDWR;
                break;
            default:
                break;
        }
            
        if (IsOpened()) {
            return -1;
        }
    
        int fd = open(path_.c_str(), flag, permission);
        if (fd < 0) {
            return -1;
        }
        file_->fd_ = fd;
        return 0;
    }

    int File::Seek(int64_t off, SeekPosition where)
    {
        if (!IsOpened()) {
            return -1;
        }

        int w = SEEK_SET;
        switch (where)
        {
            case kSeekSet:
                w = SEEK_SET;
                break;
            case kSeekCur:
                w = SEEK_CUR;
                break;
            case kSeekEnd:
                w = SEEK_END;
                break;
            default:
                break;
        }

        if (lseek(file_->fd_, off, w) == -1)
            return -1;
        
        return 0;
    }

    void File::Close()
    {
        if (IsOpened()) {
            fsync(file_->fd_);
            close(file_->fd_);
            file_->fd_ = -1;
        }
        return;
    }
    int File::Read(std::string* out, uint32_t expected_size)
    {
        int ret = 0;
        ssize_t readsize = 0;
        out->clear();

        if (!IsOpened()) {
            return -1;
        }

        std::shared_ptr<char> str_array(new char[expected_size], [](char* ptr) { delete[] ptr; });

        readsize = read(file_->fd_, str_array.get(), expected_size);
        if (readsize == -1) {
            return -1;
        }
        if (readsize == 0) {
            return 0;
        }

        //assert(readsize == expected_size);
        out->assign(str_array.get(), readsize);
        return 0;
    }

    int File::Write(const void* indata, uint32_t insize)
    {
        ssize_t writesize = 0;

        if (!IsOpened()) {
            return -1;
        }

        writesize = write(file_->fd_, indata, insize);
        if (writesize == -1)
        {
            return -1;
        }

        return 0;
    }

    int File::ReadLine(std::string* out)
    {
        if (!IsOpened()) {
            return -1;
        }

        out->clear();

        ssize_t readsize = 0;
        std::shared_ptr<char> strptr(new char[256], [](char* p) { delete[] p; });

        while (true)
        {
            readsize = read(file_->fd_, strptr.get(), 256);
            if (0 >= readsize) {
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
            fsync(file_->fd_);
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
        return file_ != nullptr && file_->fd_ != -1;
    }
    

    // static 
    uint64_t File::Size(const std::string& filepath)
    {
        if (filepath.empty()) {
            return 0;
        }

        struct stat filestat;
        if (stat(filepath.c_str(), &filestat) != 0)
        {
            return 0;
        }
        return filestat.st_size;
    }

    // static 
    int File::Remove(const std::string& filepath)
    {
        return remove(filepath.c_str());
    }
    // static 
    int File::Copy(const std::string& srcpath, const std::string& dstpath, bool overlay)
    {
        if (!IsExist(srcpath)) {
            return -1;
        }

        if (!overlay && IsExist(dstpath)) {
            return -1;
        }

        int rd = open(srcpath.c_str(), O_RDONLY);
        int wd = open(dstpath.c_str(), O_CREAT | O_WRONLY);
        if (rd < 0 || wd < 0)
        {
            return -1;
        }

        const int roundsize = 1024 * 1024 * 4;
        std::shared_ptr<char> buf_ptr(new char[roundsize], [](char* p) { delete[] p; });

        while (true) {
            int rs = read(rd, buf_ptr.get(), roundsize);
            if (rs > 0) 
            {
                if (write(wd, buf_ptr.get(), rs) < rs) {
                    // exception occur
                    close(wd);
                    remove(dstpath.c_str());
                    return -1;
                }
            }
            else if (rs == 0) {
                break;
            } 
            else {
                // error
                close(wd);
                remove(dstpath.c_str());
                return -1;
            }
        }

        if (wd > 0)
            close(wd);
        if (rd > 0)
            close(rd);

        return 0;
    }
    // static 
    int File::Truncate(const std::string& filename, uint64_t length)
    {
        if (!File::IsExist(filename)) {
            return -1;
        }

        return truncate(filename.c_str(), length);
    }
    // static 
    bool File::IsNormalFile(const std::string& filename)
    {
        struct stat filestat;
        if (stat(filename.c_str(), &filestat) != 0)
        {
            return false;
        }
        if (S_ISREG(filestat.st_mode))
        {
            return true;
        }
        return false;
    }
    // static 
    bool File::IsExist(const std::string& path)
    {
        return access(path.c_str(), F_OK) == 0;
    }
    // static 
    int File::Rename(const std::string& oldname, const std::string& newname)
    {
        return rename(oldname.c_str(), newname.c_str());
    }

};