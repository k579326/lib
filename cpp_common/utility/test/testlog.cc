

#include "logger.h"


int main()
{
	char a[256] = "sd���ǰ�ɫ����ż��������";

	// FILE* fp = fopen("D:\\CrashPad_Android.vsdx", "rb");
	// fread(a, 1, 256, fp);
	// fclose(fp);

	LOG_InitAsConsoleMode("logtest", "1.0", "./");

	LOGM_Debug("char array a", a, sizeof(a));
	LOG_Infor("���Ƿּ���־��Infor��־");
	LOG_Warning("���Ƿּ���־��Warning��־");
	LOG_Fatal("���Ƿּ���־��fatal��־");
	LOG_Error("���Ƿּ���־��Error��־");
	LOG_FREE(1, TC_Red, "һ����ɫ��־");
	LOG_FREE(2, TC_Cyan, "������ɫ��־");
	LOG_FREE(3, TC_Yellow, "������ɫ��־");
	LOG_FREE(4, TC_Purple, "�ļ���ɫ��־");
	LOG_FREE(5, TC_Green, "�弶��ɫ��־");
	LOG_FREE(3, TC_Blue, "������ɫ��־");
	
	return 0;
}