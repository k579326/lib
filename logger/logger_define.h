
#ifndef _LOGGER_DEFINE_H_
#define _LOGGER_DEFINE_H_


// 小于当前日志等级的打印不会输出到日志
enum LogLevels
{
	kDebugLevel	= 0,	    // 调试日志,
	kInforLevel,			// 普通
	kWarningLevel,			// 警告信息
	kErrorLevel,			// 出错但不影响程序继续运行的信息
	kFatalLevel,			// 导致程序异常中止的错误信息
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

