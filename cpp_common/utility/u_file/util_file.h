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

    public:     /* ��״̬�·��� */
        /**
        * notice:
        *   1. �����ǰ�򿪵�·���ĸ�Ŀ¼�����ڣ��������ļ��򿪡����಻���𴴽�Ŀ¼��ʹ����Ҫ�Լ�ȷ���������ļ���Ŀ¼�Ѵ��ڣ�
        *   2. ���������·��������๹��ʱʹ�������·����Open������ʧ�ܡ�
        */
        int Open(const std::string& mode);
        int Close();
        int Seek(int32_t off, int where);
        int Read(std::string* out, uint32_t expected_size);

        /* �����ڴ����ı��ļ� */
        int ReadLine(std::string* out);
        bool IsEOF();

        int Write(const unsigned char* indata, uint32_t insize);
        
        int Flush();


    public: /* �ر�״̬�¿ɵ��÷��� */

        /* ���������ļ�·�������ø÷������ļ������ڹر�״̬ */
        void Reset(const char* path);
        std::string Name() const;
        bool IsOpened(void) const;


    public: /* ��̬���� */
        static uint32_t Size(const std::string& filepath);
        static int Remove(const std::string& filepath);
        static int Copy(const std::string& srcpath, const std::string& dstpath, bool overlay);
        static int Rename(const std::string& oldname, const std::string& newname);

        /* @function ���������ļ���С�������������߽ض��ļ�������Ŀռ�����������
           @param filename  �ļ���
           @param length    �������ļ���С
        */
        static bool Truncate(const std::string& filename, uint32_t length);
        static bool IsExist(const std::string& path);

    private:

        std::string path_;
        FILE* file_;
    };

};










#endif

