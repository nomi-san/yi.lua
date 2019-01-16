#include "yi.h"
#include <stdarg.h>

#define MOD_ADLER 65521u;

uint32_t yi_adler32(const char *str, size_t len)
{
	uint32_t a = 1, b = 0;

	for (size_t i = 0; i < len; ++i) {
		a = (a + str[i]) % MOD_ADLER;
		b = (b + a) % MOD_ADLER;
	}

	return (b << 16) | a;
}

void *yiL_newudata(lua_State* L, size_t sz)
{
	void **ud = (void**)lua_newuserdata(L, sizeof(void*));
	*ud = calloc(1, sz);
	return *ud;
}

void *yiL_checkudata(lua_State* L, int i, const char* s)
{
	if (lua_isnoneornil(L, i)) return NULL;
	void **ud = (void**)luaL_checkudata(L, i, s);
	if (!ud || !*ud) return NULL;
	return *ud;
}

void *yiL_toudata(lua_State* L, int i)
{
	if (lua_isnoneornil(L, i)) return NULL;
	void **ud = (void**)lua_touserdata(L, i);
	if (!ud || !*ud) return NULL;
	return *ud;
}

void yiL_pushudata(lua_State* L, void* udata, const char* meta)
{
	if (!udata) {
		lua_pushnil(L);
		return;
	}

	void** ud = (void**)lua_newuserdata(L, sizeof(void*));
	*ud = udata;
	luaL_setmetatable(L, meta);
}

const char *yiL_tohstr(lua_State *L, int idx, uint32_t *hash)
{
	size_t len = 0;
	uint32_t _hash = 0;
	const char *str = lua_tolstring(L, idx, &len);

	_hash = yi_adler32(str, len);

	if (hash != NULL) *hash = _hash;
	return str;
}

uint32_t yiL_tohash(lua_State *L, int idx)
{
	size_t len = 0;
	uint32_t hash = 0;
	const char *str = lua_tolstring(L, idx, &len);

	hash = yi_adler32(str, len);

	return hash;
}

#if defined(yi_WIN32)
wchar_t *yiL_tolwstr(lua_State *L, int i, size_t *l) {
	if (lua_isnoneornil(L, i)) return L"";

	const char *s = lua_tolstring(L, i, l);
	int size = MultiByteToWideChar(CP_UTF8, 0, s, -1, 0, 0);

	wchar_t *ws = yi_alloc2(size, sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, s, -1, ws, size);

	return ws;
}

void yiL_pushwstr(lua_State *L, const wchar_t *wstr) {
	int size = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, /* s */ 0, /* size */ 0, 0, 0);

	char *s = (char*)calloc(size, sizeof(char));
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, s, size, 0, 0);

	lua_pushstring(L, s);
	free(s);
}
#else
#define CUTE_UTF_IMPLEMENTATION 1
#include "inc/cute_utf.h"

wchar_t *yiL_tolwstr(lua_State *L, int i, size_t *l)
{
	if (lua_isnoneornil(L, i)) return L"";

	int len;
	const char *str = lua_tolstring(L, i, &len);

	wchar_t *wstr = yi_alloc2(len, sizeof(wchar_t));
	cu_widen(str, wstr);

	if (l) *l = len;
	return wstr;
}

void yiL_pushwstr(lua_State *L, const wchar_t *wstr)
{
	if (!wstr) {
		lua_pushnil(L);
		return;
	}

	int len = wcslen(wstr);
	char *str = calloc(1, len * sizeof(wchar_t) + 1);
	cu_shorten(wstr, str);

	lua_pushstring(L, str);
	free(str);
}

#undef CUTE_UTF_IMPLEMENTATION
#endif

int yi_printf(const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	int ret = vfprintf(stdout, fmt, argp);
	va_end(argp);

	fflush(stdout);
	return ret;
}