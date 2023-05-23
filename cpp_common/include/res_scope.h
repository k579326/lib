#pragma once

#include <algorithm>
#include <functional>

class ScopeResource
{
public:
    template<class D, class... T>
    ScopeResource(D&& f, T&&... t)
    {
        _f = std::bind(std::forward<D>(f), std::forward<T>(t)...);
    }

    ~ScopeResource() {
        if (_f) _f();
    }

    ScopeResource(const ScopeResource&) = delete;
    ScopeResource& operator= (const ScopeResource&) = delete;
private:
    std::function<void()> _f;
};

#define _SCOPENAME(F, L) F##L
#define _VARIABLENAME(L) _SCOPENAME(__FUNCTION__, L)
#define SetupResourceClean(f, ...) ScopeResource _VARIABLENAME(__LINE__)(f, ##__VA_ARGS__)


