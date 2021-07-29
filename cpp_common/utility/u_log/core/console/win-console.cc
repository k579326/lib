
#include "console_extern.h"

static int s_default_color = -1;
static BOOL s_alloced = FALSE;


typedef struct 
{
    __LogTextColor   value;
    int             wcolor;
}_ColorPair;

const static _ColorPair cs_colormap[] = 
{
    {Black, 0},
    {Red,   FOREGROUND_RED},
    {Green, FOREGROUND_GREEN},
    {Blue,  FOREGROUND_BLUE},
    {Yellow,FOREGROUND_RED | FOREGROUND_GREEN},
    {Purple,FOREGROUND_RED | FOREGROUND_BLUE},
    {White, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE},
    {Cyan,  FOREGROUND_GREEN | FOREGROUND_BLUE}
};


ConsoleID GetConsole()
{
    ConsoleID cid = GetStdHandle(STD_OUTPUT_HANDLE);
    if (cid == INVALID_HANDLE_VALUE) {
        s_alloced = AllocConsole();
        cid = GetStdHandle(STD_OUTPUT_HANDLE);
    }
    
    if (cid == INVALID_HANDLE_VALUE && s_alloced)
    {
        FreeConsole();
        s_alloced = FALSE;
    }
    
    return cid;
}

bool SetConsoleTextColor(ConsoleID console, __LogTextColor color, bool intensity)
{
    int i = 0;
    WORD color_attr;
    WORD old_attr;
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    
    if (console == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    for (i = 0; i < sizeof(cs_colormap) / sizeof(_ColorPair); i++)
    {
        if (cs_colormap[i].value == color) {
            color_attr = cs_colormap[i].wcolor;
            break;
        }    
    }
    
    if (i == sizeof(cs_colormap) / sizeof(_ColorPair)) {
        return false;
    }
    
    GetConsoleScreenBufferInfo(console, &csbiInfo);
    old_attr = csbiInfo.wAttributes;
    
    if (s_default_color == (int)InvalidColor)
    {
        s_default_color = old_attr;
    }
    
    if (intensity)
        color_attr |= FOREGROUND_INTENSITY;
    
    old_attr = old_attr & ~(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

    SetConsoleTextAttribute(console, color_attr | old_attr);
    return true;
}

int WriteConsole_(ConsoleID console, const void* ptr, int len)
{
    DWORD dwSizeWriten = 0;
    if (WriteConsoleA(console, ptr, len, &dwSizeWriten, NULL) == FALSE)
    {
        return -1;
    }
    return 0;
}

void RestoreConsole(ConsoleID console)
{
    SetConsoleTextAttribute(console, s_default_color);
}



