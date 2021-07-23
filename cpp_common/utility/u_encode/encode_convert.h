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
    };
    template<>
    struct EncodeProperties<UTF8_BOM> {
        using valuetype = char;
    };
    template<>
    struct EncodeProperties<UTF16_LE> {
        using valuetype = wchar_t;
    };
    template<>
    struct EncodeProperties<UTF16_BE> {
        using valuetype = wchar_t;
    };
    template<>
    struct EncodeProperties<ANSI> {
        using valuetype = char;
    };

    template<EncodeType _F, EncodeType _T>
    struct CodeConvert {
        using FromType = typename EncodeProperties<_F>::valuetype;
        using RetType = typename EncodeProperties<_T>::valuetype;
        using RetValue = std::pair<std::shared_ptr<RetType>, size_t>;
    };

    template<EncodeType _F>
    struct CodeConvert<_F, _F> 
    {
        using FromType = typename EncodeProperties<_F>::valuetype;
        using RetType = typename EncodeProperties<_F>::valuetype;
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
    struct CodeConvert<UTF8, ANSI> {
        using FromType = typename EncodeProperties<UTF8>::valuetype;
        using RetType = typename EncodeProperties<ANSI>::valuetype;
        using RetValue = std::pair<std::shared_ptr<RetType>, size_t>;
        static RetValue convert(const FromType* s, size_t count) {
            size_t outlen;
            RetType* p = Utf8ToAnsi(s, count, &outlen);
            auto ret = std::shared_ptr<RetType>(new RetType[outlen],
                [](RetType* p)->void{ delete[] p; });
            //auto ret = std::make_shared<RetType[]>(outlen);
            memcpy(ret.get(), p, count * sizeof(RetType));
            free(p);
            return std::make_pair(ret, outlen);
        }
    };
    template<>
    struct CodeConvert<UTF8, UTF16_LE> {
        using FromType = typename EncodeProperties<UTF8>::valuetype;
        using RetType = typename EncodeProperties<UTF16_LE>::valuetype;
        using RetValue = std::pair<std::shared_ptr<RetType>, size_t>;
        static RetValue convert(const FromType* s, size_t count) {
            size_t outcount;
            RetType* p = Utf8ToUtf16LE(s, count, &outcount);
            auto ret = std::shared_ptr<RetType>(new RetType[outcount],
                [](RetType* p)->void { delete[] p; });
            //auto ret = std::make_shared<RetType[]>(outcount);
            memcpy(ret.get(), p, count * sizeof(RetType));
            free(p);
            return std::make_pair(ret, outcount);
        }
    };
    /*template<>
    struct CodeConvert<UTF8, UTF16_BE> {
        using FromType = EncodeProperties<UTF8>::valuetype;
        using RetType = EncodeProperties<UTF16_BE>::valuetype;
        using RetValue = std::pair<std::shared_ptr<RetType[]>, size_t>;
        static RetValue convert(const FromType* s, size_t count) {
            size_t outcount;
            RetType* p = Utf8ToUtf16(s, count, &outcount);
            auto ret = std::make_shared<RetType[]>(outcount);
            memcpy(ret.get(), p, count * sizeof(RetType));
            return std::make_pair(ret, outcount);
        }
    };*/
    template<>
    struct CodeConvert<ANSI, UTF8> {
        using FromType = typename EncodeProperties<ANSI>::valuetype;
        using RetType = typename EncodeProperties<UTF8>::valuetype;
        using RetValue = std::pair<std::shared_ptr<RetType>, size_t>;
        static RetValue convert(const FromType* s, size_t count) {
            size_t outcount;
            RetType* p = AnsiToUtf8(s, count, &outcount);
            auto ret = std::shared_ptr<RetType>(new RetType[outcount],
                [](RetType* p)->void { delete[] p; });
            //auto ret = std::make_shared<RetType[]>(outcount);
            memcpy(ret.get(), p, outcount * sizeof(RetType));
            free(p);
            return std::make_pair(ret, outcount);
        }
    };
    template<>
    struct CodeConvert<ANSI, UTF16_LE> {
        using FromType = typename EncodeProperties<ANSI>::valuetype;
        using RetType = typename EncodeProperties<UTF16_LE>::valuetype;
        using RetValue = std::pair<std::shared_ptr<RetType>, size_t>;
        static RetValue convert(const FromType* s, size_t count) {
            size_t outcount;
            RetType* p = AnsiToUtf16LE(s, count, &outcount);
            auto ret = std::shared_ptr<RetType>(new RetType[outcount],
                [](RetType* p)->void { delete[] p; });
            //auto ret = std::make_shared<RetType[]>(outcount);
            memcpy(ret.get(), p, outcount * sizeof(RetType));
            free(p);
            return std::make_pair(ret, outcount);
        }
    };
    /*template<>
    struct CodeConvert<ANSI, UTF16_BE> {
        using FromType = EncodeProperties<ANSI>::valuetype;
        using RetType = EncodeProperties<UTF16_BE>::valuetype;
        using RetValue = std::pair<std::shared_ptr<RetType[]>, size_t>;
        static RetValue convert(const FromType* s, size_t count) {
            size_t outcount;
            RetType* p = AnsiToUtf8(s, count, &outcount);
            auto ret = std::make_shared<RetType[]>(outcount);
            memcpy(ret.get(), p, count * sizeof(RetType));
            return std::make_pair(ret, outcount);
        }
    };*/
    template<>
    struct CodeConvert<UTF16_LE, UTF8> {
        using FromType = typename EncodeProperties<UTF16_LE>::valuetype;
        using RetType = typename EncodeProperties<UTF8>::valuetype;
        using RetValue = std::pair<std::shared_ptr<RetType>, size_t>;
        static RetValue convert(const FromType* s, size_t count) {
            size_t outcount;
            RetType* p = Utf16LEToUtf8(s, count, &outcount);
            auto ret = std::shared_ptr<RetType>(new RetType[outcount],
                [](RetType* p)->void { delete[] p; });
            //auto ret = std::make_shared<RetType[]>(outcount);
            memcpy(ret.get(), p, outcount * sizeof(RetType));
            free(p);
            return std::make_pair(ret, outcount);
        }
    };
    template<>
    struct CodeConvert<UTF16_LE, ANSI> {
        using FromType = typename EncodeProperties<UTF16_LE>::valuetype;
        using RetType = typename EncodeProperties<ANSI>::valuetype;
        using RetValue = std::pair<std::shared_ptr<RetType>, size_t>;
        static RetValue convert(const FromType* s, size_t count) {
            size_t outcount;
            RetType* p = Utf16LEToAnsi(s, count, &outcount);
            auto ret = std::shared_ptr<RetType>(new RetType[outcount],
                [](RetType* p)->void { delete[] p; });
            //auto ret = std::make_shared<RetType[]>(outcount);
            memcpy(ret.get(), p, outcount * sizeof(RetType));
            free(p);
            return std::make_pair(ret, outcount);
        }
    };
    template<>
    struct CodeConvert<UTF16_LE, UTF16_BE> {
        using FromType = typename EncodeProperties<UTF16_LE>::valuetype;
        using RetType = typename EncodeProperties<UTF16_BE>::valuetype;
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
        static RetValue convert(const FromType* s, size_t count) {
            auto _BtoL = CodeConvert<UTF16_LE, UTF16_BE>::convert(s, count);
            return CodeConvert <UTF16_LE, _T>::convert(_BtoL.first.get(), _BtoL.second);
        }
    };
    template<EncodeType _T>
    struct CodeConvert<_T, UTF16_BE> : public CodeConvert<_T, UTF16_LE>
    {
        static RetValue convert(const FromType* s, size_t count) {
            auto _to16L = CodeConvert<_T, UTF16_LE>::convert(s, count);
            return CodeConvert<UTF16_LE, UTF16_BE>::convert(_to16L.first.get(), _to16L.second);
        }
    };

    /*template<EncodeType _T>
    struct CodeConvert<_T, UTF8_BOM> : public CodeConvert<_T, UTF8> {};
    */

    
    template<EncodeType _T>
    struct CodeConvert<UTF8_BOM, _T> : public CodeConvert<UTF8, _T> {};


};
