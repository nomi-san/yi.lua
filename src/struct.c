#include "yi.h"

#define _ABS(N) ((N<0)?(-N):(N))

int yi_struct(lua_State *L)
{
	if (!lua_istable(L, 1)) return 0;
	uint32_t len = lua_rawlen(L, 1);

	if (len <= 0) return 0;

	yi_Struct *st = malloc(sizeof(yi_Struct));
	st->elms = NULL;
	st->addr = NULL;
	st->len = 0;
	st->size = 0;
	st->changed = 0;

#if defined(yi_X64)
	int alignment = 8;
#else
	int alignment = 4;
#endif

	lua_rawgeti(L, 1, 1);
	if (lua_isinteger(L, -1))
	{	lua_pop(L, 1);

		st->len = len;
		st->elms = malloc(sizeof(yi_Element) * st->len);

		for (int i = 0; i < st->len; i++) {
			lua_geti(L, 1, i + 1);
			int type = lua_tointeger(L, -1);
			lua_pop(L, 1);
			
			int align, offset, aligned, padding;
			offset = st->size;
			align = yi_sizeofType(type);

			padding = (-offset & (align - 1));
			aligned = (offset + (align - 1)) & -align;
			
			st->size = aligned + align;

			st->elms[i].type = type;
			st->elms[i].offset = aligned;

#if defined(yi_X64)
			if (align >= 8)
				alignment = 16u;
#else
			if (align >= 8)
				alignment = 8u;
#endif
		}
	}
	else if (lua_isstring(L, -1))
	{	lua_pop(L, 1);


		return 0;
	}

	if (st->size % alignment != 0)
		st->size += alignment - (st->size % alignment);

	st->addr = calloc(sizeof(char), st->size);

	yiL_pushudata(L, st, $__yi_struct);
	return 1;
}

static int yi_struct__index(lua_State *L)
{
	yi_Struct *st = yiL_toudata(L, 1);
	if (!st) return 0;

	if (lua_isinteger(L, 2))
	{
		int idx = lua_tointeger(L, 2);

		if (_ABS(idx) > st->len) return 0;

		switch (idx) {
			case -1:
				lua_pushlightuserdata(L, *(void**)st->addr);
				return 1;

			case 0:
				lua_pushinteger(L, st->len);
				return 1;
				sizeof(WNDCLASSEXA);
			default: {
				if (idx < 0) {

					void *e_addr = (void*)((char*)st->addr + st->elms[_ABS(idx)-1].offset);
					lua_pushlightuserdata(L, *(void**)e_addr);

					return 1;
				}

				yi_pushElement(L, st->addr, &st->elms[idx-1]);
			}
		}
	}

	return 1;
}

static int yi_struct__newindex(lua_State *L)
{
	yi_Struct *st = yiL_toudata(L, 1);
	if (!st) return 0;

	if (lua_isinteger(L, 2)) {
		int idx = lua_tointeger(L, 2);
		
		if (idx == 0) {
			if (lua_isinteger(L, 3))
				st->addr = (void*)lua_tointeger(L, 3);
			else
				st->addr = lua_touserdata(L, 3);

			st->changed = 1;
		}
		else
			yi_setElement(L, st->addr, &st->elms[_ABS(idx) - 1], 3);
	}

	return 0;
}

static int yi_struct__call(lua_State *L)
{
	yi_Struct *st = yiL_toudata(L, 1);
	if (!st) return 0;

	return 0;
}

static int yi_struct__len(lua_State *L)
{
	yi_Struct *st = yiL_toudata(L, 1);
	if (!st) return 0;

	lua_pushinteger(L, st->size);
	lua_pushinteger(L, st->len);
	return 2;
}

static int yi_struct__unm(lua_State *L)
{
	yi_Struct *st = yiL_toudata(L, 1);
	if (!st) return 0;

	lua_pushlightuserdata(L, st->addr);
	return 1;
}

static int yi_struct__eq(lua_State *L)
{
	yi_Struct *st = yiL_toudata(L, 1);
	yi_Struct *st2 = yiL_toudata(L, 1);

	bool ret = 1;

	if (!st || !st2)
		ret = 0;
	else
		ret = st->addr == st2->addr;

	lua_pushboolean(L, ret);
	return 0;
}

static int yi_struct__gc(lua_State *L)
{
	yi_Struct *st = yiL_toudata(L, 1);
	if (!st) return 0;

	if (st->addr && !st->changed)
		free(st->addr);

	if (st->elms)
		free(st->elms);

	free(st);
	return 0;
}

static int yi_struct__tostring(lua_State *L)
{
	yi_Struct *st = yiL_toudata(L, 1);
	if (!st) return 0;

	char buf[MAX_PATH];
	char *str = buf;

	str += sprintf(str, "%s: %p { ", $__yi_struct, st);

	for (int i = 0; i < st->len; i++) {

		const char *type = yi_typeofType(st->elms[i].type);

		if (i == (st->len - 1))
			str += sprintf(str, "%s }", type);
		else
			str += sprintf(str, "%s, ", type);
	}

	lua_pushstring(L, buf);
	return 1;
}

static const luaL_Reg yi_struct__meta[] = {

	{ "__index",	yi_struct__index },
	{ "__newindex", yi_struct__newindex },
	{ "__call",		yi_struct__call },
	{ "__len",		yi_struct__len },
	{ "__unm",		yi_struct__unm },
	{ "__eq",		yi_struct__eq },
	{ "__gc",		yi_struct__gc },
	{ "__tostring",	yi_struct__tostring },

	{ NULL, NULL }
};

void yi_struct_reg(lua_State *L)
{
	luaL_newmetatable(L, $__yi_struct);
	luaL_setfuncs(L, yi_struct__meta, 0);
	lua_pop(L, 1);
}