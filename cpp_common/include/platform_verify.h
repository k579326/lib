

#ifndef _PLATFORM_VERIFY_H_
#define _PLATFORM_VERIFY_H_


#ifdef WIN32
	#define PW_OS_WIN		1
#elif defined __ANDROID__
	#define PW_OS_ANDROID 	1
#elif defined __LINUX__
	#define PW_OS_LINUX 	1
#elif defined __APPLE__
	#include <TargetConditionals.h>
	#if (TARGET_OS_IOS || TARGET_OS_SIMULATOR)		// only support iOS phone and simulator.
		#define PW_OS_IOS 	1
	#endif

	#if TARGET_OS_OSX
		#define PW_OS_MAC 	1
	#endif
#else
	#error current platform unsupported!
#endif


#endif // _PLATFORM_VERIFY_H_

