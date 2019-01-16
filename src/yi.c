#include "yi.h"

int yiL_Ref;

int yi_thread(lua_State *L);
int yi_closure(lua_State *L);
int yi_p2i(lua_State *L);
int yi_i2p(lua_State *L);
int yi_check(lua_State *L);
int yi_load(lua_State *L);
int yi_enum(lua_State *L);
int yi_struct(lua_State *L);

#define yiH__test    0x045d01c1

static int yi_test(lua_State *L)
{
	void *x = lua_touserdata(L, 1);
	int ret = GetCursorPos(x);

	lua_pushinteger(L, ret);
	return 1;
}

static int yi__index(lua_State *L)
{
	uint32_t hash;
	const char *idx = yiL_tohstr(L, 2, &hash);

	switch (hash)
	{
		case hash(test):
			lua_pushcfunction(L, yi_test);
			break;

		case hash(closure):
			lua_pushcfunction(L, yi_closure);
			break;

		case hash(enum):
			lua_pushcfunction(L, yi_enum);
			break;

		case hash(struct):
			lua_pushcfunction(L, yi_struct);
			break;

		case hash(thread):
			lua_pushcfunction(L, yi_thread);
			break;

		case hash(check):
			lua_pushcfunction(L, yi_check);
			break;

		case hash(load):
			lua_pushcfunction(L, yi_load);
			break;

		case hash(p2i):
			lua_pushcfunction(L, yi_p2i);
			break;

		case hash(i2p):
			lua_pushcfunction(L, yi_i2p);
			break;

		case hash(types):
			lua_rawgeti(L, LUA_REGISTRYINDEX, yiL_Ref);
			lua_rawgeti(L, -1, 1);
			break;

		default:
			return 0;
	}

	return 1;
}

static int yi__gc(lua_State *L)
{
	yi_cleanMem();
	return 0;
}

static const luaL_Reg yi__meta[] = {
	{ "__index",		yi__index },
	{ "__gc",			yi__gc },
	{ NULL, NULL }
};

void yi_closure_reg(lua_State *L);
void yi_thread_reg(lua_State *L);
void yi_types_reg(lua_State *L);
void yi_dcall_reg(lua_State *L);
void yi_struct_reg(lua_State *L);

#if defined(yi_WIN32) || defined(_MSC_VER)
__declspec(dllexport)
#elif defined(__GNUC__)
__attribute__((visibility("default")))
#endif
int luaopen_yi(lua_State *L)
{
	luaL_checkversion(L);

	lua_newtable(L);

	yi_types_reg(L);

	lua_pushvalue(L, -1);
	yiL_Ref = luaL_ref(L, LUA_REGISTRYINDEX);
	lua_pop(L, 1);

	yi_closure_reg(L);
	yi_thread_reg(L);
	yi_dcall_reg(L);
	yi_enum_reg(L);
	yi_struct_reg(L);

	luaL_newmetatable(L, $__yi);
	luaL_setfuncs(L, yi__meta, 0);
	lua_pop(L, 1);

	lua_newuserdata(L, 1u);
	luaL_setmetatable(L, $__yi);

	return 1;
}

struct yi_FakeStackT {

	char		__0;
	short		__1;
	int			__2;
	long long	__3;
	float		__4;
	double		__5;
	char*		__6;
	unsigned
		short*	__7;
	void*		__8;

};