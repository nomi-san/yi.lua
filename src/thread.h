#ifndef _yi_thread_
#define _yi_thread_
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "yi.h"

#if defined(yi_WIN32)

#define thread_exit(handle) \
	TerminateThread(handle, 0)

#define thread_status DWORD
#define thread_handle HANDLE
#else
#include <pthread.h>

#define thread_exit() \
	pthread_exit(NULL)

#define thread_status void*
#define thread_handle pthread_t
#endif

typedef struct _yi_Thread {

	lua_State *main_state;
	lua_State *state;
	thread_handle handle;
	int data;
	int fn;

} yi_Thread;

#ifdef __cplusplus
}
#endif

#endif // !_yi_thread_
