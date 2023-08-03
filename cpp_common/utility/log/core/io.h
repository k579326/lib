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
    IOInterface() : level_(kLevelEnd) {}
    virtual ~IOInterface() {}
    
    virtual int Open(const CptString& path) = 0;
    virtual int Write(const CptString& content) = 0;
    virtual void Close() = 0;

    void SetLevel(LogLevels l) { level_ = l; }
    void SetColor(TextColor c) { color_ = c; }

    LogLevels GetLevel() { return level_; }
    TextColor GetColor() { return color_; }
private:
    LogLevels level_;
    TextColor color_;
};

class FileIO : public IOInterface
{
public:
    FileIO() {}
    ~FileIO() {}

    virtual int Open(const CptString& path) override;
    virtual int Write(const CptString& content) override;
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
    using LogColorMap = std::map<LogLevels, __LogTextColor>;
    using LogColorMapFormOut = std::map<TextColor, __LogTextColor>;

public:
    ConsoleIO()
    {
        color_map_.insert(std::make_pair(kDebugLevel, White));
        color_map_.insert(std::make_pair(kInforLevel, Green));
        color_map_.insert(std::make_pair(kWarningLevel, Yellow));
        color_map_.insert(std::make_pair(kErrorLevel, Purple));
        color_map_.insert(std::make_pair(kFatalLevel, Red));

        out_color_map_.insert(std::make_pair(TC_White, White));
        out_color_map_.insert(std::make_pair(TC_Green, Green));
        out_color_map_.insert(std::make_pair(TC_Yellow, Yellow));
        out_color_map_.insert(std::make_pair(TC_Purple, Purple));
        out_color_map_.insert(std::make_pair(TC_Black, Black));
        out_color_map_.insert(std::make_pair(TC_Red, Red));
        out_color_map_.insert(std::make_pair(TC_Cyan, Cyan));
        out_color_map_.insert(std::make_pair(TC_Blue, Blue));
    }
    ~ConsoleIO();

    virtual int Open(const CptString& path) override;
    virtual int Write(const CptString& content) override;
    virtual void Close() override;

private:
    __LogTextColor _GetColor()
    {
        __LogTextColor c;
        if (GetLevel() == kLevelEnd) {
            auto it = out_color_map_.find(GetColor());
            if (it == out_color_map_.end()) {
                return InvalidColor;
            }
            c = it->second;
        }
        else {
            auto it = color_map_.find(GetLevel());
            if (it == color_map_.end()) {
                return InvalidColor;
            }
            c = it->second;
        }

        return c;
    }

private:
    ConsoleID console_ = (ConsoleID)-1;
    LogColorMap color_map_;
    LogColorMapFormOut out_color_map_;
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

