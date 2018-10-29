#ifndef MILO_H
#define MILO_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "lua.h"
#include "lapi.h"
#include "lauxlib.h"

void* milo_newudata(lua_State* L, size_t sz);
void milo_pushudata(lua_State *L, void *udata, const char *name);
void* milo_getudata(lua_State *L, int idx);
void* milo_checkudata(lua_State *L, int idx, const char *name);
int milo_free(lua_State *L);

#define alloc(s)	calloc(1, (s))
#define alloc_t(t)	calloc(1, sizeof(t))

wchar_t *str_to_wstr(const char* str);
void lua_pushwstring(lua_State *L, wchar_t *wstr);

#ifdef __cplusplus
}
#endif

#endif