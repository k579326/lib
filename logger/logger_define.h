
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
    kDateTime = 0x01,
    kThreadId = 0x02,
    kLogLevel = 0x04,
    kFileName = 0x08,
    kLineNum = 0x10,
    kErrorNum = 0x20,
    kVersion = 0x40
};

enum LogOutput
{
    kFileModel = 0,
    kConsoleModel
};


#endif

