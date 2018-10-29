#include "milo.h"

wchar_t *str_to_wstr(const char* str)
{
	int size = strlen(str) + 1;
	wchar_t *wstr = malloc(size * sizeof(wchar_t));
	MultiByteToWideChar(65001, 0, str, -1, wstr, size);
	return wstr;
}

void lua_pushwstring(lua_State *L, wchar_t *wstr) {
	int size = wcslen(wstr);
	char *str = malloc(sizeof(char) * size);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, size, 0, 0);
	lua_pushstring(L, str);
	free(str);
}