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


#define _SCOPENAME(F, L) F##L
#define SetupResourceClean(f) ScopeResource<void> _SCOPENAME(__FILE__, __LINE__)(f)
#define SetupResourceCleanEx(t, f) ScopeResource<decltype(t)> _SCOPENAME(__FILE__, LINE__)(t, f);

