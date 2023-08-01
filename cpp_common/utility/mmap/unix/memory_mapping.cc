
#include "memory_mapping.h"

#include <string.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <memory>

namespace filemap
{
    PhyFileMap::PhyFileMap() : file_size_(0), filepath_(0) {
        fhinfo_.hf = -1;
        fhinfo_.hm = 0;
        fhinfo_.is_owner = false;
    }
    PhyFileMap::~PhyFileMap() {
        Close();
    }

    bool PhyFileMap::InitFromFile(const char* path, Permission pms)
    {
        if (access(path, F_OK)) return false;
        
        int open_flag = 0;
        int map_access = 0;
        switch (pms)
        {
            case Permission::Read:
                open_flag = O_RDONLY;
                map_access |= PROT_READ;
                break;
            case Permission::Write|Permission::Read:
                open_flag = O_RDWR;
                map_access = PROT_READ|PROT_WRITE;
                break;
            default:
                return false;
        }

        struct stat filestat;
        if (stat(path, &filestat) != 0)
        {
            return false;
        }

        int fd = 0;
        if (0 > (fd = open(path, open_flag)))
        {
            return false;
        }

        InitFromFile(fd, pms);

        std::unique_ptr<char[]> bf = std::make_unique<char[]>(strlen(path) + 1);
        strcpy(bf.get(), path);
        filepath_ = bf.release();

        file_size_ = filestat.st_size;
        fhinfo_.hf = fd;
        fhinfo_.hm = 0;
        fhinfo_.is_owner = true;
        return true;
    }
    bool PhyFileMap::InitFromFile(const wchar_t* path, Permission pms)
    {
        return false;
    }
    bool PhyFileMap::InitFromFile(FHandle filehandle, Permission pms)
    {
        permission_ = pms;
        fhinfo_.hf = filehandle;
        fhinfo_.is_owner = false;

        char buf[32];
        sprintf(buf, "/proc/self/%d/fd/%d", getpid(), (int)filehandle);

        std::unique_ptr<char[]> path = std::make_unique<char[]>(1024);
        int len = readlink(buf, path.get(), 1024);
        if (len > 0) {         
            path.get()[len] = 0;
            filepath_ = path.release();
        }
        return true;
    }
    
    size_t PhyFileMap::FileSize() const
    {
        return file_size_;
    }
    const char* PhyFileMap::FileName() const
    {
        return (const char*)filepath_;
    }
    Memory PhyFileMap::MapView(size_t offset, size_t size, Permission pms)
    {
        int map_access = 0;

        if ((pms | permission_) != permission_)
            return {0, 0};

        switch (pms)
        {
            case Permission::Read|Permission::Write:
                map_access = PROT_WRITE|PROT_READ;
                break;
            case Permission::Read:
                map_access = PROT_READ;
                break;
            case Permission::Write:
                map_access = PROT_WRITE;
                break;
            default:
                return {0, 0};
        }

        size_t pagesize = getpagesize();
        size_t start = offset - offset % pagesize;
        size_t bias = offset - start;

        void* addr = mmap(0, size + bias, map_access, MAP_SHARED, fhinfo_.hf, start);
        if (!addr)
            return {0, 0};
        
        if (mvm_.ptr) {
            munmap(mvm_.ptr, mvm_.sz);
        }
        mvm_.ptr = addr;
        mvm_.sz = size + bias;

        return {(char*)addr + bias, size};
    }
    
    void PhyFileMap::Close()
    {
        if (mvm_.ptr) {
            munmap(mvm_.ptr, mvm_.sz);
            mvm_.ptr = nullptr;
            mvm_.sz = 0;
        }
        if (fhinfo_.is_owner && fhinfo_.hf != -1) {
            close(fhinfo_.hf);
            fhinfo_.hf = -1;
        }
        
        if (filepath_) {
            delete[] (char*)filepath_;
            filepath_ = nullptr;
        }
        file_size_ = 0;
    }

}

