#include "yi.h"

typedef struct {

	size_t size;
	int ref;

} yi_Enum;

int yi_enum(lua_State *L)
{
	if (!lua_istable(L, 1)) return 0;
	size_t len = lua_rawlen(L, 1);

	if (!len) return 0;
	lua_rawgeti(L, 1, 1);

	if (!lua_isstring(L, -1)) return 0;
	lua_pop(L, 1);

	yi_Enum *en = malloc(sizeof(yi_Enum));
	lua_pushvalue(L, 1);
	en->size = len;
	en->ref = luaL_ref(L, LUA_REGISTRYINDEX);

	for (int i = 1, n = 0; i <= len; i++) {
		lua_rawgeti(L, 1, i);

		const char *v = lua_tostring(L, -1);

		if (lua_rawgeti(L, 1, i + 1) && lua_isinteger(L, -1)) {
			n = lua_tointeger(L, -1);
			i++;
			lua_pop(L, 1);
		}

		lua_pushinteger(L, n);
		lua_setfield(L, 1, v);
		n++;
		lua_pop(L, 1);
	}

	yiL_pushudata(L, en, $__yi_enum);
	return 1;
}

static int yi_enum__index(lua_State *L)
{
	yi_Enum *en = yiL_toudata(L, 1);
	if (!en) return 0;

	if (lua_isstring(L, 2)) {
		const char* idx = lua_tostring(L, 2);
		lua_rawgeti(L, LUA_REGISTRYINDEX, en->ref);
		if (lua_getfield(L, -1, idx))
			return 1;
	}

	return 0;
}

static int yi_enum__len(lua_State *L)
{
	yi_Enum *en = yiL_toudata(L, 1);
	if (!en) return 0;

	lua_pushinteger(L, en->size);
	return 1;
}

static int yi_enum__unm(lua_State *L)
{
	yi_Enum *en = yiL_toudata(L, 1);
	if (!en) return 0;

	lua_pushlightuserdata(L, en);
	return 1;
}

static int yi_enum__tostring(lua_State *L)
{
	yi_Enum* en = yiL_toudata(L, 1);
	if (!en) return 0;

	char buf[MAX_PATH];
	sprintf(buf, "%s: %p", $__yi_enum, en);

	lua_pushstring(L, buf);
	return 1;
}

static int yi_enum__gc(lua_State *L)
{
	yi_Enum *en = yiL_toudata(L, 1);
	if (!en) return 0;

	if (en->ref != -1)
		luaL_unref(L, LUA_REGISTRYINDEX, en->ref);

	free(en);
	return 0;
}

static const luaL_Reg yi_enum_meta[] = {

	{ "__index",	yi_enum__index},
	{ "__len",		yi_enum__len},
	{ "__unm",		yi_enum__unm},
	{ "__tostring",	yi_enum__tostring},
	{ "__gc",		yi_enum__gc},

	{ NULL, NULL}
};

void yi_enum_reg(lua_State *L)
{
	luaL_newmetatable(L, $__yi_enum);
	luaL_setfuncs(L, yi_enum_meta, 0);
	lua_pop(L, 1);
}