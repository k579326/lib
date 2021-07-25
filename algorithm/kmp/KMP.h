#ifndef _KMP_ALGORITHM_H_
#define _KMP_ALGORITHM_H_

/*
原始的KMP算法，仅支持英文
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <memory>
#include <vector>


template<class Elm>
class KMP
{

public:
    KMP(const Elm* source, size_t slen, const Elm* pattern, size_t plen);
    ~KMP() {}
    
    Elm* getFirshMatch();
    Elm* getNext();
    std::vector<Elm*> getAllMatch();

private:
    void genPatternTable();
    void optimizTable();

    Elm* doMatch(const Elm* begin);

private:
    Elm* next_begin_;
    std::shared_ptr<int[]> next_;
    const std::pair<const Elm*, size_t> source_;
    const std::pair<const Elm*, size_t> pattern_;
};



template<class Elm>
KMP<Elm>::KMP(const Elm* source, size_t slen, const Elm* pattern, size_t plen) :
    source_(source, slen), pattern_(pattern, plen),
    next_(nullptr)
{
    next_begin_ = nullptr;
    next_.reset(new int[plen]);
    memset(next_.get(), 0, plen * sizeof(Elm));

    genPatternTable();
    optimizTable();
}


template<class Elm>
void KMP<Elm>::genPatternTable()
{
    int prev = 0;
    const Elm* c_pattern = pattern_.first;
    int* c_next = next_.get();

    for (int i = 0; i < pattern_.second; i++)
    {
        if (i == 0) {
            c_next[i] = 0;
            continue;
        }

        prev = c_next[i - 1];

        if (prev > 0 &&
            c_pattern[i] == c_pattern[prev])
        {
            c_next[i] = c_next[i - 1] + 1;
            continue;
        }

        if (prev > 0 &&
            c_pattern[i] != c_pattern[prev])
        {
            int pv = prev;
            
            if (c_pattern[i] == c_pattern[0])
                c_next[i] = 1;
            else
                c_next[i] = 0;

            while (pv != 0) {
                if (c_pattern[pv] != c_pattern[i])
                    pv = c_next[pv - 1];
                else {
                    c_next[i] = pv + 1;
                    break;
                }
            }

            continue;
        }
        if (prev <= 0 && c_pattern[i] == c_pattern[0])
        {
            c_next[i] = 1;
            continue;
        }
        if (prev <= 0 && c_pattern[i] != c_pattern[0])
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
    for (int i = (int)pattern_.second - 1; i > 0; i--)
        c_next[i] = c_next[i - 1];
    c_next[0] = -1;

    /*
    printf("\n未优化的next数组：\n");
    for (int i = 0; i < pattern_.second; i++)
        printf("%d ", c_next[i]);
        */
    return;
}

template<class Elm>
void KMP<Elm>::optimizTable()
{
    int* c_next = next_.get();

    for (int i = (int)pattern_.second - 1; i > 0; i--)
    {
        int steps = c_next[i];
        if (pattern_.first[steps] == pattern_.first[i]) {
            c_next[i] = (c_next[i] == 0 ? -1 : c_next[steps]);
        }
    }

    // printf("\n优化后的next数组：\n");
    // for (int i = 0; i < pattern_.second; i++) {
    //     printf("%d ", c_next[i]);
    // }
}



template<class Elm>
Elm* KMP<Elm>::getFirshMatch()
{
    return doMatch(source_.first);
}

template<class Elm>
Elm* KMP<Elm>::getNext()
{
    if (!next_begin_)
        return nullptr;
    return doMatch(next_begin_);
}


template<class Elm>
std::vector<Elm*> KMP<Elm>::getAllMatch()
{
    std::vector<Elm*> ret;

    Elm* _match = nullptr;
    const Elm* _begin = source_.first;
    do {
        _match = doMatch(_begin);
        if (_match) {
            _begin = _match + pattern_.second;
            ret.push_back(_match);
        }
    } while (_match != NULL);

    return ret;
}

template<class Elm>
Elm* KMP<Elm>::doMatch(const Elm* begin)
{
    if (begin > source_.first + source_.second) {
        return nullptr;
    }

    int is = 0, ip = 0;
    bool hit = false;
    int* c_next = next_.get();
    const Elm* c_pattern = pattern_.first;

    for (; is < source_.second; )
    {
        // printf("\n%d, %d", is, ip);
        if (begin[is] == c_pattern[ip])
        {
            if (ip == pattern_.second - 1) {
                next_begin_ = const_cast<Elm*>(begin + is + 1);
                hit = true;
                break;
            }
            else {
                is++, ip++;
            }
        }
        else
        {
            if (c_next[ip] < 0) {
                is++;
                ip = 0;
            }
            else {
                ip = c_next[ip];
            }
        }
    }

    return hit ? const_cast<Elm*>(begin + is - pattern_.second + 1) : nullptr;
}











#endif // _KMP_ALGORITHM_H_
