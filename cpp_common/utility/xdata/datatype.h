#pragma once

#include <string>
#include <list>
#include <map>
#include <algorithm>

#include <_basic_type_check.h>

enum ElmType
{
    Int,                // char short int longlong
    Unsigned,           // unsigned Int
    Float,              // float double
    Bool,               // bool
    String,             // char* (utf-8)
    Array,              
    Dict,
    None,
};


class IType
{
public:
    IType(ElmType type = None) : type_(type), ptr_(0) {}
    virtual ~IType() {}

    IType(const IType& O) { Reset(); }

    IType(IType&& other) 
    {
        type_ = other.type_;
        ptr_ = other.ptr_;
        
        other.ptr_ = 0;
        other.type_ = None;
    }

    IType& operator=(IType&& other)
    {
        delete ptr_; 

        type_ = other.type_;
        ptr_ = other.ptr_;
        
        other.ptr_ = 0;
        other.type_ = None;
    }

protected:
    template<class To, class From>
    static To* mutable_cast(const From* p)
    {
        return reinterpret_cast<To*>(const_cast<From*>(p));
    }

    void Reset(void* ptr = nullptr)
    {
        if (ptr_) delete ptr_;
        ptr_ = ptr;
    }
    void ChangeType(ElmType type) { type_ = type; }
    ElmType Type() const { return type_; }
    const void* Ptr() const { return ptr_; }

private:
    ElmType type_;
    void* ptr_;
};


template<class T>
class BasicType : public IType
{
    static_assert(is_basic_type<T>, "type invalid"); 
public:
    BasicType() : IType(std::is_unsigned_v<T> ? Unsigned : Int) 
    {
        // Reset(new T);
    }
    BasicType(T t) : IType(std::is_unsigned_v<T> ? Unsigned : Int)
    { 
        Reset(new T(t));
    }

    T Get() { return *(T*)Ptr(); }

    BasicType(const BasicType& other) : IType(other.Type())
    {
        Reset(new T(other.Get()));
    }

    BasicType& operator=(T t)
    {
        Reset(new T(t))
        ChangeType(std::is_unsigned_v<T> ? Unsigned : Int);
    }
};

class StringType : public IType
{
public:
    StringType() : IType(String) 
    {
        // Reset(new std::string);
    }
    StringType(const char* s) : IType(String)
    {
        if (!s)
            Reset(new std::string(s));
    }
    StringType(const std::string& s) : IType(String)
    {
        Reset(new std::string(s));
    }

    const std::string& Get() const { return *mutable_cast<std::string, void>(Ptr()); }

    StringType& operator=(const char* s) 
    {
        if (!Ptr() || Type() != String)
        {
            Reset(s ? new std::string(s) : nullptr);
        }
        else
        {
            auto ptr = mutable_cast<std::string, void>(Ptr());
            if (s) *ptr = s; else Reset();
        }
    }
    StringType& operator=(const std::string& s)
    {
        if (!Ptr() || Type() != String)
        {
            Reset(new std::string(s));
        }
        else
        {
            *mutable_cast<std::string, void>(Ptr()) = s;
        }
    }
};

class ArrayType : public IType
{
    using _Type = std::list<IType*>;
public:
    ArrayType() : IType(Array) 
    {
        Reset(new _Type);
    }

    virtual ~ArrayType() 
    {
        auto _obj = mutable_cast<_Type, void>(Ptr());
        for_each(_obj->begin(), _obj->end(), [](IType* e)->void{ delete e; });
        _obj->clear();
    }

    ArrayType(const ArrayType& o) : IType(Array)
    {
        Reset(new _Type(*(_Type*)o.Ptr()));
    }



    // ArrayType& operator=(const ArrayType& other)
    // {
    //     if (!Ptr())
    //         Reset(new _Type(*(_Type*)other.Ptr()));
    //     else
    //         *mutable_cast<_Type, void>(Ptr()) = *mutable_cast<_Type, void>(other.Ptr());
    // }

    const std::list<IType*>& Get() const { return *mutable_cast<_Type, void>(Ptr()); } 

    size_t ElementCount() const { return Get().size(); }

    IType* At(size_t index) const 
    { 
        size_t i = 0;

        for (auto it = Get().begin(); it != Get().end(); it++)
        {
            if (i == index)
                return *it;
        } 
        return nullptr;
    }

    void Remove(size_t index)
    {
        size_t i = 0;

        for (auto it = Get().begin(); it != Get().end(); it++)
        {
            if (i == index)
            {
                mutable_cast<_Type, void>(Ptr())->erase(it);
                break;
            }
        }
    }

    template<class VT>
    ArrayType& Append(const VT& value)
    {
        mutable_cast<_Type, void>(Ptr())->push_back();
        return *this;
    }
};

class DictType : public IType
{
    using _Type = std::map<std::string, IType*>;
public:
    DictType() : IType(Dict) 
    {
        Reset(new _Type);
    }
    virtual ~DictType() 
    {
        auto _obj = mutable_cast<_Type, void>(Ptr());
        for_each(_obj->begin(), _obj->end(), [](std::pair<std::string, IType*>& e)->void{ delete e.second; });
        _obj->clear();
    }
    DictType(const DictType& o) : IType(Dict)
    {
        Reset(new _Type(*(_Type*)o.Ptr()));
    }

    IType* Find(const std::string& key)
    {
        auto _owner = mutable_cast<_Type, void>(Ptr());
        auto _it = _owner->find(key);
        if (_it == _owner->end())
        {
            return nullptr;
        }
        return _it->second;
    }

    IType* Insert(const std::string& key, IType* value)
    {
        auto _owner = mutable_cast<_Type, void>(Ptr());
        auto _it = _owner->find(key);
        if (_it == _owner->end())
        {
            _it = _owner->insert(std::make_pair(key, new DictType)).first;
        }
        return _it->second;
    }
};




