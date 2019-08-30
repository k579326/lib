#ifndef _UTIL_FILE_H_
#define _UTIL_FILE_H_


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
    
        File(const char* path);

        File(const File& f) = delete;
        File(File&& f) = delete;
        File& operator=(const File& f) = delete;
        File& operator=(const File&& f) = delete;

        /**
        * notice:
        *   1. �����ǰ�򿪵�·���ĸ�Ŀ¼�����ڣ��������ļ��򿪡����಻���𴴽�Ŀ¼��ʹ����Ҫ�Լ�ȷ���������ļ���Ŀ¼�Ѵ��ڣ�
        *   2. ���������·��������๹��ʱʹ�������·����Open������ʧ�ܡ�
        */
        int Open(const std::string& mode);
        int Close();

        /* ���������ļ�·�������ø÷������ļ������ڹر�״̬ */
        void Reset(const char* path);
        
        int Read(std::string* out, uint32_t expected_size);
        int ReadLine(std::string* out, uint32_t expected_size);
        bool IsEOF();

        int Write(const unsigned char* indata, uint32_t& insize);
        
        int Flush();

        std::string Name() const;
        bool IsOpened(void) const;

        static uint32_t Size(const std::string& filepath);
        static int Remove(const std::string& filepath);
        static int Copy(const std::string& srcpath, const std::string& dstpath, bool overlay);
        static int Rename(const std::string& oldname, const std::string& newname);
        static bool Truncate(const std::string& filename, uint32_t length);
        static bool IsExist(const std::string& path);

    private:

        std::string path_;
        FILE* file_;
    };

};










#endif

