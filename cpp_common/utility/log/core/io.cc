
#include <core/io.h>

#include "path/util_path.h"
#include "dir/util_dir.h"
#include "encode/platform_codecvt.h"

using namespace fileutil;

UniversalFactory::UniversalFactory()
{
    fm_.insert(std::make_pair(LogOutput::kFileModel, std::make_shared<FileIOFactory>()));
    fm_.insert(std::make_pair(LogOutput::kConsoleModel, std::make_shared<ConsoleIOFactory>()));
}

UniversalFactory::~UniversalFactory()
{

}

UniversalFactory* UniversalFactory::GetInstance()
{
    static UniversalFactory inst;
    return &inst;
}


std::shared_ptr<IOInterface> UniversalFactory::CreateIO(LogOutput type)
{
    auto it = fm_.find(type);
    if (it == fm_.end()) {
        return nullptr;
    }

    return it->second->CreateIO();
}

int FileIO::Open(const CptString& path)
{
    CptString dir = pathutil::GetDirOfPathName(path);
    if (CommCreateDir(dir.c_str())) {
        return -1;
    }

    f_ = std::make_shared<fileutil::File>(path.c_str());
    if (!f_) {
        return -1;
    }

    return f_->Open(kAppendForce, kRdWr);
}

int FileIO::Write(const CptString& content, __LogTextColor textcolor)
{
    int ret = 0;
    if (!f_) {
        return -1;
    }
    
#if defined _UNICODE || defined UNICODE
    std::string localstring = U16StringToU8String(content);
    ret = f_->Write(localstring.c_str(), localstring.size());
#else
    ret = f_->Write(content.c_str(), content.size());
#endif
    f_->Flush();
    return ret;
}

void FileIO::Close()
{
    if (f_) f_->Close();
}



/* console io */

ConsoleIO::ConsoleIO()
{
}

ConsoleIO::~ConsoleIO()
{
}

int ConsoleIO::Open(const CptString& path)
{
    console_ = GetConsole();
    return (int)console_ == -1 ? -1 : 0;
}

int ConsoleIO::Write(const CptString& content, __LogTextColor textcolor)
{
    int ret;
    if ((int)console_ == -1)
        return -1;

    SetConsoleTextColor(console_, textcolor, true);
    // Windows控制台只可正常显示属于本地字符集编码的字符
#if defined _UNICODE || defined UNICODE
    std::string localstring = U16StringToAnsiString(content);
    ret = WriteConsole_(console_, localstring.c_str(), localstring.size());
#else
    ret = WriteConsole_(console_, content.c_str(), content.size());
#endif
    RestoreConsole(console_);
    return ret;
}

void ConsoleIO::Close()
{
    console_ = (ConsoleID)-1;
    return;
}










