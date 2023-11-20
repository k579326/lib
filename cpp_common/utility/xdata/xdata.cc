
#include "xdata.h"
#include <string.h>

#include <algorithm>

#include <dataencoder.h>
#include <datasource.h>
#include "filesource.h"

#define __SyncBlockBegin()  int _size_self = (int)DataEncoder::Size(this)
#define __SyncBlockEnd()    DataSync::sync(this,  (int)DataEncoder::Size(this) - _size_self)


void Linker::set_parent(Linker* p)
{
    parent_ = p;
}
void Linker::set_next(Linker* p) 
{
    next_ = p;
}
void Linker::set_prev(Linker* p)
{
    prev_ = p;
}

Linker* Linker::parent() const
{
    return parent_;
}
Linker* Linker::next() const
{
    return next_;
}
Linker* Linker::prev() const
{
    return prev_;
}



XData::XData() : type_(ElmType::None), bytes_8_data_(0), complex_data_ptr_(nullptr)
{
}
XData::XData(ElmType type) : type_(type), bytes_8_data_(0)
{
    _default_init(type);
}
XData::~XData()
{
    _clean();
}

XData::XData(int8_t v) : type_(Int8), bytes_8_data_(v), complex_data_ptr_(nullptr) {}
XData::XData(int16_t v) : type_(Int16), bytes_8_data_(v), complex_data_ptr_(nullptr) {}
XData::XData(int32_t v) : type_(Int32), bytes_8_data_(v), complex_data_ptr_(nullptr) {}
XData::XData(int64_t v) : type_(Int64), bytes_8_data_(v), complex_data_ptr_(nullptr) {}
XData::XData(uint8_t v) : type_(UInt8), bytes_8_data_(v), complex_data_ptr_(nullptr) {}
XData::XData(uint16_t v) : type_(UInt16), bytes_8_data_(v), complex_data_ptr_(nullptr) {}
XData::XData(uint32_t v) : type_(UInt32), bytes_8_data_(v), complex_data_ptr_(nullptr) {}
XData::XData(uint64_t v) : type_(UInt64), bytes_8_data_(v), complex_data_ptr_(nullptr) {}
XData::XData(bool v) : type_(Bool), bytes_8_data_(v), complex_data_ptr_(nullptr) {}
XData::XData(float v) : type_(Float), complex_data_ptr_(nullptr) 
{
    memcpy(&bytes_8_data_, &v, sizeof(v));
}
XData::XData(double v) : type_(Double), complex_data_ptr_(nullptr)
{
    memcpy(&bytes_8_data_, &v, sizeof(v));
}
XData::XData(const char* s) : type_(String), bytes_8_data_(0), complex_data_ptr_(nullptr)
{
    string_ = s ? s : "";
}
XData::XData(const char* key, XData* value) : type_(Pair), bytes_8_data_(0)
{
    _default_init(Pair);

    auto pair = std::make_pair(key, value);
    value->_set_parent(this);
    *_pair_pointer() = pair;
}


XData::XData(XData&& other)
{
    type_ = other.type_;
    bytes_8_data_ = other.bytes_8_data_;
    string_ = other.string_;
    complex_data_ptr_ = other.complex_data_ptr_;

    for (XData* child = const_cast<XData*>(other.first_child());
        child != nullptr; 
        child = const_cast<XData*>(child->next_sibling()))
    {
        child->set_parent(this);
    }

    other.type_ = None;
    other.bytes_8_data_ = 0;
    other.string_.clear();
    other.string_.shrink_to_fit();
    other.complex_data_ptr_ = nullptr;
}

XData& XData::operator=(XData&& other)
{
    __SyncBlockBegin();
    _clean();
    type_ = other.type_;
    bytes_8_data_ = other.bytes_8_data_;
    string_ = other.string_;
    complex_data_ptr_ = other.complex_data_ptr_;

    for (XData* child = const_cast<XData*>(other.first_child());
        child != nullptr;
        child = const_cast<XData*>(child->next_sibling()))
    {
        child->set_parent(this);
    }

    other.type_ = None;
    other.bytes_8_data_ = 0;
    other.string_.clear();
    other.string_.shrink_to_fit();
    other.complex_data_ptr_ = nullptr;
    __SyncBlockEnd();
    return *this;
}


XData& XData::operator=(int8_t v)
{
    __SyncBlockBegin();
    if (type() != Int8)
    {
        _clean();
    }
    type_ = Int8;
    memcpy(&bytes_8_data_, &v, sizeof(v));
    __SyncBlockEnd();
    return *this;
}
XData& XData::operator=(int16_t v)
{
    __SyncBlockBegin();
    if (type() != Int16)
    {
        _clean();
    }
    type_ = Int16;
    memcpy(&bytes_8_data_, &v, sizeof(v));
    __SyncBlockEnd();
    return *this;
}
XData& XData::operator=(int32_t v)
{
    __SyncBlockBegin();
    if (type() != Int32)
    {
        _clean();
    }
    type_ = Int32;
    memcpy(&bytes_8_data_, &v, sizeof(v));
    __SyncBlockEnd();
    return *this;
}
XData& XData::operator=(int64_t v)
{
    __SyncBlockBegin();
    if (type() != Int64)
    {
        _clean();
    }
    type_ = Int64;
    memcpy(&bytes_8_data_, &v, sizeof(v));
    __SyncBlockEnd();
    return *this;
}
XData& XData::operator=(uint8_t v)
{
    __SyncBlockBegin();
    if (type() != UInt8)
    {
        _clean();
    }
    type_ = UInt8;
    memcpy(&bytes_8_data_, &v, sizeof(v));
    __SyncBlockEnd();
    return *this;
}
XData& XData::operator=(uint16_t v)
{
    __SyncBlockBegin();
    if (type() != UInt16)
    {
        _clean();
    }
    type_ = UInt16;
    memcpy(&bytes_8_data_, &v, sizeof(v));
    __SyncBlockEnd();
    return *this;
}
XData& XData::operator=(uint32_t v)
{
    __SyncBlockBegin();
    if (type() != UInt32)
    {
        _clean();
    }
    type_ = UInt32;
    memcpy(&bytes_8_data_, &v, sizeof(v));
    __SyncBlockEnd();
    return *this;
}
XData& XData::operator=(uint64_t v)
{
    __SyncBlockBegin();
    if (type() != UInt64)
    {
        _clean();
    }
    type_ = UInt64;
    memcpy(&bytes_8_data_, &v, sizeof(v));
    __SyncBlockEnd();
    return *this;
}
XData& XData::operator=(bool v)
{
    __SyncBlockBegin();
    if (type() != Bool)
    {
        _clean();
    }
    type_ = Bool;
    memcpy(&bytes_8_data_, &v, sizeof(v));
    __SyncBlockEnd();
    return *this;
}
XData& XData::operator=(float v)
{
    __SyncBlockBegin();
    if (type() != Float)
    {
        _clean();
    }
    type_ = Float;
    memcpy(&bytes_8_data_, &v, sizeof(v));
    __SyncBlockEnd();
    return *this;
}
XData& XData::operator=(double v)
{
    __SyncBlockBegin();
    if (type() != Double)
    {
        _clean();
    }
    type_ = Double;
    memcpy(&bytes_8_data_, &v, sizeof(v));
    __SyncBlockEnd();
    return *this;
}
XData& XData::operator=(const char* s)
{
    __SyncBlockBegin();
    if (type() != String)
    {
        _clean();
    }

    type_ = String;
    string_ = s ? s : "";
    __SyncBlockEnd();
    return *this;
}

// for array
XData& XData::append(XData&& elm)
{
    auto n = new XData(std::move(elm));
    switch (type())
    {
    case None:
        _default_init(Array);
        DataSync::update_type(this, Array);
        break;
    case Pair:
        return _pair_pointer()->second->append(n);
    default:
        break;
    }

    if (type() != Array) abort();

    return append(n);
}

XData& XData::append(XData* elm)
{
    if (!_array_add_element(elm))
    {
        abort();
        return *this;
    }
    DataSync::sync(elm, DataSync::calc_move_distance(elm, nullptr));
    return *this;
}

XData& XData::operator[](size_t index)
{
    if (!_array_pointer() || type() != Array)
    {
        abort();
        return XData();
    }

    if (_array_pointer()->size() >= index)
    {
        abort();
        return XData();
    }

    for (auto it = _array_pointer()->begin(); it != _array_pointer()->end(); it++)
    {
        if (index == 0) return **it;
    }

    abort();
    return XData();
}

// for dictionary
XData& XData::operator[](const char* key)
{
    if (type() == None)
    {
        _default_init(Dict);    
        // change type in file
        DataSync::update_type(this, Dict);
    }
    else if (type() == Pair)
    {
        return _pair_pointer()->second->operator[](key);
    }
    XData* result = nullptr;

    if (!_dict_pointer() || type() != Dict)
    {
        abort();
        return XData();
    }
    
    auto it = std::find_if(_dict_pointer()->begin(), _dict_pointer()->end(), [&](XData* e)->bool {
        return e->_pair_pointer()->first == key;
    });

    if (it != _dict_pointer()->end())
    {
        return *((*it)->_pair_pointer()->second);
    }
    else
    {
        XData* v = new XData();
        XData* pair = new XData(key, v);
        DataSync::sync(v, DataSync::calc_move_distance(v, nullptr));

        if (_dict_add_element(pair))
        {
            DataSync::sync(pair, DataSync::calc_move_distance(pair, nullptr));
            return *v;
        }
    }
    abort();
    return XData();
}

ElmType XData::type() const
{
    return type_;
}

char XData::as_char() const
{
    return *(char*)&bytes_8_data_;
}
short XData::as_short() const
{
    return *(short*)&bytes_8_data_;
}
int XData::as_int() const
{
    return *(int*)&bytes_8_data_;
}
long long XData::as_longlong() const
{
    return *(long long*)&bytes_8_data_;
}

unsigned char XData::as_uchar() const
{
    return *(unsigned char*)&bytes_8_data_;
}
unsigned short XData::as_ushort() const
{
    return *(unsigned short*)&bytes_8_data_;
}
unsigned int XData::as_uint() const
{
    return *(unsigned int*)&bytes_8_data_;
}
unsigned long long XData::as_ulonglong() const
{
    return *(unsigned long long*)&bytes_8_data_;
}

bool XData::as_bool() const
{
    return *(bool*)&bytes_8_data_;
}
float XData::as_float() const
{
    return *(float*)&bytes_8_data_;
}
double XData::as_double() const
{
    return *(double*)&bytes_8_data_;
}

const char* XData::as_string() const
{
    return string_.c_str();
}

std::pair<std::string, const XData*> XData::as_pair() const
{
    if (type() != Pair) return {"", nullptr};
    if (!_pair_pointer()) return {"", nullptr};

    //return std::make_pair(_pair_pointer()->first, _pair_pointer()->second);
    return *_pair_pointer();
}

void XData::_clean()
{
    switch (type())
    {
        case String:
            // string_.shrink_to_fit();
            break;
        case Array:
        {
            auto ptr = _array_pointer();
            if (ptr)
            {
                for_each(ptr->begin(), ptr->end(), [](XData* e)->void{ delete e; });
                delete ptr;
                complex_data_ptr_ = nullptr;
            }
            break;
        }
        case Dict:
        {
            auto ptr = _dict_pointer();
            if (ptr)
            {
                for_each(ptr->begin(), ptr->end(), [](XData* e)->void 
                {
                    delete e; 
                });

                delete ptr;
                complex_data_ptr_ = nullptr;
            }
            break;
        }
        case Pair:
        {
            auto ptr = _pair_pointer();
            if (ptr)
            {
                delete ptr->second;
                delete ptr;
                complex_data_ptr_ = nullptr;
            }
            break;
        }
        default:
            break;
    }

    type_ = None;
}

void XData::_copy(const XData& other)
{
    _clean();
    switch (other.type())
    {
    case String:
        string_ = other.string_;
        break;
    case Array:
    {
        complex_data_ptr_ = new ArrayType;
        *_array_pointer() = *(other._array_pointer());
        break;
    }
    case Dict:
    {
        complex_data_ptr_ = new DictType;
        *_dict_pointer() = *(other._dict_pointer());
        break;
    }
    case Pair:
        complex_data_ptr_ = new PairType;
        *_pair_pointer() = *(other._pair_pointer());
        break;
    default:
        bytes_8_data_ = other.bytes_8_data_;
        break;
    }
    type_ = other.type_;
}

XData::ArrayType* XData::_array_pointer() const
{
    return reinterpret_cast<ArrayType*>(complex_data_ptr_);
}
XData::DictType* XData::_dict_pointer() const
{
    return reinterpret_cast<DictType*>(complex_data_ptr_);
}
XData::PairType* XData::_pair_pointer() const
{
    return reinterpret_cast<PairType*>(complex_data_ptr_);
}

bool XData::_array_add_element(XData* data)
{
    // don't add element of Pair type to array.
    if (data->type() == Pair) return false;

    XData* prev = nullptr; 
    if (!_array_pointer()->empty())
    {
        prev = _array_pointer()->back();
    }
    _array_pointer()->push_back(data);
    
    prev ? prev->set_next(data) : void();
    data->set_prev(prev);
    data->_set_parent(this);
    return true;
}


bool XData::_dict_add_element(XData* data)
{
    XData* prev = nullptr; 
    if (!_dict_pointer()->empty())
        prev = _dict_pointer()->back();
    
    data->_set_parent(this);
    data->set_prev(prev);
    if (prev) prev->set_next(data);
    _dict_pointer()->push_back(data);
    return true;
}

void XData::_set_parent(const XData* data)
{
    set_parent(const_cast<XData*>(data));
}


void XData::_default_init(ElmType et)
{
    type_ = et;
    switch (type())
    {
        case Array:
            complex_data_ptr_ = new ArrayType();
            break;
        case Dict:
            complex_data_ptr_ = new DictType();
            break;
        case Pair:
            complex_data_ptr_ = new PairType();
            break;
        default:
            complex_data_ptr_ = nullptr;
            break;
    }
}


const XData* XData::first_child() const
{
    const XData* result = nullptr;
    switch (type())
    {
        case Array:
        {
            auto ptr = _array_pointer();
            if (!ptr || ptr->empty()) result = nullptr;
            else result = *ptr->begin();
            break;
        }
        case Dict:
        {
            auto ptr = _dict_pointer();
            if (!ptr || ptr->empty()) result = nullptr;
            else result = *ptr->begin();
            break;
        }
        case Pair:
        {
            auto ptr = _pair_pointer();
            if (!ptr) result = nullptr;
            else result = ptr->second;
            break;
        }
        default:
            break;
    }
    return result;
}
const XData* XData::next_sibling() const
{
    return static_cast<XData*>(next());
}

bool XData::attach(std::shared_ptr<IDataSource> ds)
{
    if (parent()) return false;
    source_ = ds;
    return true;
}
std::shared_ptr<IDataSource> XData::source() const
{
    if (parent()) return nullptr;
    return source_;
}

bool XData::AttachToFile(XData& data, const CptString& source, SerializeOperation so)
{
    if (data.source()) return false;

    std::shared_ptr<FileSource> s(new FileSource());
    
    if (!s->Init(source, so == XData::SerializeOperation::R ? false : true))
    {
        return false;
    }

    if (!s->Open()) return false;
    if (!data.attach(s)) return false;

    if (so == W)
    {
        s->PrepareHeader();
        DataSync::sync(&data, DataEncoder::Size(&data));
    }
    
    if (so == R || so == RW)
    {
        XData* xdata;
        void* source_data;
        size_t sz = s->ReadAll(&source_data);
        if (!sz) return false;
        if (DataEncoder::decode({ (char*)source_data, sz }, &xdata))
        {
            return false;
        }

        data = std::move(*xdata);
        delete xdata;
    }
    
    return true;
}

