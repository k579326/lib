
#ifndef _UTIL_DIR_EXT_H_
#define _UTIL_DIR_EXT_H_


#include "util_dir.h"

namespace dirutil
{
    class Dir
    {
    public:  
        Dir() {}
        ~Dir() {}
        
        Dir(const Dir& dir) = delete;
        Dir(Dir&& dir) = delete;
        Dir& operator=(const Dir& dir) = delete;
        Dir& operator=(Dir&& dir) = delete;
        
        int Open()
        {
            
        }
        
    };
    
    
    
};





#endif // _UTIL_DIR_EXT_H_
