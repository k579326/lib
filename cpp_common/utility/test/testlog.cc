

#include "logger.h"


int main()
{
	CptChar a[256] = TEXT("sd���ǰ�ɫ����ż��������");

	// FILE* fp = fopen("D:\\CrashPad_Android.vsdx", "rb");
	// fread(a, 1, 256, fp);
	// fclose(fp);

	//LOG_InitAsConsoleMode(TEXT("logtest"), TEXT("1.0"), TEXT("./"));
	LOG_InitAsFileMode(TEXT("logtest"), TEXT("1.0"), TEXT("./"));

	LOGM_Debug(TEXT("char array a"), a, sizeof(a));
	LOG_Infor(TEXT("���Ƿּ���־��Infor��־"));
	LOG_Warning(TEXT("���Ƿּ���־��Warning��־"));
	LOG_Fatal(TEXT("���Ƿּ���־��fatal��־"));
	LOG_Error(TEXT("���Ƿּ���־��Error��־"));
	LOG_FREE(1, TC_Red, TEXT("һ����ɫ��־"));
	LOG_FREE(2, TC_Cyan, TEXT("������ɫ��־"));
	LOG_FREE(3, TC_Yellow, TEXT("������ɫ��־"));
	LOG_FREE(4, TC_Purple, TEXT("�ļ���ɫ��־"));
	LOG_FREE(5, TC_Green, TEXT("�弶��ɫ��־"));
	LOG_FREE(3, TC_Blue, TEXT("������ɫ��־"));

	// LOG_Uninit();

	getchar();

	return 0;
}