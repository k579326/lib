#ifndef _LOG_IO_H_
#define _LOG_IO_H_

#include <map>
#include <string>
#include <memory>

#include "logger_define.h"
#include "console/console_extern.h"
#include "cpt-string.h"

namespace filemap
{
    class PhyFileMap;
}

class IOInterface
{
public:    
    IOInterface() {}
    virtual ~IOInterface() {}
    
    virtual int Open(const CptString& path) = 0;
    
    virtual int Write(const CptString& content, __LogTextColor textcolor) = 0;
    
    virtual void Close() = 0;
    
};

class FileIO : public IOInterface
{
public:
    FileIO() {}
    ~FileIO() {}

    virtual int Open(const CptString& path) override;
    virtual int Write(const CptString& content, __LogTextColor textcolor) override;
    virtual void Close() override;
private:
    bool Prepare(const CptString& path) const;
    uint64_t GetRealSize(const void* ptr);
    void RecordRealSize(uint64_t inc);
    void InitFile();
private:
    std::shared_ptr<filemap::PhyFileMap> map_ = nullptr;
    void* pointer_ = 0;
    uint64_t offset_ = 0;
#if defined _UNICODE || defined UNICODE
    const int file_info_size_ = 256;
#else
    const int file_info_size_ = 128;
#endif
    const int file_max_size_ = file_info_size_ * 1024 + file_info_size_;
};

class ConsoleIO : public IOInterface
{
public:
    ConsoleIO();
    ~ConsoleIO();

    virtual int Open(const CptString& path) override;
    virtual int Write(const CptString& content, __LogTextColor textcolor) override;
    virtual void Close() override;

private:
    ConsoleID console_ = (ConsoleID)-1;
};



class IOFactory
{
public:
    IOFactory() {}
    ~IOFactory() {}

    virtual std::shared_ptr<IOInterface> CreateIO() = 0;
};


class ConsoleIOFactory : public IOFactory
{
public:
    ConsoleIOFactory() {}
    ~ConsoleIOFactory() {}

    virtual std::shared_ptr<IOInterface> CreateIO() {
        return std::make_shared<ConsoleIO>();
    }
};

class FileIOFactory : public IOFactory
{
public:
    FileIOFactory() {}
    ~FileIOFactory() {}

    virtual std::shared_ptr<IOInterface> CreateIO() {
        return std::make_shared<FileIO>();
    }
};


class UniversalFactory
{
protected:
    UniversalFactory();

public:
    ~UniversalFactory();
    std::shared_ptr<IOInterface> CreateIO(LogOutput type);

public:
    static UniversalFactory* GetInstance();

private:
    std::map<LogOutput, std::shared_ptr<IOFactory>> fm_;
};













#endif // _LOG_IO_H_

