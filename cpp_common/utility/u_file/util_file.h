#ifndef _UTIL_FILE_H_
#define _UTIL_FILE_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>


namespace fileutil
{

    class File
    {
    public:
        File();
        ~File();
    
        File(const char* path);     /* can not be null */

        File(const File& f) = delete;
        File(File&& f) = delete;
        File& operator=(const File& f) = delete;
        File& operator=(const File&& f) = delete;

    public:     /* 打开状态下方法 */
        /**
        * notice:
        *   1. 如果当前打开的路径的父目录不存在，则不允许文件打开。该类不负责创建目录，使用者要自己确保操作的文件的目录已存在；
        *   2. 不接受相对路径，如果类构造时使用了相对路径，Open方法会失败。
        */
        int Open(const std::string& mode);
        int Close();
        int Seek(int32_t off, int where);
        int Read(std::string* out, uint32_t expected_size);

        /* 仅用于处理文本文件, 如果成功返回正整数，读到文件尾返回0，失败返回-1*/
        int ReadLine(std::string* out);
        bool IsEOF();

        int Write(const unsigned char* indata, uint32_t insize);
        
        int Flush();


    public: /* 关闭状态下可调用方法 */

        /* 重新设置文件路径，调用该方法后，文件对象处于关闭状态 */
        void Reset(const char* path);
        std::string Name() const;
        bool IsOpened(void) const;


    public: /* 静态方法 */
        static uint64_t Size(const std::string& filepath);
        static int Remove(const std::string& filepath);
        static int Copy(const std::string& srcpath, const std::string& dstpath, bool overlay);
        static int Rename(const std::string& oldname, const std::string& newname);

        /* @function 快速设置文件大小（扩大容量或者截断文件，扩大的空间的内容随机）
           @param filename  文件名
           @param length    期望的文件大小
        */
        static bool Truncate(const std::string& filename, uint32_t length);
        static bool IsExist(const std::string& path);

    private:

        std::string path_;
        FILE* file_;
    };

};










#endif

