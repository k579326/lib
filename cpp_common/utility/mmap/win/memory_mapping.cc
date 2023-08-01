#include "memory_mapping.h"

#include <windows.h>
#include <shlwapi.h>
#include <string.h>

#include <memory>

namespace filemap
{

PhyFileMap::PhyFileMap() : file_size_(0), filepath_(0), permission_(Permission::Read)
{
    fhinfo_.hf = INVALID_HANDLE_VALUE;
    fhinfo_.hm = 0;
    fhinfo_.is_owner = false;

    mvm_.ptr = 0;
    mvm_.sz = 0;
}
PhyFileMap::~PhyFileMap() 
{
    Close();
}

bool PhyFileMap::InitFromFile(const char* path, Permission pms)
{
    int result = MultiByteToWideChar(CP_ACP, 0, path, strlen(path), NULL, 0);
    wchar_t* buf = (wchar_t*)malloc((result + 1) * sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, 0, path, strlen(path), buf, result);
    buf[result] = 0;

    bool success = InitFromFile(buf, pms);
    free(buf);
    return success;
}

bool PhyFileMap::InitFromFile(const wchar_t* path, Permission pms)
{
    if (!PathFileExistsW(path)) return false;
    
    DWORD open_permission = 0;

    if (pms | Permission::Read)
    {
        open_permission |= GENERIC_READ;
    }
    if (pms | Permission::Write)
    {
        open_permission |= GENERIC_WRITE;
    }

    HANDLE hfile = CreateFileW(path, open_permission, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hfile == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    if (!InitFromFile(hfile, pms)) {
        CloseHandle(hfile);
        return false;
    }

    if (!filepath_)
    {
        filepath_ = new wchar_t[wcslen(path) + 1];
        wcscpy((wchar_t*)filepath_, path);
    }
    fhinfo_.is_owner = true;

    return true;
}

bool PhyFileMap::InitFromFile(FHandle filehandle, Permission pms)
{
    LARGE_INTEGER li = {};
    if (!GetFileSizeEx(filehandle, &li)) {
        return false;
    }

    DWORD map_permission = 0;

    switch (pms)
    {
    case Permission::Read|Permission::Write:
        map_permission = PAGE_READWRITE;
        break;
    case Permission::Read:
        map_permission = PAGE_READONLY;
        break;
    default:
        return false;
    }

    HANDLE hmap = CreateFileMappingW(filehandle, NULL, map_permission, li.HighPart, li.LowPart, NULL);
    if (!hmap) {
        return false;
    }

    fhinfo_.hf = filehandle;
    fhinfo_.hm = hmap;
    fhinfo_.is_owner = false;

    file_size_ = li.QuadPart;
    permission_ = pms;

    // get filepath
    std::unique_ptr<wchar_t[]> pathbuf = std::make_unique<wchar_t[]>(1024);

    HMODULE h = LoadLibraryA("kernal32.dll");
    FARPROC f = GetProcAddress(h, "GetFinalPathNameByHandleW");

    DWORD size = 0;
    typedef decltype(&GetFinalPathNameByHandleW) __Local_FuncPtr; 
    if (f) 
    {
        __Local_FuncPtr __GetFinalPathNameByHandleW = (__Local_FuncPtr)f;
        size = __GetFinalPathNameByHandleW((HANDLE)filehandle, pathbuf.get(), 1024, 0);
    }
    if (size != 0) {
        filepath_ = pathbuf.release();
        ((wchar_t*)filepath_)[size] = 0;
    } else {
        filepath_ = nullptr;
    }
    return true;
}

size_t PhyFileMap::FileSize() const
{
    return file_size_;
}

const wchar_t* PhyFileMap::FileName() const
{
    return (const wchar_t*)filepath_;
}

Memory PhyFileMap::MapView(size_t offset, size_t size, Permission pms)
{
    DWORD access = 0;

    if ((pms | permission_) != permission_)
        return {0, 0};

    switch (pms)
    {
    case Permission::Read:
        access = FILE_MAP_READ;
        break;
    case Permission::Write:
        access = FILE_MAP_WRITE;
        break;
    case Permission::Read|Permission::Write:
        access = FILE_MAP_READ|FILE_MAP_WRITE;
        break;
    default:
        return {0, 0};
    }

    SYSTEM_INFO si = {};
    GetSystemInfo(&si);

    size_t start = offset - offset % si.dwAllocationGranularity;
    LARGE_INTEGER li = {};
    li.QuadPart = start;

    size_t bias = offset - start;

    void* p = MapViewOfFile(fhinfo_.hm, access, li.HighPart, li.LowPart, size + bias);
    if (!p)
        return {0, 0};

    if (mvm_.ptr)
        UnmapViewOfFile(mvm_.ptr);

    mvm_.ptr = p;
    mvm_.sz = size + bias;

    return {(char*)p + bias, size};
}

void PhyFileMap::Close()
{
    if (mvm_.ptr)
    {
        UnmapViewOfFile(mvm_.ptr);
        mvm_.ptr = 0;
        mvm_.sz = 0;
    }

    if (fhinfo_.hm)
    {
        CloseHandle(fhinfo_.hm);
    }
    if (fhinfo_.is_owner && fhinfo_.hf != INVALID_HANDLE_VALUE) 
    {
        CloseHandle(fhinfo_.hf);
    }
    memset(&fhinfo_, 0, sizeof(fhinfo_));
    if (filepath_) delete[] (wchar_t*)filepath_;
    filepath_ = nullptr;
    file_size_ = 0;
}

    
    
};

