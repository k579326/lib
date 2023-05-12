#pragma once

template<class T>
using ScopeResourceDeleter = void(*)(T);

template<class T>
class ScopeResource
{
public:
    ScopeResource(T t, ScopeResourceDeleter<T> f)
    {
        _t = t;
        _f = f;
    }
    ~ScopeResource() {
        _f(_t);
    }

    ScopeResource(const ScopeResource&) = delete;
    ScopeResource& operator= (const ScopeResource&) = delete;
private:
    T _t;
    ScopeResourceDeleter<T> _f;
};

template<>
class ScopeResource<void>
{
public:
    ScopeResource(ScopeResourceDeleter<void> f)
    {
        _f = f;
    }
    ~ScopeResource() {
        _f();
    }
    ScopeResource(const ScopeResource&) = delete;
    ScopeResource& operator= (const ScopeResource&) = delete;
private:
    ScopeResourceDeleter<void> _f;
};
