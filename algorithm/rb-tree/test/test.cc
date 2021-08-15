
#include "rb_tree.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <chrono>
#include <map>

struct MyPair
{
    int key;
    char value[10];
};

bool less(const MyPair* p1, const MyPair* p2)
{
    return p1->key < p2->key;
}


int main()
{
    RbTree* map = CreateRbTree(sizeof(MyPair), (TypeLess)less);

    MyPair pair;

    auto starttime = std::chrono::steady_clock::now();
    for (int i = 0; i < 1000000; i++)
    {
        pair.key = rand();
        Insert(map, &pair);
    }
    auto endtime = std::chrono::steady_clock::now();
    uint64_t usetime = std::chrono::duration_cast<std::chrono::microseconds>(endtime.time_since_epoch()).count() -
        std::chrono::duration_cast<std::chrono::microseconds>(starttime.time_since_epoch()).count();
    printf("rbtree insert 10000 times use time %llu \n", usetime);


    starttime = std::chrono::steady_clock::now();
    for (int i = 0; i < 1000000; i++)
    {
        pair.key = i;
        Find(map, &pair);
    }
    endtime = std::chrono::steady_clock::now();
    usetime = std::chrono::duration_cast<std::chrono::microseconds>(endtime.time_since_epoch()).count() -
        std::chrono::duration_cast<std::chrono::microseconds>(starttime.time_since_epoch()).count();
    printf("rbtree search 10000 times use time %llu \n", usetime);
    //WalkTreeAsLevel(map);

    starttime = std::chrono::steady_clock::now();
    struct intless {
        constexpr bool operator ()(const int& a, const int& b) const
        {
            return a < b;
        }
    };

    std::map<int, void*, intless> testmap;
    for (int i = 0; i < 1000000; i++)
    {
        testmap.insert(std::make_pair(rand(), (void*)i));
    }
    endtime = std::chrono::steady_clock::now();
    usetime = std::chrono::duration_cast<std::chrono::microseconds>(endtime.time_since_epoch()).count() -
        std::chrono::duration_cast<std::chrono::microseconds>(starttime.time_since_epoch()).count();
    printf("map insert 10000 times use time %llu \n", usetime);

    starttime = std::chrono::steady_clock::now();
    for (int i = 0; i < 1000000; i++)
    {
        testmap.find(i);
    }

    endtime = std::chrono::steady_clock::now();
    usetime = std::chrono::duration_cast<std::chrono::microseconds>(endtime.time_since_epoch()).count() -
        std::chrono::duration_cast<std::chrono::microseconds>(starttime.time_since_epoch()).count();
    printf("map search 10000 times use time %llu \n", usetime);



    while (true)
    {

        
    }
    

    return 0;
}

