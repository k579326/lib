#include <stdlib.h>
#include <sys/ioctl.h>      //net api need
#include <sys/socket.h>     //net api need
#include <netinet/in.h>     //net api need
#include <net/if.h>         //net api need
#include <arpa/inet.h>      //net api need
#include <sys/stat.h>
#include <dlfcn.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "other-tmp.h"
#include "log.h"


static pthread_mutexattr_t attr;

int mp_init_lock(MP_LOCK_OBJ* plock)
{
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    
    return pthread_mutex_init(plock, &attr);
}

int mp_del_lock(MP_LOCK_OBJ* plock)
{
    pthread_mutexattr_destroy(&attr);
    return pthread_mutex_destroy(plock);
}

int mp_enter_lock(MP_LOCK_OBJ* plock)
{
    return pthread_mutex_lock(plock);
}

int mp_leave_lock(MP_LOCK_OBJ* plock)
{
    return pthread_mutex_unlock(plock);
}

int ssmp_rand(void* data, int size)
{
    int remain = 0;
    int each_num;
    char* p = NULL;
	int i;
	unsigned int seed;
    
#ifdef __APPLE__
#include <sys/time.h>
    struct timeval tv;
    gettimeofday(&tv, NULL);
    seed = tv.tv_sec + rand() % 10000;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    // seed = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;  可能超出无符号整数seed最大范围，放弃
    seed = ts.tv_sec + rand() % 10000;
#endif
    
    srand(seed);
    remain = size % sizeof(int);

    p = (char*)data;
    for (i = 0; i < size / sizeof(int); i++)
    {
        each_num = rand();
        memcpy((void*)p, &each_num, sizeof(each_num));
        p += sizeof(each_num);
    }

    if (remain != 0)
    {
        each_num = rand();
        memcpy(p, &each_num, remain);
    }
	usleep(10000);
    return 0;
}


int mp_getlocalmac(char* mac_string)
{
/*    struct   ifreq   ifreq; 
    int   sock;
    bool bvir_dev = false;
    char mac[64];
    int ret = 0;
	int i;
    for (i = 1; ; i++) 
    {	
        char if_name[128];
        char* pret = if_indextoname(i, if_name);
        
        if (pret == NULL)
        {
            bvir_dev = false;
            break;
        }
        
        sock = socket(AF_INET, SOCK_STREAM, 0);
        strcpy(ifreq.ifr_name, if_name); 
        ret = ioctl(sock, SIOCGIFHWADDR, &ifreq);
        if (ret < 0) 
        { 
            bvir_dev = false;
            break;
        } 
        
        sprintf(mac, "%02x-%02x-%02x-%02x-%02x-%02x", 
                            (unsigned char)ifreq.ifr_hwaddr.sa_data[0], 
                            (unsigned char)ifreq.ifr_hwaddr.sa_data[1], 
                            (unsigned char)ifreq.ifr_hwaddr.sa_data[2], 
                            (unsigned char)ifreq.ifr_hwaddr.sa_data[3], 
                            (unsigned char)ifreq.ifr_hwaddr.sa_data[4], 
                            (unsigned char)ifreq.ifr_hwaddr.sa_data[5]    ); 
                
        strcpy(mac_string, mac);
        if (strcmp(mac_string, "00-00-00-00-00-00") == 0)
        {
            bvir_dev = false;
            continue;
        }      
    }

    

    return bvir_dev ? 0 : -1;*/
    return 0;
}


char* mp_utf8_to_ansi(const char* str_utf8)
{
	char* temp = malloc(strlen(str_utf8) + 1);
    if(NULL == temp)
    {
        return NULL;
    }
	strcpy(temp, str_utf8);
    return temp;
}

char* mp_ansi_to_utf8(const char* str_ansi)
{
	char* temp = malloc(strlen(str_ansi) + 1);
    if(NULL == temp)
    {
        return NULL;
    }
	strcpy(temp, str_ansi);
    return temp;
}

wchar_t* mp_utf8_to_widechar(const char* utf8)
{// 暂时不使用
    assert(0);
    return NULL;
}

char* mp_widechar_to_utf8(const wchar_t* widechar)
{// 暂时不使用
    assert(0);
    return NULL;
}

wchar_t* mp_ansi_to_widechar(const char* ansi)
{// 暂时不使用
    assert(0);
    return NULL;
}

char* mp_widechar_to_ansi(wchar_t* widechar)
{// 暂时不使用
    assert(0);
    return NULL;
}

