

#include "printer.h"


SyncPrinter::SyncPrinter(LogOutput mode)
{
    io_ = UniversalFactory::GetInstance()->CreateIO(mode);
}
SyncPrinter::~SyncPrinter()
{
    std::unique_lock<std::mutex> autolock(io_lock_);
    io_->Close();
}
void SyncPrinter::Output(_LOGPROPERTY& property)
{
    std::unique_lock<std::mutex> autolock(io_lock_);
    property.logstr += TEXT("\n");
    io_->SetLevel(property.level);
    io_->SetColor(property.color);
    io_->Write(property.logstr);
}
void SyncPrinter::SetIO(const CptString& path)
{
    std::unique_lock<std::mutex> autolock(io_lock_);
    io_->Close();
    io_->Open(path);
}



