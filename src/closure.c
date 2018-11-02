#include "struct.h"
#include "clofn.h"

static const char* __milo_closure__ = "closure";

typedef struct {
	void*		addr;
	int			ref;
	lua_State *	L;
	milo_type_t type;
	void*		ret;
	size_t		nargs;
	milo_arg_t* args;
} milo_closure_t;

/*
	// my closure proxy //

	call(a, b, ...)

	=> proxy(args[])
		|-> &args + 0 => a
		|-> &args + sizeof(a) => b
		|...
*/
def_clofn(void*, milo_closure_proxy, size_t, data, (void *args[]),
{
	milo_closure_t* cl = (milo_closure_t*)data;
	lua_State* L = cl->L;

	lua_rawgeti(L, LUA_REGISTRYINDEX, cl->ref);
	if (!lua_isfunction(L, -1))
		return NULL;

	for (int i = 0; i < cl->nargs; i++) {
		void* val = milo_struct_getelement(&args, cl->args[i].offset);
		milo_struct_pushvalue(L, val, cl->args[i].type);
	}

	if (cl->type != c_none) {
		lua_call(L, cl->nargs, 1);
		lua_pushvalue(L, -1);
		milo_struct_setvalue(L, &cl->ret, cl->type, -1);

		return cl->ret;
	}

	lua_call(L, cl->nargs, 0);
	return NULL;
})

/*
	closure = milo.closure (
		function,
		type, {arg_type}
	)
*/
static int milo_closure(lua_State *L)
{
	if (!lua_isfunction(L, 1) || !lua_istable(L, 3))
		return 0;

	milo_closure_t* cl = alloc_t(milo_closure_t);
	cl->L = L;

	lua_pushvalue(L, 1);
	cl->ref = luaL_ref(L, LUA_REGISTRYINDEX);
	cl->type = lua_tointeger(L, 2);
	cl->nargs = lua_rawlen(L, 3);

	cl->args = alloc(sizeof(milo_arg_t) * cl->nargs);

	size_t size = 0;

	for (int i = 0; i < cl->nargs; i++) {
		lua_rawgeti(L, 3, i + 1);
		milo_type_t t = lua_tointeger(L, -1);
		size_t sz = milo_type_tosize(t);

		cl->args[i].offset = size;
		cl->args[i].type = t;

		size += sz;
	}

	cl->addr = new_clofn(milo_closure_proxy, cl);

	milo_pushudata(L, cl, __milo_closure__);
	return 1;
}

/*
	closure(...) --> call closure function
*/
static int milo_closure__call(lua_State *L)
{
	milo_closure_t* cl = milo_getudata(L, 1);

	lua_rawgeti(L, LUA_REGISTRYINDEX, cl->ref);
	if (!lua_isfunction(L, -1)) return 0;

	for (int i = 0; i < cl->nargs; i++)
		lua_pushvalue(L, i + 2);

	if (cl->type != c_none) {
		lua_call(L, cl->nargs, 1);
		lua_pushvalue(L, -1);
	}
	else {
		lua_call(L, cl->nargs, 0);
		lua_pushnil(L);
	}

	return 1;
}

/*
	#closure --> number of argument
*/
static int milo_closure__len(lua_State *L)
{
	milo_closure_t* cl = milo_getudata(L, 1);
	if (!cl) return 0;

	lua_pushinteger(L, cl->nargs);
	return 1;
}

/*
	-closure --> pointer to closure function
*/
static int milo_closure__unm(lua_State *L)
{
	milo_closure_t* cl = milo_getudata(L, 1);
	if (!cl) return 0;

	lua_pushlightuserdata(L, cl->addr);
	return 1;
}

/*
	closure:__tostring()
	tostring(closure)
	print(closure) --> closure: 0abc1234
*/
static int milo_closure__tostring(lua_State *L)
{
	milo_closure_t* cl = milo_getudata(L, 1);
	if (!cl) return 0;

	char buf[MILO_TS_LEN];
	snprintf(buf, MILO_TS_LEN, "%s: %08x", __milo_closure__, (unsigned int)cl->addr);
	lua_pushstring(L, buf);

	return 1;
}

/*
	auto gc
*/
static int milo_closure__gc(lua_State *L)
{
	milo_closure_t* cl = milo_getudata(L, 1);
	if (!cl) return 0;

	luaL_unref(L, LUA_REGISTRYINDEX, cl->ref);
	free(cl->args);
	free(cl->addr);
	free(cl);

	return 0;
}

static const luaL_Reg milo_closure_meta[] = {
	{ "__call", milo_closure__call },
	{ "__len", milo_closure__len },
	{ "__unm", milo_closure__unm },
	{ "__tostring", milo_closure__tostring },
	{ "__gc", milo_closure__gc },
	{ NULL, NULL }
};

void milo_closure_reg(lua_State *L)
{
	luaL_newmetatable(L, __milo_closure__);
	luaL_setfuncs(L, milo_closure_meta, 0);
	lua_pop(L, 1);

	lua_pushcfunction(L, milo_closure);
	lua_setfield(L, -2, "closure");
}