
#pragma once

#include <cstddef>

namespace filemap
{
#if defined __linux__ || defined __ANDROID__
    typedef int FHandle;
#elif defined WIN32 || defined _WIN32
    typedef void* FHandle;
#else
    #error only support windows, android, linux
#endif
    
    struct Memory {
        void*   ptr;
        size_t  sz;
    };
    
    enum Permission : int
    {
        Read = 0x01,
        Write = 0x02,
    };

    class PhyFileMap
    {
    public:
        PhyFileMap();
        ~PhyFileMap();
        
        bool InitFromFile(const char* path, Permission);
        bool InitFromFile(const wchar_t* path, Permission);        
        bool InitFromFile(FHandle filehandle, Permission);
        
        size_t FileSize() const;
#ifdef WIN32
        const wchar_t* FileName() const;
#else
        const char* FileName() const;
#endif        
        Memory MapView(size_t offset, size_t size, Permission pms);
        
        void Close();
        
    private:
        struct _FHandleInfo {
            FHandle hf = 0;
            FHandle hm = 0;
            bool is_owner = false;
        };
    
        void*               filepath_;
        unsigned long long  file_size_;
        _FHandleInfo        fhinfo_;
        Permission          permission_;
        Memory              mvm_;
    };
    
};
