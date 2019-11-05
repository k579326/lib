#ifndef _LOG_IO_H_
#define _LOG_IO_H_

#include <map>
#include <string>
#include <memory>

#include "u_file/util_file.h"

#include "logger_define.h"


class IOInterface
{
public:    
    IOInterface() {}
    virtual ~IOInterface() {}
    
    virtual int Open(const std::string& path) = 0;
    
    virtual int Write(const std::string& content) = 0;
    
    virtual void Close() = 0;
    
};

class FileIO : public IOInterface
{
public:
    FileIO() {}
    ~FileIO() {}

    virtual int Open(const std::string& path) override;
    virtual int Write(const std::string& content) override;
    virtual void Close() override;

private:
    std::shared_ptr<fileutil::File> f_ = nullptr;
};

class ConsoleIO : public IOInterface
{
public:
    ConsoleIO() {}
    ~ConsoleIO() {}

    virtual int Open(const std::string& path) override {
        return -1;
    }
    virtual int Write(const std::string& content) override {
        return -1;
    }
    virtual void Close() override
    {
        return;
    }
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

