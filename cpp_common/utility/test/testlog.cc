

#include "logger.h"


int main()
{
	char a[256] = "sd我是暗色调的偶尔怕热人";

	// FILE* fp = fopen("D:\\CrashPad_Android.vsdx", "rb");
	// fread(a, 1, 256, fp);
	// fclose(fp);

	LOG_InitAsConsoleMode("logtest", "1.0", "./");

	LOGM_Debug("char array a", a, sizeof(a));
	
	return 0;
}