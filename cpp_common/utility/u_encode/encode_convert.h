#pragma once

#include <assert.h>
#include <string.h>

#include <map>
#include <string>
#include <memory>

#include "encode_def.h"
#include "platform_codecvt.h"

namespace codeconvert
{
    template<EncodeType E>
    struct EncodeProperties {};

    template<>
    struct EncodeProperties<UTF8> {
        using valuetype = char;
        using Elm = valuetype;
        using flen = decltype(&strlen);
        
        static constexpr size_t valuesize = sizeof(valuetype);
        static constexpr flen getlength = &strlen;
    };
    template<>
    struct EncodeProperties<UTF8_BOM> {
        using valuetype = char;
        using Elm = valuetype;
        using flen = decltype(&strlen);

        static constexpr size_t valuesize = sizeof(valuetype);
        static constexpr flen getlength = &strlen;
    };
    template<>
    struct EncodeProperties<UTF16_LE> {
        using valuetype = cv_char16;
        using Elm = valuetype;
        using flen = size_t (*)(const cv_char16* string);
    private:
        static size_t _cv_wcslen(const cv_char16* s) {
            size_t count = 0;
            while (*s++ != 0) {
                count++;
            }
            return count;
        }
    public:
        static constexpr size_t valuesize = sizeof(valuetype);
        static constexpr flen getlength = &EncodeProperties<UTF16_LE>::_cv_wcslen;
    };
    template<>
    struct EncodeProperties<UTF16_BE> : public EncodeProperties<UTF16_LE>{};

    template<>
    struct EncodeProperties<ANSI> {
        using valuetype = char;
        using Elm = valuetype;
        using flen = decltype(&strlen);

        static constexpr size_t valuesize = sizeof(valuetype);
        static constexpr flen getlength = &strlen;
    };

    struct pair_less {
        using element = std::pair<EncodeType, EncodeType>;
        bool operator ()(const element& a, const element& b) const 
        {
            if (a.first == b.first)
                return a.second < b.second;
            else
                return a.first < b.first;
        }
    };

    static const std::map<std::pair<EncodeType, EncodeType>, void*, pair_less>
    func_map = 
    {   { std::pair<EncodeType, EncodeType>(UTF8, ANSI), (void*)&Utf8ToAnsi },
        { std::pair<EncodeType, EncodeType>(UTF8, UTF16_LE), (void*)&Utf8ToUtf16LE },
        { std::pair<EncodeType, EncodeType>(ANSI, UTF16_LE), (void*)&AnsiToUtf16LE },
        { std::pair<EncodeType, EncodeType>(ANSI, UTF8), (void*)&AnsiToUtf8 },
        { std::pair<EncodeType, EncodeType>(UTF16_LE, ANSI), (void*)&Utf16LEToAnsi },
        { std::pair<EncodeType, EncodeType>(UTF16_LE, UTF8), (void*)&Utf16LEToUtf8 },
    };

    template<EncodeType _F, EncodeType _T>
    struct CodeConvert {
        using _S_EP = EncodeProperties<_F>;
        using _D_EP = EncodeProperties<_T>;
        using FromType = typename _S_EP::valuetype;
        using RetType = typename _D_EP::valuetype;
        using RetValue = std::pair<std::shared_ptr<RetType>, size_t>;
        using CVFUNC = RetType* (*)(const FromType* s, size_t count, size_t* outc);

        static RetValue convert(const FromType* s, size_t count) {
            size_t outcount;
            constexpr auto key = std::make_pair(_F, _T);
            auto it = func_map.find(key);
            if (it == func_map.end())
                return RetValue(nullptr, 0);
            RetType* p = ((CVFUNC)it->second)(s, count * _S_EP::valuesize, &outcount);
            auto ret = std::shared_ptr<RetType>(new RetType[outcount / _D_EP::valuesize],
                [](RetType* p)->void { delete[] p; });
            //auto ret = std::make_shared<RetType[]>(outlen);
            memcpy(ret.get(), p, outcount);
            free(p);
            return std::make_pair(ret, outcount / _D_EP::valuesize);
        }
    };

    template<EncodeType _F>
    struct CodeConvert<_F, _F> 
    {
        using _S_EP = EncodeProperties<_F>;
        using _D_EP = EncodeProperties<_F>;
        using FromType = typename _S_EP::valuetype;
        using RetType = typename _D_EP::valuetype;
        using RetValue = std::pair<std::shared_ptr<RetType>, size_t>;
        static RetValue convert(const RetType* s, size_t count) {
            auto ret = std::shared_ptr<RetType>(new RetType[count], 
                [](RetType* p)->void{ delete[] p; });
            // auto ret = std::make_shared<RetType[]>(new RetType[count]);
            memcpy(ret.get(), s, count * sizeof(RetType));
            return std::make_pair(ret, count);
        }
    };

    template<>
    struct CodeConvert<UTF16_LE, UTF16_BE> {
        using _S_EP = EncodeProperties<UTF16_LE>;
        using _D_EP = EncodeProperties<UTF16_BE>;
        using FromType = typename _S_EP::valuetype;
        using RetType = typename _D_EP::valuetype;
        using RetValue = std::pair<std::shared_ptr<RetType>, size_t>;
        static RetValue convert(const FromType* s, size_t count) {
            assert((std::is_same<FromType, RetType>::value));
            auto ret = std::shared_ptr<RetType>(new RetType[count],
                [](RetType* p)->void { delete[] p; });
            memcpy(ret.get(), s, sizeof(RetType) * count);
            //auto ret = std::make_shared<RetType[]>(new RetType[count]);
            size_t size_element = sizeof(RetType);
            size_t mid = size_element / 2;

            if (mid != 0) {
                for (size_t k = 0; k < count; k++)
                {
                    uint8_t* p = reinterpret_cast<uint8_t*>(&ret.get()[k]);
                    for (size_t i = 0; i < mid; i++)
                    {
                        std::swap<uint8_t>(p[i], p[size_element-i-1]);
                    }
                }
            }
            return std::make_pair(ret, count);
        }
    };

    template<>
    struct CodeConvert<UTF16_BE, UTF16_LE> : public CodeConvert<UTF16_LE, UTF16_BE>
    {};

    template<EncodeType _T>
    struct CodeConvert<UTF16_BE, _T> : public CodeConvert<UTF16_LE, _T>
    {
        using RetValue = typename CodeConvert<UTF16_LE, _T>::RetValue;
        using FromType = typename CodeConvert<UTF16_LE, _T>::FromType;
        static RetValue convert(const FromType* s, size_t count) {
            auto _BtoL = CodeConvert<UTF16_BE, UTF16_LE>::convert(s, count);
            return CodeConvert<UTF16_LE, _T>::convert(_BtoL.first.get(), _BtoL.second);
        }
    };
    template<EncodeType _T>
    struct CodeConvert<_T, UTF16_BE> : public CodeConvert<_T, UTF16_LE>
    {
        using RetValue = typename CodeConvert<_T, UTF16_LE>::RetValue;
        using FromType = typename CodeConvert<_T, UTF16_LE>::FromType;
        static RetValue convert(const FromType* s, size_t count) {
            auto _to16L = CodeConvert<_T, UTF16_LE>::convert(s, count);
            return CodeConvert<UTF16_LE, UTF16_BE>::convert(_to16L.first.get(), _to16L.second);
        }
    };

    /* there two full specialization conquer the conflict of partial specialization from UTF8_BOM and UTF16_BE*/
    template<>
    struct CodeConvert<UTF8_BOM, UTF16_BE> : public CodeConvert<UTF8, UTF16_BE> {};
    template<>
    struct CodeConvert<UTF16_BE, UTF8_BOM> : public CodeConvert<UTF16_BE, UTF8> {};

    template<EncodeType _T>
    struct CodeConvert<_T, UTF8_BOM> : public CodeConvert<_T, UTF8> {};
    template<EncodeType _T>
    struct CodeConvert<UTF8_BOM, _T> : public CodeConvert<UTF8, _T> {};




    static std::pair<std::shared_ptr<void>, size_t>
    EasyConvert(EncodeType f, EncodeType t, void* stream, size_t bytesize)
    {
        if (f == UTF8_BOM) f = UTF8;
        if (t == UTF8_BOM) t = UTF8;

        if (f == t) {
            std::pair<std::shared_ptr<void>, size_t> ret;
            ret.first = std::shared_ptr<void>(new char[bytesize], 
                [](void* p)->void { delete[] p; });
            memcpy(ret.first.get(), stream, bytesize);
            ret.second = bytesize;
            return ret;
        }

        if (f == ANSI && t == UTF8) {
            std::pair<std::shared_ptr<void>, size_t> ret;
            auto tostring = CodeConvert<ANSI, UTF8>::convert((const char*)stream, bytesize);
            ret.first = std::reinterpret_pointer_cast<char>(tostring.first);
            ret.second = tostring.second;
            return ret;
        }
        if (f == UTF8 && t == ANSI) {
            std::pair<std::shared_ptr<void>, size_t> ret;
            auto tostring = CodeConvert<UTF8, ANSI>::convert((const char*)stream, bytesize);
            ret.first = std::reinterpret_pointer_cast<char>(tostring.first);
            ret.second = tostring.second;
            return ret;
        }
        if (f == ANSI && t == UTF16_LE) {
            std::pair<std::shared_ptr<void>, size_t> ret;
            auto tostring = CodeConvert<ANSI, UTF16_LE>::convert((const char*)stream, bytesize);
            ret.first = std::reinterpret_pointer_cast<cv_char16>(tostring.first);
            ret.second = tostring.second * sizeof(cv_char16);
            return ret;
        }
        if (f == ANSI && t == UTF16_BE) {
            std::pair<std::shared_ptr<void>, size_t> ret;
            auto tostring = CodeConvert<ANSI, UTF16_BE>::convert((const char*)stream, bytesize);
            ret.first = std::reinterpret_pointer_cast<cv_char16>(tostring.first);
            ret.second = tostring.second * sizeof(cv_char16);
            return ret;
        }
        if (f == UTF16_LE && t == ANSI) {
            std::pair<std::shared_ptr<void>, size_t> ret;
            auto tostring = CodeConvert<UTF16_LE, ANSI>::convert((const cv_char16*)stream, bytesize / sizeof(cv_char16));
            ret.first = std::reinterpret_pointer_cast<char>(tostring.first);
            ret.second = tostring.second;
            return ret;
        }
        if (f == UTF16_BE && t == ANSI) {
            std::pair<std::shared_ptr<void>, size_t> ret;
            auto tostring = CodeConvert<UTF16_BE, ANSI>::convert((const cv_char16*)stream, bytesize / sizeof(cv_char16));
            ret.first = std::reinterpret_pointer_cast<char>(tostring.first);
            ret.second = tostring.second;
            return ret;
        }
        if (f == UTF16_LE && t == UTF8) {
            std::pair<std::shared_ptr<void>, size_t> ret;
            auto tostring = CodeConvert<UTF16_LE, UTF8>::convert((const cv_char16*)stream, bytesize / sizeof(cv_char16));
            ret.first = std::reinterpret_pointer_cast<char>(tostring.first);
            ret.second = tostring.second;
            return ret;
        }
        if (f == UTF16_BE && t == UTF8) {
            std::pair<std::shared_ptr<void>, size_t> ret;
            auto tostring = CodeConvert<UTF16_BE, UTF8>::convert((const cv_char16*)stream, bytesize / sizeof(cv_char16));
            ret.first = std::reinterpret_pointer_cast<char>(tostring.first);
            ret.second = tostring.second;
            return ret;
        }
        if (f == UTF16_LE && t == UTF16_BE) {
            std::pair<std::shared_ptr<void>, size_t> ret;
            auto tostring = CodeConvert<UTF16_LE, UTF16_BE>::
                convert((const cv_char16*)stream, bytesize / sizeof(cv_char16));
            ret.first = std::reinterpret_pointer_cast<cv_char16>(tostring.first);
            ret.second = tostring.second * sizeof(cv_char16);
            return ret;
        }
        if (f == UTF16_BE && t == UTF16_LE) {
            std::pair<std::shared_ptr<void>, size_t> ret;
            auto tostring = CodeConvert<UTF16_BE, UTF16_LE>::
                convert((const cv_char16*)stream, bytesize / sizeof(cv_char16));
            ret.first = std::reinterpret_pointer_cast<cv_char16>(tostring.first);
            ret.second = tostring.second * sizeof(cv_char16);
            return ret;
        }
        if (f == UTF8 && t == UTF16_LE) {
            std::pair<std::shared_ptr<void>, size_t> ret;
            auto tostring = CodeConvert<UTF8, UTF16_LE>::convert((const char*)stream, bytesize);
            ret.first = std::reinterpret_pointer_cast<cv_char16>(tostring.first);
            ret.second = tostring.second * sizeof(cv_char16);
            return ret;
        }
        if (f == UTF8 && t == UTF16_BE) {
            std::pair<std::shared_ptr<void>, size_t> ret;
            auto tostring = CodeConvert<UTF8, UTF16_BE>::convert((const char*)stream, bytesize);
            ret.first = std::reinterpret_pointer_cast<cv_char16>(tostring.first);
            ret.second = tostring.second * sizeof(cv_char16);
            return ret;
        }

        return std::pair<std::shared_ptr<void>, size_t>();
    }
};
