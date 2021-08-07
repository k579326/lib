
#include <core/io.h>

#include "u_path/util_path.h"
#include "u_dir/util_dir.h"

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

int FileIO::Open(const std::string& path)
{
    std::string dir = pathutil::GetDirOfPathName(path);
    if (CommCreateDir(dir.c_str())) {
        return -1;
    }

    f_ = std::make_shared<fileutil::File>(path.c_str());
    if (!f_) {
        return -1;
    }

    return f_->Open(kAppendForce, kRdWr);
}

int FileIO::Write(const std::string& content, __LogTextColor textcolor)
{
    if (!f_) {
        return -1;
    }
    
    int ret = f_->Write(content.c_str(), content.size());
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

int ConsoleIO::Open(const std::string& path)
{
    console_ = GetConsole();
    return (int)console_ == -1 ? -1 : 0;
}

int ConsoleIO::Write(const std::string& content, __LogTextColor textcolor)
{
    int ret;
    if ((int)console_ == -1)
        return -1;

    SetConsoleTextColor(console_, textcolor, true);
    ret = WriteConsole_(console_, content.c_str(), content.size());
    RestoreConsole(console_);
    return ret;
}

void ConsoleIO::Close()
{
    console_ = (ConsoleID)-1;
    return;
}










