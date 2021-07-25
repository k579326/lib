#pragma once


/*
扩展的KMP，支持主流的GBK, UTF8, UTF16编码的字符串匹配
*/

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include <vector>
#include <memory>
#include <map>

#include "u_encode/encode_convert.h"

using namespace codeconvert;

template<EncodeType ET>
struct Encodetraits {};

template<>
struct Encodetraits<UTF8> : public EncodeProperties<UTF8>
{
    static char* char_next(const char* p) {
        if (!char_begin(p)) {
            // never to return nullptr
            return const_cast<char*>(p + 1);
        }
        if (((*p) & U8_MAIN_MASK3) == U8_MAIN_PREFIX3)
            return const_cast<char*>(p + 4);
        if (((*p) & U8_MAIN_MASK2) == U8_MAIN_PREFIX2)
            return const_cast<char*>(p + 3);
        if (((*p) & U8_MAIN_MASK1) == U8_MAIN_PREFIX1)
            return const_cast<char*>(p + 2);
        return const_cast<char*>(p + 1);
    }

    static bool char_equel(const char* f, const char* s, bool icase) {
        uint8_t uf = *f;
        uint8_t us = *s;
        bool f_isascii = (uf >= 'a' && uf <= 'z') || (uf >= 'A' && uf <= 'Z');
        bool s_isascii = (us >= 'a' && us <= 'z') || (us >= 'A' && us <= 'Z');
        bool larger = uf > us;
        if (f_isascii && s_isascii)
        {
            if (!icase) {
                return uf == us;
            }
            else {
                return larger ? us == uf - 32 : us == uf + 32;
            }
        }

        if (!f_isascii == s_isascii)
            return false;

        size_t charsize = 1;
        if ((uf & U8_MAIN_MASK3) == U8_MAIN_PREFIX3)
            charsize = 4;
        if ((uf & U8_MAIN_MASK2) == U8_MAIN_PREFIX2)
            charsize = 3;
        if ((uf & U8_MAIN_MASK1) == U8_MAIN_PREFIX1)
            charsize = 2;

        return memcmp(f, s, charsize) == 0;
    }
private:
    static bool char_begin(const char* p) {
        if ((U8_SUB_MASK & (*p)) == U8_SUB_PREFIX)
            return false;
        return true;
    }
};

template<>
struct Encodetraits<UTF16_LE> : public EncodeProperties<UTF16_LE>
{
    static valuetype* char_next(const valuetype* p) {
        return const_cast<valuetype*>(p + 1);
    }
    static bool char_equel(const valuetype* f, const valuetype* s, bool icase) {
        uint32_t uf = *f;
        uint32_t us = *s;
        bool f_isascii = (uf >= L'a' && uf <= L'z') || (uf >= L'A' && uf <= L'Z');
        bool s_isascii = (us >= L'a' && us <= L'z') || (us >= L'A' && us <= L'Z');
        bool larger = uf > us;
        if (f_isascii && s_isascii)
        {
            if (!icase) {
                return uf == us;
            }
            else {
                return larger ? us == uf - 32 : us == uf + 32;
            }
        }

        return uf == us;
    }
};
template<>
struct Encodetraits<ANSI> : public EncodeProperties<ANSI>
{
    static char* char_next(const char* p) {
        if ((uint8_t)(*p) < 0x80)
            return const_cast<char*>(p + 1);
        else
            return const_cast<char*>(p + 2);
    }
    static bool char_equel(const char* f, const char* s, bool icase) {
        uint8_t uf = *f;
        uint8_t us = *s;
        bool f_isascii = (uf >= 'a' && uf <= 'z') || (uf >= 'A' && uf <= 'Z');
        bool s_isascii = (us >= 'a' && us <= 'z') || (us >= 'A' && us <= 'Z');
        bool larger = uf > us;
        if (f_isascii && s_isascii)
        {
            if (!icase) {
                return uf == us;
            }
            else {
                return larger ? us == uf - 32 : us == uf + 32;
            }
        }

        if (!f_isascii == s_isascii)
            return false;

        return memcmp(f, s, 2) == 0;
    }
};

template<>
struct Encodetraits<UTF16_BE> : public Encodetraits<UTF16_LE>
{};
template<>
struct Encodetraits<UTF8_BOM> : public Encodetraits<UTF8>
{};




class InterfacePattern
{
public:
    virtual ~InterfacePattern() {};
    virtual void GenStepTable(bool icase) { icase_ = icase; }
    virtual void ProcessSourceString() = 0;
    virtual void OptimizStepTable() {}
    virtual int* GetStepTable() = 0;
    virtual size_t GetTableSize() = 0;
    virtual EncodeType GetEncodeType() const { return codetype; }
    virtual bool IsIgnoreCase() const { return icase_; }
protected:
    EncodeType codetype;
    bool icase_;
};


template<EncodeType _ET, class _Traits = Encodetraits<_ET>>
class Pattern : public InterfacePattern
{
public:
    using _EncodeHelp = _Traits;
    using TYPE = typename _EncodeHelp::valuetype;
    using Elm = std::pair<TYPE*, size_t>;   // pair <pointer, bytecount>, not <pointer, stringlen>
    using CHECK_EQ_Func = decltype(&_EncodeHelp::char_equel);
public:
    Pattern() {
        sourestring_ = nullptr;
        sourcesize_ = 0;
        CheckEQ = _EncodeHelp::char_equel;
        codetype = _ET;
    };
    Pattern(const TYPE* begin, size_t size) {
        sourestring_ = new TYPE[size];
        memcpy(sourestring_, begin, size * sizeof(TYPE));
        sourcesize_ = size;
        CheckEQ = _EncodeHelp::char_equel;
        codetype = _ET;
    }
    virtual ~Pattern() {
        delete[] sourestring_;
    }
    

    void ProcessSourceString() 
    {
        realstring_.clear();

        TYPE* cur = sourestring_;
        while (cur < sourestring_ + sourcesize_)
        {
            TYPE* p = _EncodeHelp::char_next(cur);
            assert(p <= sourestring_ + sourcesize_);
            size_t bytesize = 
                (p - cur) * _EncodeHelp::valuesize; // wchar_t*(2) - wchar_t*(0) = 1 ??????? 
            realstring_.push_back(std::make_pair(cur, bytesize));
            cur = p;
        }
        assert(cur == sourestring_ + sourcesize_);
        std::shared_ptr<int> avater(new int[realstring_.size()], 
            [](int* p)-> void{ delete[] p; });
        steptable_ = std::move(avater);
    }

    void GenStepTable(bool icase) {
        int prev = 0;
        decltype(realstring_)& rs = realstring_;
        int* c_next = steptable_.get();

        for (int i = 0; i < rs.size(); i++)
        {
            if (i == 0) {
                c_next[i] = 0;
                continue;
            }

            prev = c_next[i - 1];

            if (prev > 0 && CheckEQ(rs[i].first, rs[prev].first, icase))
            {
                c_next[i] = c_next[i - 1] + 1;
                continue;
            }

            if (prev > 0 && !CheckEQ(rs[i].first, rs[prev].first, icase))
            {
                int pv = prev;
                if (CheckEQ(rs[i].first, rs[0].first, icase))
                    c_next[i] = 1;
                else
                    c_next[i] = 0;

                while (pv != 0) {
                    if (!CheckEQ(rs[pv].first, rs[i].first, icase))
                        pv = c_next[pv - 1];
                    else {
                        c_next[i] = pv + 1;
                        break;
                    }
                }

                continue;
            }

            if (prev <= 0 && CheckEQ(rs[i].first, rs[0].first, icase))
            {
                c_next[i] = 1;
                continue;
            }
            if (prev <= 0 && !CheckEQ(rs[i].first, rs[0].first, icase))
            {
                c_next[i] = 0;
                continue;
            }
        }
        /*
        printf("\n");
        for (int i = 0; i < pattern_.second; i++)
            printf("%d ", c_next[i]);
            */
        for (int i = (int)rs.size() - 1; i > 0; i--)
            c_next[i] = c_next[i - 1];
        c_next[0] = -1;

        /*
        printf("\nδ�Ż���next���飺\n");
        for (int i = 0; i < pattern_.second; i++)
            printf("%d ", c_next[i]);
            */
        // record icase value
        InterfacePattern::GenStepTable(icase);

        return;
    }
    void OptimizStepTable() { }

    int* GetStepTable() { return steptable_.get(); }
    size_t GetTableSize() { return realstring_.size(); }
    const std::vector<Elm>& GetRealString()
    {
        return realstring_;
    }
    size_t GetSourceSize() { return sourcesize_; }

private:
    std::shared_ptr<int> steptable_;
    std::vector<Elm> realstring_;   // used for match
    TYPE* sourestring_;     // source string
    size_t sourcesize_;     // length of source string
    CHECK_EQ_Func CheckEQ;
};




class PatternMgr
{
    
public:
    using PatternPtr = std::shared_ptr<InterfacePattern>;

    PatternPtr& GetPattern(EncodeType type)
    {
        if (type == UTF8_BOM) type = UTF8;
        return Patterns[type];
    }
    
    template<EncodeType _ET, class _Traits = Encodetraits<_ET>>
    bool InputPattern(const typename _Traits::Elm* string, size_t length, bool icase)
    {
        auto to_a = codeconvert::CodeConvert<_ET, ANSI>::convert(string, length);
        auto to_u16l = codeconvert::CodeConvert<_ET, UTF16_LE>::convert(string, length);
        auto to_u16b = codeconvert::CodeConvert<_ET, UTF16_BE>::convert(string, length);
        auto to_u8 = codeconvert::CodeConvert<_ET, UTF8>::convert(string, length);

        Patterns[ANSI] = std::make_shared<Pattern<ANSI>>(to_a.first.get(), to_a.second);
        Patterns[UTF8] = std::make_shared<Pattern<UTF8>>(to_u8.first.get(), to_u8.second);
        Patterns[UTF16_LE] = std::make_shared<Pattern<UTF16_LE>>(to_u16l.first.get(), to_u16l.second);
        Patterns[UTF16_BE] = std::make_shared<Pattern<UTF16_BE>>(to_u16b.first.get(), to_u16b.second);
        Patterns[UTF8_BOM] = std::make_shared<Pattern<UTF8_BOM>>(to_u8.first.get(), to_u8.second);

        for (auto& e: Patterns) {
            e.second->ProcessSourceString();
            e.second->GenStepTable(icase);
        }

        return true;
    }


private:
    std::map<EncodeType, PatternPtr> Patterns;
    
    EncodeType AllEncode[5] = { UTF8, UTF8_BOM, UTF16_LE, UTF16_BE, ANSI };
};


template<EncodeType _ET>
class KMPExtention
{
    using _Helper = Encodetraits<_ET>;
    using Elm = typename _Helper::Elm;
public:
    KMPExtention(bool icase){
        // icase_ = icase;
    }
    ~KMPExtention() {}

    void SetPattern(std::shared_ptr<Pattern<_ET>>& ptn);
    void SetSource(const Elm* source, size_t len);
    Elm* getFirshMatch();
    Elm* getNext();
    std::vector<typename Encodetraits<_ET>::Elm*> getAllMatch();

private:
    Elm* doMatch(const Elm* begin);

private:
    EncodeType encode_type_;
    Elm* next_begin_;
    std::pair<const Elm*, size_t> source_;
    std::shared_ptr<Pattern<_ET>> ptn_;
    bool icase_;
};

template<EncodeType _ET>
void KMPExtention<_ET>::SetPattern(std::shared_ptr<Pattern<_ET>>& ptn)
{
    ptn_ = ptn;
    icase_ = ptn_.get()->IsIgnoreCase();
    return;
}

template<EncodeType _ET>
void KMPExtention<_ET>::SetSource(const Elm* source, size_t len)
{
    source_.first = source;
    source_.second = len;
    encode_type_ = _ET;
}

template<EncodeType _ET>
typename Encodetraits<_ET>::Elm* KMPExtention<_ET>::doMatch(const Elm* begin)
{
    if (begin > source_.first + source_.second) {
        return nullptr;
    }

    int is = 0, ip = 0;
    bool hit = false;

    int* next = ptn_->GetStepTable();
    size_t next_size = ptn_->GetTableSize();
    const std::vector<typename Pattern<_ET>::Elm> x = ptn_->GetRealString();
    const Elm* start = (Elm*)begin;

    for (; start < source_.first + source_.second; )
    {
        // printf("\n%d, %d", is, ip);
        bool eq = _Helper::char_equel(start, x[ip].first, icase_);
        Elm* nextptr = _Helper::char_next(start);
        if (eq)
        {
            if (nextptr > source_.first + source_.second) {
                break;
            }
            if (ip == x.size() - 1) {
                next_begin_ = nextptr;
                hit = true;
                break;
            }
            else {
                start = nextptr;
                ip++;
            }
        }
        else
        {
            if (next[ip] < 0) {
                start = nextptr;
                ip = 0;
            }
            else {
                ip = next[ip];
            }
        }
    }

    if (hit) {
        return const_cast<Elm*>(next_begin_ - ptn_->GetSourceSize());
    }
    else {
        next_begin_ = nullptr;
    }
    return nullptr;
}



template<EncodeType _ET>
typename Encodetraits<_ET>::Elm* KMPExtention<_ET>::getFirshMatch()
{
    return doMatch(source_.first);
}

template<EncodeType _ET>
typename Encodetraits<_ET>::Elm* KMPExtention<_ET>::getNext()
{
    if (!next_begin_)
        return nullptr;
    return doMatch(next_begin_);
}


template<EncodeType _ET>
std::vector<typename Encodetraits<_ET>::Elm*> KMPExtention<_ET>::getAllMatch()
{
    std::vector<Elm*> ret;
    Elm* _match = nullptr;
    next_begin_ = const_cast<Elm*>(source_.first);
    do {
        _match = doMatch(next_begin_);
        if (_match) {
            ret.push_back(_match);
        }
    } while (_match != NULL);
    return ret;
}












