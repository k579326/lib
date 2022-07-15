

#pragma once

#include <stdint.h>
#if defined _WIN32 || defined WIN32
#include <windows.h>
#elif defined __linux__ || defined _linux
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <unistd.h>
#include <sys/syscall.h>
#elif defined __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#include <iOS_special.h>
#endif
#else
#error "ERROR, target platform not supported"
#endif


static uint32_t GetSelfThreadId()
{
#if defined _WIN32 || defined WIN32	
	return GetCurrentThreadId();
#elif defined __unix__
	return syscall(SYS_gettid);
#elif defined __APPLE__
	// iOS/macOS 均没有实现SYS_gettid系统调用，总是返回-1，暂时不考虑APPLE系统的tid获取，
	// 实在需要，使用pthread_t代替，虽然返回的结果与gettid不同，但也可起到区分不同线程的作用
	return 0;
#endif
}

