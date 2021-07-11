#pragma once

#include "search_def.h"


enum FilterType
{
    kRegex = 0,
    kFileSize,
};

class InterfaceFilter
{
public:
    InterfaceFilter(bool reverse, FilterType type) : 
        reverse_(reverse), type_(type) {}
    virtual ~InterfaceFilter() {}

    virtual bool Check(const CPTString& target) const = 0;
    virtual FilterType GetType() const { return type_; }
protected:
    // revert the Checking result
    bool reverse_ = true;

    FilterType type_;
};