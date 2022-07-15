
#include "console_extern.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <map>

#define unused(x) ((void*)&x)

static std::string s_color = "\e[0m";

static const std::map<__LogTextColor, std::string> color_map = 
{
    {Black, "\e[%d;30m"},
    {Red,   "\e[%d;31m"},
    {Green, "\e[%d;32m"},
    {Blue,  "\e[%d;34m"},
    {Yellow,"\e[%d;33m"},
    {Purple,"\e[%d;35m"},
    {White, "\e[%d;37m"},
    {Cyan,  "\e[%d;36m"}
};




// 总是使用STDOUT_FILENO
ConsoleID GetConsole()
{
    // 只要不返回-1就可以，并不使用
    return 0;
}

bool SetConsoleTextColor(ConsoleID console, __LogTextColor color, bool intensity)
{
    unused(console);
    
    auto it = color_map.find(color);
    if (it == color_map.end()) {
        return false;
    }
    
    char buf[64] = { 0 };
    
    sprintf(buf, it->second.c_str(), intensity ? 1 : 0);
    s_color = buf;
    
    return true;
}

int WriteConsole_(ConsoleID console, const void* ptr, int len)
{
    unused(console);
    
    const char* term = getenv("TERM");
    
    bool term_supported = term != nullptr &&
        (strcmp(term, "xterm") == 0 ||
        strcmp(term, "xterm-color") == 0 ||
        strcmp(term, "xterm-256color") == 0 ||
        strcmp(term, "linux") == 0 ||
        strcmp(term, "cygwin") == 0);
    
    if (1 != isatty(fileno(stdout)) || !term_supported)
    {
        write(STDOUT_FILENO, ptr, len);
        return 0;
    }
    
    char buf[1024 * 4];
    sprintf(buf, "%s%s\e[0m", s_color.c_str(), (const char*)ptr);
    
    write(STDOUT_FILENO, s_color.c_str(), s_color.size());
    write(STDOUT_FILENO, ptr, len);
    write(STDOUT_FILENO, "\e[0m", strlen("\e[0m"));
    
    return 0;
}

void RestoreConsole(ConsoleID console)
{
    unused(console);
}


