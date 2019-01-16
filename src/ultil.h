#ifndef _yi_ultil_
#define _yi_ultil_
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//#define _DEBUG 1

#if defined(DEBUG) || defined(_DEBUG)
#define yi_DEBUG 1
#endif

#if defined(WIN32) || defined(_WIN32)
#if defined(_MSC_VER)
#pragma warning(disable : 4996)
#pragma optimize( "y", on )
#pragma strict_gs_check(push, off)
#endif
#define yi_WIN32 1
#include <windows.h>
#define yi_CLBK __stdcall
#else
#define yi_UNIX 1
#include <unistd.h>
#include <dlfcn.h>
#define yi_CLBK
#endif

#if defined(_M_AMD64) || defined(_M_X64) || defined(__x86_64__) || defined(__x86_64)
#if defined(__GNUC__)
#pragma GCC error "GNU compiler for Windows 64bit (x86_64) is not supported!"
#endif
#define yi_X64 1
#elif defined(_M_I86) || defined(_M_IX86) || defined(__i386__) || defined (__i386)
#define yi_X86 1
#endif

#if defined(_MSC_VER)
#define yi_MSVC 1
#elif defined(__GNUC__)
#define yi_GNU 1
#else
#error "unsupported compiler!"
#endif

#if !defined(MAX_PATH)
#define MAX_PATH 255
#endif

typedef enum _yi_ObjType yi_ObjType;

void yi_addMem(void *ptr, yi_ObjType type);
void yi_cleanMem();

void *yi_alloc(size_t size);
void *yi_alloc2(size_t count, size_t size);
void *yi_realloc(void *ptr, size_t size);
void yi_free(void *ptr);
#define yi_alloct(type) yi_alloc(sizeof(type))
#define yi_alloct2(type) yi_alloc2(sizeof(type))

uint32_t yi_adler32(const char *str, size_t len);

void *yiL_newudata(lua_State* L, size_t sz);
void *yiL_checkudata(lua_State* L, int i, const char* s);
void *yiL_toudata(lua_State* L, int i);
void yiL_pushudata(lua_State* L, void* udata, const char* meta);
const char *yiL_tohstr(lua_State *L, int idx, uint32_t *hash);
uint32_t yiL_tohash(lua_State *L, int idx);

wchar_t *yiL_tolwstr(lua_State *L, int i, size_t *l);
void yiL_pushwstr(lua_State *L, const wchar_t *wstr);
#define yiL_towstr(L, i) yiL_tolwstr(L, i, NULL);

int yi_printf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif // !_yi_ultil_
