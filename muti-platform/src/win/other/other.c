
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Iphlpapi.h>   // net api
#include <io.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "other-tmp.h"
#include "da_error.h"

int mp_init_lock(MP_LOCK_OBJ* plock)
{
    InitializeCriticalSection(plock);
    return 0;
}

int mp_del_lock(MP_LOCK_OBJ* plock)
{
    DeleteCriticalSection(plock);
    return 0;
}

int mp_enter_lock(MP_LOCK_OBJ* plock)
{
    EnterCriticalSection(plock);
    return 0;
}

int mp_leave_lock(MP_LOCK_OBJ* plock)
{
    LeaveCriticalSection(plock);
    return 0;
}


int ssmp_rand(void* data, int size)
{
    int remain = 0;
    short each_num;
    char* p = NULL;
	int i;
	
    srand(GetTickCount());
    remain = size % sizeof(short);

    p = (char*)data;
    for (i = 0; i < size / sizeof(short); i++)
    {
        each_num = rand();
        memcpy(p, &each_num, sizeof(each_num));
        p += sizeof(each_num);
    }

    if (remain != 0)
    {
        each_num = rand();
        memcpy(p, &each_num, remain);
    }

	Sleep(10);
    return 0;
}


int mp_getlocalmac(char* mac)
{
    int bRet = -1;
    unsigned long uLen = sizeof(IP_ADAPTER_INFO);
    PIP_ADAPTER_INFO pAdapterInfo = NULL;

	strcpy(mac, "");
    do
    {
        pAdapterInfo = (IP_ADAPTER_INFO*)malloc(uLen);
        if (pAdapterInfo == NULL)
        {
            break;
        }

        unsigned int uRet = GetAdaptersInfo(pAdapterInfo, &uLen);
        if (uRet == ERROR_BUFFER_OVERFLOW)
        {
            free(pAdapterInfo);
            pAdapterInfo = NULL;

            pAdapterInfo = (IP_ADAPTER_INFO*)malloc(uLen);
            if (pAdapterInfo == NULL)
            {
                break;
            }

            uRet = GetAdaptersInfo(pAdapterInfo, &uLen);
            if (uRet != NO_ERROR)
            {
                break;
            }
        }
        else if (uRet != NO_ERROR)
        {
            break;
        }

        char szMac[MAX_PATH] = { 0 };
		int i;
        for (i = 0; i < pAdapterInfo->AddressLength; i++)
        {
            if (i == (pAdapterInfo->AddressLength - 1))
            {
                sprintf(szMac, _countof(szMac), "%.2X", (int)pAdapterInfo->Address[i]);
                strcat(mac, szMac);
            }
            else
            {
                sprintf(szMac, _countof(szMac), "%.2X-", (int)pAdapterInfo->Address[i]);
				strcat(mac, szMac);
            }
        }

        bRet = 0;

    } while (false);

    if (pAdapterInfo != NULL)
    {
        free(pAdapterInfo);
        pAdapterInfo = NULL;
    }

    return bRet;
}

char* mp_ansi_to_utf8(const char* str_ansi)
{
     int wlen = 0;
     wchar_t* wbuf = 0;
     char* utf8 = 0;
 
     // ANSI -> UNICODE
     wlen = MultiByteToWideChar(CP_ACP, 0, str_ansi, -1, NULL, 0);
     wbuf = (wchar_t*)malloc((wlen + 1)*sizeof(wchar_t));
     if(NULL == wbuf)
     {
        return NULL;
     }
     MultiByteToWideChar(CP_ACP, 0, str_ansi, -1, wbuf, wlen);
     wbuf[wlen] = 0;
 
     // UNICODE -> UTF8
     int ulen = WideCharToMultiByte(CP_UTF8, 0, wbuf, wlen, NULL, 0, NULL, NULL);
     utf8 = (char*)malloc((ulen + 1)*sizeof(char));
     if(NULL == utf8)
     {
        return NULL;
     }
     WideCharToMultiByte(CP_UTF8, 0, wbuf, wlen, utf8, ulen, NULL, NULL);
     utf8[ulen] = 0;
	 
     free(wbuf);
	 
     return utf8;
}

char* mp_utf8_to_ansi(const char* str_utf8)
{
     int wlen = 0;
     wchar_t* wbuf = 0;
     char* ansi = 0;
 
     // str_utf8 -> UNICODE
     wlen = MultiByteToWideChar(CP_UTF8, 0, str_utf8, -1, NULL, 0);
     wbuf = (wchar_t*)malloc((wlen + 1) * sizeof(wchar_t));
     if(NULL == wbuf)
     {
        return NULL;
     }
     MultiByteToWideChar(CP_UTF8, 0, str_utf8, -1, wbuf, wlen);
     wbuf[wlen] = 0;
 
     // UNICODE -> ansi
     int alen = WideCharToMultiByte(CP_ACP, 0, wbuf, wlen, NULL, 0, NULL, NULL);
     ansi = (char*)malloc((alen + 1) * sizeof(char));
     if(NULL == ansi)
     {
        return NULL;
     }
     WideCharToMultiByte(CP_ACP, 0, wbuf, wlen, ansi, alen, NULL, NULL);
     ansi[alen] = 0;
 
     free(wbuf);
     return ansi;
}

wchar_t* mp_utf8_to_widechar(const char* utf8)
{
    int wlen = 0;
    wchar_t* wbuf = 0;

    // str_utf8 -> UNICODE
    wlen = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    wbuf = (wchar_t*)malloc((wlen + 1) * sizeof(wchar_t));
    if (NULL == wbuf)
    {
        return NULL;
    }
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wbuf, wlen);
    wbuf[wlen] = 0;

    return wbuf;
}

char* mp_widechar_to_utf8(const wchar_t* widechar)
{
    int wlen = 0;
    char* utf8 = NULL;
    // UNICODE -> UTF8
    int ulen = WideCharToMultiByte(CP_UTF8, 0, widechar, -1, NULL, 0, NULL, NULL);
    utf8 = (char*)malloc((ulen + 1)*sizeof(char));
    if (NULL == utf8)
    {
        return NULL;
    }
    WideCharToMultiByte(CP_UTF8, 0, widechar, -1, utf8, ulen, NULL, NULL);
    utf8[ulen] = 0;

    return utf8;
}

wchar_t* mp_ansi_to_widechar(const char* ansi)
{
    int wlen = 0;
    wchar_t* wbuf = 0;
    char* utf8 = 0;

    // ANSI -> UNICODE
    wlen = MultiByteToWideChar(CP_ACP, 0, ansi, -1, NULL, 0);
    wbuf = (wchar_t*)malloc((wlen + 1)*sizeof(wchar_t));
   
    MultiByteToWideChar(CP_ACP, 0, ansi, -1, wbuf, wlen);
    wbuf[wlen] = 0;

    return wbuf;
}

char* mp_widechar_to_ansi(wchar_t* widechar)
{
    char* ansi = NULL;
    int wlen = 0;
    // UNICODE -> ansi
    int alen = WideCharToMultiByte(CP_ACP, 0, widechar, -1, NULL, 0, NULL, NULL);
    ansi = (char*)malloc((alen + 1) * sizeof(char));
    if (NULL == ansi)
    {
        return NULL;
    }
    WideCharToMultiByte(CP_ACP, 0, widechar, -1, ansi, alen, NULL, NULL);
    ansi[alen] = 0;

    return ansi;
}


