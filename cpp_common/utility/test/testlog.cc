#include "logger.h"


int main()
{
    CptChar a[256] = TEXT("sd我是暗色调的偶尔怕热人");

    // FILE* fp = fopen("D:\\CrashPad_Android.vsdx", "rb");
    // fread(a, 1, 256, fp);
    // fclose(fp);

    LOG_InitAsConsoleMode(TEXT("logtest"), TEXT("1.0"), TEXT("./"));
    //LOG_InitAsFileMode(TEXT("logtest"), TEXT("1.0"), TEXT("./"));

    LOGM_Debug(TEXT("char array a"), a, sizeof(a));
    LOG_Infor(TEXT("这是分级日志的Infor日志"));
    LOG_Warning(TEXT("这是分级日志的Warning日志"));
    LOG_Fatal(TEXT("这是分级日志的fatal日志"));
    LOG_Error(TEXT("这是分级日志的Error日志"));
    LOG_FREE(1, TC_Red, TEXT("一级红色日志"));
    LOG_FREE(2, TC_Cyan, TEXT("二级青色日志"));
    LOG_FREE(3, TC_Yellow, TEXT("三级黄色日志"));
    LOG_FREE(4, TC_Purple, TEXT("四级紫色日志"));
    LOG_FREE(5, TC_Green, TEXT("五级绿色日志"));
    LOG_FREE(3, TC_Blue, TEXT("三级蓝色日志"));

    // LOG_Uninit();

    getchar();

    return 0;
}
