
#pragma once


#ifdef PF_OS_WIN
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
	#define MP_LOCK_OBJ CRITICAL_SECTION
#else
	#include <pthread.h>
	#define MP_LOCK_OBJ pthread_mutex_t
	#define MAX_PATH PATH_MAX
#endif

#ifdef __cplusplus
extern "C" {
#endif
	int mp_init_lock(MP_LOCK_OBJ* obj);
	int mp_del_lock(MP_LOCK_OBJ* obj);
	int mp_enter_lock(MP_LOCK_OBJ* obj);
	int mp_leave_lock(MP_LOCK_OBJ* obj);

	int mp_getlocalmac(char* mac);

	//生成size个字节的随机数
	int ssmp_rand(void* data, int size);

	char* mp_utf8_to_ansi(const char* utf8);
	char* mp_ansi_to_utf8(const char* ansi);
    wchar_t* mp_utf8_to_widechar(const char* utf8);
    char* mp_widechar_to_utf8(const wchar_t* widechar);

    wchar_t* mp_ansi_to_widechar(const char* ansi);
    char* mp_widechar_to_ansi(wchar_t* widechar);

#ifdef __cplusplus
}
#endif

