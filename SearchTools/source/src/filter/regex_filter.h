#pragma once


#include <string>
#include <vector>
#include <regex>

#include "filter.h"

template<class T>
bool StringCompare(const T& t1, const T& t2, bool ignore_case)
{
    if (!ignore_case) {
        return t1 == t2;
    }

    if (t1.size() != t2.size())
        return false;

    for (size_t i = 0; i < t1.size(); i++)
    {
        if (t1[i] == t2[i])
            continue;

        if (t1[i] < t2[i] && 
            t1[i] + 32 == t2[i] &&
            t2[i] >= CPT('a') && t2[i] <= CPT('z'))
            continue;
        if (t2[i] < t1[i] && 
            t2[i] + 32 == t1[i] && 
            t1[i] >= CPT('a') && t1[i] <= CPT('z'))
            continue;
        
        return false;
    }
    return true;
}


template<class T>
struct Traits {
};

template<>
struct Traits<std::string>
{
    using value_type = std::string;
    using regex = std::regex;
};

template<>
struct Traits<std::wstring>
{
    using value_type = std::wstring;
    using regex = std::wregex;
};


template<class _ST, class TT = Traits<_ST>>
class RegexFilter : public InterfaceFilter
{
public:
    using value_type = typename TT::value_type;
    using regex = typename TT::regex;
public:

    void SetFilters(std::vector<_ST> filters, bool is_easy);

    RegexFilter(bool reverse) : 
        InterfaceFilter(reverse, kRegex), is_easy_(true) {}
    ~RegexFilter() = default;

    bool Check(const _ST& filepath) const override;
private:
    bool CheckAsNormalRegex(const _ST& filepath) const;
    bool CheckAsEasyMatch(const _ST& filepath) const;
private:
    bool is_easy_ = true;
    std::vector<_ST> filters_; 
};



template<class _ST, class TT>
void RegexFilter<_ST, TT>::SetFilters(std::vector<_ST> filters, bool is_easy)
{
    filters_ = filters;
    is_easy_ = is_easy;
}


template<class _ST, class TT>
bool RegexFilter<_ST, TT>::Check(const _ST& str) const
{
    if (filters_.empty())
        return true;

    bool ret = !is_easy_ ? CheckAsNormalRegex(str) : 
        CheckAsEasyMatch(str);
    
    return ret ^ reverse_;
}

template<class _ST, class TT>
bool RegexFilter<_ST, TT>::CheckAsNormalRegex(const _ST& filepath) const
{
    RegexFilter::regex rgx;

    for (auto& e : filters_)
    {
        rgx.assign(e, regex::icase);
        if (std::regex_match(filepath, rgx))
            return true;
    }

    return false;
}

template<class _ST, class TT>
bool RegexFilter<_ST, TT>::CheckAsEasyMatch(const _ST& filepath) const
{
    bool result = false;
    _ST x;

    for (auto& e : filters_)
    {
        if (e.size() > filepath.size())
            continue;

        if (*e.begin() == CPT('*') && *(e.end() - 1) == CPT('*'))
        {
            size_t pos_slash = filepath.find_last_of(CPT('/'), 0);
            _ST y = filepath.substr(pos_slash + 1, filepath.size() - pos_slash - 1);
            x = e.substr(1, e.size() - 2);
            if (y.find(x) == _ST::npos)
                result = false;
            else 
                result = true;
        }
        else if (*e.begin() == CPT('*'))
        {
            x = e.substr(1, e.size() - 1);
            _ST y = filepath.substr(filepath.size() - x.size(), x.size());
            result = StringCompare<_ST>(x, y, true);         
        }
        else if (*(e.end() - 1) == CPT('*'))
        {
            x = e.substr(0, e.size() - 1);
            size_t pos_slash = filepath.find_last_of(CPT('/'));
            _ST y = filepath.substr(pos_slash + 1, x.size());
            result = StringCompare<_ST>(x, y, true);
        }

        if (result)
            break;
    }

    return result;
}