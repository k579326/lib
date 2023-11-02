#pragma once

#include <stdint.h>

#include <string>
#include <list>
#include <map>
#include <memory>


#include "datasync.h"
#include "cpt-string.h"

enum ElmType : char
{
    Int8,
    Int16,
    Int32,
    Int64,
    UInt8,
    UInt16,
    UInt32,
    UInt64,
    Float,              // float 
    Double,
    Bool,               // bool
    String,             // char* (utf-8)
    Array,    
    Pair,               
    Dict,
    None = 127,
};

class XData : public Linker, public Position
{
    using ArrayType = std::list<XData*>;
    using PairType = std::pair<std::string, XData*>;
    using DictType = std::list<XData*>;
    friend class DataSync;
    friend class DataEncoder;
public:
    XData();
    XData(ElmType type);
    virtual ~XData();

    XData(int8_t v);
    XData(int16_t v);
    XData(int32_t v);
    XData(int64_t v);
    XData(uint8_t v);
    XData(uint16_t v);
    XData(uint32_t v);
    XData(uint64_t v);
    XData(bool v);
    XData(float v);
    XData(double v);
    XData(const char* s);

    // XData(const XData&);
    XData(XData&&);

    XData& operator=(int8_t v);
    XData& operator=(int16_t v);
    XData& operator=(int32_t v);
    XData& operator=(int64_t v);
    XData& operator=(uint8_t v);
    XData& operator=(uint16_t v);
    XData& operator=(uint32_t v);
    XData& operator=(uint64_t v);
    XData& operator=(bool v);
    XData& operator=(float v);
    XData& operator=(double v);
    XData& operator=(const char* s);

    // XData& operator=(const XData& other);
    XData& operator=(XData&& other);

    // for array
    XData& append(XData&& elm);
    XData& operator[](size_t index);

    // for dictionary
    XData& operator[](const char* key);

    ElmType type() const;

    char as_char() const;
    short as_short() const;
    int as_int() const;
    long long as_longlong() const;

    unsigned char as_uchar() const;
    unsigned short as_ushort() const;
    unsigned int as_uint() const;
    unsigned long long as_ulonglong() const;

    bool as_bool() const;
    float as_float() const;
    double as_double() const;

    const char* as_string() const;
    std::pair<std::string, const XData*> as_pair() const;

    const XData* first_child() const;
    const XData* next_sibling() const; 
private:
    XData& append(XData* elm);
    XData(const char* key, XData* value);

private:
    void _default_init(ElmType type);
    void _clean();
    void _copy(const XData& other);

    bool _array_add_element(XData* data);
    bool _dict_add_element(XData* data);

    void _set_parent(const XData* data);

    ArrayType* _array_pointer() const;
    DictType* _dict_pointer() const;
    PairType* _pair_pointer() const;

    bool attach(std::shared_ptr<class IDataSource> ds);
    std::shared_ptr<class IDataSource> source() const;
private:
    ElmType type_;
    uint64_t bytes_8_data_;
    std::string string_;
    void* complex_data_ptr_;
    std::shared_ptr<class IDataSource> source_ = nullptr;

// static 
public:
    enum SerializeOperation { R, W, RW};
    static bool AttachToFile(XData& data, const CptString& source, SerializeOperation so);
};





