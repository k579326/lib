

#include "util_file.h"

#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <memory>

/*
对open函数进行测试的汇总：
    常规打开模式：文件打开后指针初始偏移为0，与WindowsAPI一致

    1、O_CREAT 如果没有该flag，open函数仅可以打开已存在的文件，绝不会创建文件
        如果文件已存在，该flag仅与O_EXCL组合对open有特殊控制。

    2、O_APPEND 追加模式，持有该flag而没有O_CREAT，open函数仅可以打开已存在的文件
        它对O_CREAT的规则没有影响。此模式下read与write操作无fopen的混乱情况；
        write总是写在文件末尾，lseek对追加模式的write其无效，read结果依循文件指针位置读取。
        不受读写权限影响

    3、O_EXCL 仅对O_CREAT有效，如果O_EXCL与O_CREAT同时存在，打开已存在的文件将会失败
        文件不存在或者没有O_CREAT，该标志无效

    4、O_TRUNC 截断模式，打开的文件将被清空，可以与O_CREAT同时存在（可能没什么意义）。
        该模式需要与O_WRONLY或者O_RDWR配合（即只有写权限才可以截断打开文件）
        文件存在与否，该标志不影响open成功失败结果。
*/



#include "path/util_path.h"

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


    File::File(const CptChar* path) : path_(path), file_(nullptr)
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

        // 强制创建必须在WrrteOnly 或者 RdWr 模式下
        if (kCreateForce == om && kReadOnly == am)
            return -1;

        std::string dir = pathutil::GetDirOfPathName(path_);
        if (access(dir.c_str(), F_OK) != 0) {
            return -1;
        }
        
        int exist = access(path_.c_str(), F_OK);
        if (om == OpenModel::kCreate && !exist)
            return -1;
        if (om == OpenModel::kOpen && exist)
            return -1;
        if (om == OpenModel::kAppend && exist)
            return -1;

        int flag = 0;
        switch (om)
        {
            case OpenModel::kCreate:
                flag |= (O_CREAT|O_EXCL);
                break;
            case OpenModel::kCreateForce:
                flag |= (O_CREAT|O_TRUNC);
                break;
            case OpenModel::kOpenForce:
                flag |= O_CREAT;
                break;
            case OpenModel::kAppend:
                flag |= O_APPEND;
                break;
            case OpenModel::kAppendForce:
                flag |= (O_CREAT|O_APPEND);
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

    int File::Seek(int64_t off, SeekPosition where) const
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
            // fsync(file_->fd_);
            close(file_->fd_);
            file_->fd_ = -1;
        }
        return;
    }
    int File::Read(std::string* out, uint32_t expected_size) const
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

    int File::Write(const void* indata, uint32_t insize) const
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

    int File::ReadLine(std::string* out) const
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

    bool File::GetFileDescriptor(FileDescriptor* fd) const
    {
        if (!IsOpened())
            return false;
            
        fd->fd = file_->fd_;
        return true;
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

        int rd = open(srcpath.c_str(), O_RDONLY, 0666);
        int wd = open(dstpath.c_str(), O_CREAT|O_TRUNC|O_WRONLY, 0666);
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