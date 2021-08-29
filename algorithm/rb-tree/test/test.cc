
#include "rb_tree.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <chrono>
#include <map>
#include <string>

struct MyPair
{
    int key;
    char value[10];
};

bool less(const MyPair* p1, const MyPair* p2)
{
    return p1->key < p2->key;
}

static int64_t GetNow()
{
    auto starttime = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(starttime.time_since_epoch()).count();
}


int main()
{
    RbTree* map = CreateRbTree(sizeof(MyPair), (TypeLess)less);
    
    MyPair pair;
    int64_t usetime;
    usetime = GetNow();
    for (int i = 0; i < 10; i++)
    {
        pair.key = i;
        InsertNode(map, &pair);
    }
    usetime = GetNow() - usetime;
    printf("rbtree insert 10000 times use time %lld \n", usetime);

    WalkTreeAsLevel(map);
    
    for (int i = 0; i < 10; i++)
    {
        pair.key = i;
        DeleteNode(map, &pair);

        WalkTreeAsLevel(map);
    }
    WalkTreeAsLevel(map);

    //usetime = GetNow();
    //for (int i = 0; i < 10000000; i++)
    //{
    //    pair.key = i;
    //    Find(map, &pair);
    //}
    //usetime = GetNow() - usetime;
    //printf("rbtree search 10000 times use time %llu \n", usetime);
    WalkTreeAsLevel(map);

    // usetime = GetNow();
    // struct intless {
    //     constexpr bool operator ()(const int& a, const int& b) const
    //     {
    //         return a < b;
    //     }
    // };
    // 
    // std::map<int, std::string, intless> testmap;
    // for (int i = 0; i < 10000000; i++)
    // {
    //     testmap.insert(std::make_pair(i, "123456789"));
    // }
    // usetime = GetNow() - usetime;
    // printf("map insert 10000 times use time %llu \n", usetime);
    // 
    // usetime = GetNow();
    // for (int i = 0; i < 10000000; i++)
    // {
    //     testmap.find(i);
    // }
    // 
    // usetime = GetNow() - usetime;
    // printf("map search 10000 times use time %llu \n", usetime);



    while (true)
    {

        
    }
    

    return 0;
}

