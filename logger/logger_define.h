
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

    kLevelEnd               // 辅助，不可使用
};


enum RunModel
{
    kSync,                   // 同步输出日志
    kAsync,                  // 异步输出日志
};


enum LogColumns
{
    kDateTime = 0x01,        // 全等级有效
    kThreadId = 0x02,        // 全等级有效
    kLogLevel = 0x04,        // 全等级有效
    kFileInfo = 0x08,        // 低于kErrorLevel等级有效
    kFunction = 0x20,        // 低于kFatalLevel等级有效
    kVersion = 0x80,          // 全等级有效

    kAllColumn = kDateTime | kThreadId | kLogLevel | kFileInfo | kFunction | kVersion,
};

enum LogOutput
{
    kFileModel = 0,
    kConsoleModel
};


#endif

