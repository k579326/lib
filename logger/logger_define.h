
#ifndef _LOGGER_DEFINE_H_
#define _LOGGER_DEFINE_H_


// С�ڵ�ǰ��־�ȼ��Ĵ�ӡ�����������־
enum LogLevels
{
	kDebugLevel	= 0,	    // ������־,
	kInforLevel,			// ��ͨ
	kWarningLevel,			// ������Ϣ
	kErrorLevel,			// ������Ӱ�����������е���Ϣ
	kFatalLevel,			// ���³����쳣��ֹ�Ĵ�����Ϣ
};


enum RunModel
{
    kSync,
    kAsync,
};


enum LogColumns
{
    kDateTime = 0x01,        // ȫ�ȼ���Ч
    kThreadId = 0x02,        // ȫ�ȼ���Ч
    kLogLevel = 0x04,        // ȫ�ȼ���Ч
    kFileName = 0x08,        // ����kErrorLevel�ȼ���Ч
    kLineNum = kFileName,    // ��kFileName����
    kFunction = 0x20,        // ����kFatalLevel�ȼ���Ч
    kErrorNum = 0x40,        // ȫ�ȼ���Ч
    kVersion = 0x80          // ȫ�ȼ���Ч
};

enum LogOutput
{
    kFileModel = 0,
    kConsoleModel
};


#endif

