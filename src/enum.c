#include "milo.h"

static const char* __milo_enum__ = "enum";

typedef struct {
	size_t size;
	int ref;
} milo_enum_t;

static int milo_enum(lua_State *L)
{
	if (!lua_istable(L, 1)) return 0;
	size_t len = lua_rawlen(L, 1);

	if (!len) return 0;
	lua_rawgeti(L, 1, 1);

	if (!lua_isstring(L, -1)) return 0;
	lua_pop(L, 1);

	milo_enum_t *en = alloc_t(milo_enum_t);
	lua_pushvalue(L, 1);
	en->size = len;
	en->ref = luaL_ref(L, LUA_REGISTRYINDEX);

	for (int i = 1, n = 0; i <= len; i++) {
		lua_rawgeti(L, 1, i);

		const char *v = lua_tostring(L, -1);

		if (lua_rawgeti(L, 1, i+1) && lua_isinteger(L, -1)) {
			n = lua_tointeger(L, -1);
			i++;
			lua_pop(L, 1);
		}

		lua_pushinteger(L, n);
		lua_setfield(L, 1, v);
		n++;
		lua_pop(L, 1);
	}

	milo_pushudata(L, en, __milo_enum__);
	return 1;
}

static int milo_enum_index(lua_State *L)
{
	milo_enum_t *en = milo_getudata(L, 1);

	if (lua_isstring(L, 2)) {
		const char* idx = lua_tostring(L, 2);
		lua_rawgeti(L, LUA_REGISTRYINDEX, en->ref);
		if (lua_getfield(L, -1, idx))
			return 1;
	}

	return 0;
}

static int milo_enum_len(lua_State *L)
{
	milo_enum_t *en = milo_getudata(L, 1);
	lua_pushinteger(L, en->size);
	return 1;
}

static int milo_enum_unm(lua_State *L)
{
	milo_enum_t *en = milo_getudata(L, 1);
	lua_pushlightuserdata(L, en);
	return 1;
}

static int milo_enum_tostring(lua_State *L)
{
	milo_enum_t* en = milo_getudata(L, 1);
	if (!en) return 0;

	char buf[MILO_TS_LEN];
	snprintf(buf, MILO_TS_LEN, "%s: %08x", __milo_enum__, (unsigned int)en);
	lua_pushstring(L, buf);

	return 1;
}

static int milo_enum_gc(lua_State *L)
{
	milo_enum_t *en = milo_getudata(L, 1);
	if (en) free(en);
	return 0;
}

static const luaL_Reg milo_enum_meta[] = {
	{ "__index",	milo_enum_index},
	{ "__len",		milo_enum_len},
	{ "__unm",		milo_enum_unm},
	{ "__tostring",		milo_enum_tostring},
	{ "__gc",		milo_enum_gc},
	{ NULL, NULL}
};

void milo_enum_reg(lua_State *L)
{
	luaL_newmetatable(L, __milo_enum__);
	luaL_setfuncs(L, milo_enum_meta, 0);
	lua_pop(L, 1);

	lua_pushcfunction(L, milo_enum);
	lua_setfield(L, -2, "enum");
}