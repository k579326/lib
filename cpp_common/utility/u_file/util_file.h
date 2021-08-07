#ifndef _UTIL_FILE_H_
#define _UTIL_FILE_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>

// linux 32位程序需要此宏来操作大文件
// 64位程序不需要此宏
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif

namespace fileutil
{

    enum OpenModel {
        // 创建新文件，已存在则失败，成功时文件大小为0
        kCreate,

        // 强制创建，覆盖已存在文件，成功时文件大小为0
        kCreateForce,

        // 打开已存在文件，不存在则失败，打开成功后，文件指针初始偏移为0
        kOpen,

        // 打开文件，不存在则创建，打开成功后，文件指针初始偏移为0
        kOpenForce,

        // 追加模式打开，不存在会失败。写入操作总是在文件结尾，不受Seek影响
        // 读取操作遵从文件指针位置返回结果
        kAppend,

        // 追加模式，不存在则创建。
        // 如果AccessModel为kReadOnly，打开失败。
        // 其他与kAppend相同
        kAppendForce,

    };
    enum AccessModel {
        kReadOnly,
        kWriteOnly,
        kRdWr,
    };
    enum SeekPosition
    {
        kSeekSet,
        kSeekCur,
        kSeekEnd
    };

    class File
    {
    public:
        File();
        ~File();
    
        /* 可接受相对路径，基于getcwd，如果用于非命令行程序可能出错。*/
        File(const char* path);     /* can not be null */

        File(const File& f) = delete;
        File(File&& f) = delete;
        File& operator=(const File& f) = delete;
        File& operator=(File&& f) = delete;

    public:     /* 打开状态下方法 */
        /**
        * notice:
        *   1. 如果当前打开的路径的父目录不存在，则不允许文件打开。
        *       该类不负责创建目录
        *   2. permission 在windows下无效
        */
        int Open(OpenModel om, AccessModel am, int permission = 0755);
        void Close();
        int Seek(int64_t off, SeekPosition where) const;
        int Read(std::string* out, uint32_t expected_size) const;

        /* 仅用于处理文本文件, 如果成功返回正整数，读到文件尾返回0，失败返回-1*/
        int ReadLine(std::string* out) const;

        int Write(const void* indata, uint32_t insize) const;
        
        void Flush();


    public: /* 关闭状态下可调用方法 */

        /* 重新设置文件路径，调用该方法后，文件对象处于关闭状态 */
        void Reset(const char* path);
        std::string Name() const;
        bool IsOpened(void) const;


    public: /* 静态方法 */
        static uint64_t Size(const std::string& filepath);
        static int Remove(const std::string& filepath);
        /* overly：dstpath存在的情况下， 如果此值true, 已存在的文件会被删除 */
        static int Copy(const std::string& srcpath, const std::string& dstpath, bool overlay);
        static int Rename(const std::string& oldname, const std::string& newname);

        static bool IsNormalFile(const std::string& filename);

        /* @function 快速设置文件大小（扩大容量或者截断文件，扩大的空间的内容随机）
           @param filename  文件名，必须已存在
           @param length    期望的文件大小
        */
        static int Truncate(const std::string& filename, uint64_t length);
        static bool IsExist(const std::string& path);

    private:
        class FilePtr;
        std::string path_;
        FilePtr* file_;
    };

};










#endif

