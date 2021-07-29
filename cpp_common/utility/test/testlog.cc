

#include "logger.h"


int main()
{
	char a[256] = "sd我是暗色调的偶尔怕热人";

	// FILE* fp = fopen("D:\\CrashPad_Android.vsdx", "rb");
	// fread(a, 1, 256, fp);
	// fclose(fp);

	LOG_InitAsConsoleMode("logtest", "1.0", "./");

	LOGM_Debug("char array a", a, sizeof(a));
	LOG_Infor("这是分级日志的Infor日志");
	LOG_Warning("这是分级日志的Warning日志");
	LOG_Fatal("这是分级日志的fatal日志");
	LOG_Error("这是分级日志的Error日志");
	LOG_FREE(1, TC_Red, "一级红色日志");
	LOG_FREE(2, TC_Cyan, "二级青色日志");
	LOG_FREE(3, TC_Yellow, "三级黄色日志");
	LOG_FREE(4, TC_Purple, "四级紫色日志");
	LOG_FREE(5, TC_Green, "五级绿色日志");
	LOG_FREE(3, TC_Blue, "三级蓝色日志");
	
	return 0;
}