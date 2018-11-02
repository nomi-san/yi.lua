#include "milo.h"
#include "types.h"
#include "struct.h"
#include <malloc.h>

void* milo_newudata(lua_State* L, size_t sz)
{
	void **ptr = (void**)lua_newuserdata(L, sizeof ptr);
	*ptr = calloc(1, sz);
	return *ptr;
}

void milo_pushudata(lua_State *L, void *udata, const char *name)
{
	if (!udata) return;
	void** ptr = (void**)lua_newuserdata(L, sizeof ptr);
	*ptr = udata;
	luaL_setmetatable(L, name);
}

void* milo_getudata(lua_State *L, int idx)
{
	void **ud = (void**)lua_touserdata(L, idx);
	if (!ud || !*ud) return NULL;
	return *ud;
}

void* milo_checkudata(lua_State *L, int idx, const char *name)
{
	void **ud = (void**)luaL_checkudata(L, idx, name);
	if (!ud || !*ud) return NULL;
	return *ud;
}

// .free, __gc
int milo_free(lua_State *L)
{
	void* udata = milo_getudata(L, 1);
	if (udata) {
		free(udata);
		udata = NULL;
		lua_pushboolean(L, 1);
	}
	else
		lua_pushboolean(L, 0);

	return 1;
}

/* metatable: func
--------------------------------------------------*/

static int milo_func_def(lua_State *L)
{
	milo_func_t* fn = milo_getudata(L, 1);
	fn->type = lua_tointeger(L, 2);
	fn->nargs = lua_rawlen(L, 3);

	if (!lua_isnoneornil(L, 4)
		|| lua_toboolean(L, 4))
		fn->std = 1;

	fn->args = alloc(sizeof(milo_arg_t) * (fn->nargs));

	if ((fn->nargs != 0) || !lua_isnoneornil(L, 3))
		for (int i = 0; i < fn->nargs; i++) {
			lua_rawgeti(L, 3, i + 1);

			milo_type_t t = lua_tointeger(L, -1);
			size_t sz = milo_type_tosize(t);

			fn->args[i].type = t;
			fn->args[i].offset = fn->size;

			fn->size += sz;
		}

	milo_pushudata(L, fn, __milo_func__);
	return 1;
}

static int milo_func_call(lua_State *L)
{
	milo_func_t* fn = milo_getudata(L, 1);
	if (!fn) return 0;

	int top = lua_gettop(L);

	if (top < 1) return 0;
	if (fn->nargs != (top - 1))
		return 0;
	else {
		// allocate on stack for args
		void* args = _alloca(fn->size);

		for (int i = 0; i < fn->nargs; i++) {
			void* val = milo_struct_getelement(args, fn->args[i].offset);
			milo_struct_setvalue(
				L,
				val,
				fn->args[i].type,
				i + 2
			);
		}

		if (fn->std) // stdcall
			switch (fn->type) { // call function with stack                 // my idea, not bad ")
				case c_f32: fn->ret.f32_v = ((float(__stdcall*)())fn->addr)(*(milo_not_union_t*)args); break;
				case c_f64: // I hate float and double types
				case c_num: fn->ret.f64_v = ((double(__stdcall*)())fn->addr)(*(milo_not_union_t*)args); break;
				default: fn->ret = ((milo_variant_t(__stdcall*)())fn->addr)(*(milo_not_union_t*)args); break;
			}
		else
			switch(fn->type) {
				case c_f32: fn->ret.f32_v = ((float(*)())fn->addr)(*(milo_not_union_t*)args); break;
				case c_f64:
				case c_num: fn->ret.f64_v = ((double(*)())fn->addr)(*(milo_not_union_t*)args); break;
				default: fn->ret = ((milo_variant_t(*)())fn->addr)(*(milo_not_union_t*)args); break;
			}

		return milo_pushvariant(L, fn->ret, fn->type);
	}

	return 0;
}

static int milo_func_index(lua_State *L)
{
	milo_func_t* fn = milo_getudata(L, 1);
	const char* id = lua_tostring(L, 2);

	if (!strcmp(id, "name"))
		lua_pushstring(L, fn->name);

	else if (!strcmp(id, "addr"))
		lua_pushlightuserdata(L, fn->addr);

	else if (!strcmp(id, "def"))
		lua_pushcfunction(L, milo_func_def);

	else return 0;

	return 1;
}

static const luaL_Reg milo_func_meta[] = {
	{ "__call", milo_func_call },
	{ "__index", milo_func_index },
	{ NULL, NULL}
};

void milo_func_reg(lua_State *L)
{
	luaL_newmetatable(L, "func");
	luaL_setfuncs(L, milo_func_meta, 0);
	lua_pop(L, 1);
}

/* .load
--------------------------------------------------*/

static int milo_load(lua_State *L)
{
	milo_lib_t* lib = alloc_t(milo_lib_t);
	const char* name = lua_tostring(L, 1);

	lib->addr = LoadLibraryA(name);

	if (!lib->addr)
		return 0;
	else
		lib->name = _strdup(name);

	milo_pushudata(L, lib, __milo_lib__);
	return 1;
}

/* metatable: lib
--------------------------------------------------*/

static int milo_lib_index(lua_State *L)
{
	milo_lib_t* lib = milo_getudata(L, 1);
	const char* id = lua_tostring(L, 2);

	if (!strcmp(id, "name"))
		lua_pushstring(L, lib->name);

	else if (!strcmp(id, "addr"))
		lua_pushlightuserdata(L, lib->addr);

	else {
		void* addr = GetProcAddress(lib->addr, id);
		if (!addr) return 0;

		milo_func_t* fn = alloc_t(milo_func_t);

		fn->name = _strdup(id);
		fn->addr = addr;

		milo_pushudata(L, fn, __milo_func__);
		return 1;
	}

	return 1;
}

static const luaL_Reg milo_lib_meta[] = {
	{ "__index", milo_lib_index },
	{ NULL, NULL}
};

void milo_lib_reg(lua_State *L)
{
	luaL_newmetatable(L, __milo_lib__);
	luaL_setfuncs(L, milo_lib_meta, 0);
	lua_pop(L, 1);
}

long milo_loword(long x)
{
	return x & 0xffff;
}

long milo_hiword(long x)
{
	return x >> 0x10;
}

long milo_mkparam(long l, long h)
{
	return milo_loword(l) | (0x10000 * milo_loword(h));
}

int milo_ct(lua_State *L)
{
	lua_Integer p = milo_mkparam(
		lua_tointeger(L, 1),
		lua_tointeger(L, 2)
	);
	lua_pushinteger(L, p);
	return 1;
}

int milo_sleep(lua_State *L)
{
	if (!lua_isinteger(L, 1)) return 0;
	_sleep(lua_tointeger(L, 1));
	return 0;
}

__declspec(dllexport)
int luaopen_milo(lua_State *L)
{
	milo_func_reg(L);
	milo_lib_reg(L);

	lua_newtable(L);

	milo_struct_reg(L);
	milo_closure_reg(L);

	lua_pushcfunction(L, milo_load);
	lua_setfield(L, -2, "load");

	lua_pushcfunction(L, milo_sleep);
	lua_setfield(L, -2, "sleep");
	
	lua_pushcfunction(L, milo_ct);
	lua_setfield(L, -2, "ct");

	lua_createtable(L, 0, 0);

	lua_pushinteger(L, c_u8);
	lua_setfield(L, -2, "u8");

	lua_pushinteger(L, c_u16);
	lua_setfield(L, -2, "u16");

	lua_pushinteger(L, c_u32);
	lua_setfield(L, -2, "u32");

	lua_pushinteger(L, c_u64);
	lua_setfield(L, -2, "u64");

	lua_pushinteger(L, c_i8);
	lua_setfield(L, -2, "i8");

	lua_pushinteger(L, c_i16);
	lua_setfield(L, -2, "i16");

	lua_pushinteger(L, c_i32);
	lua_setfield(L, -2, "i32");

	lua_pushinteger(L, c_i64);
	lua_setfield(L, -2, "i64");

	lua_pushinteger(L, c_f32);
	lua_setfield(L, -2, "f32");

	lua_pushinteger(L, c_f64);
	lua_setfield(L, -2, "f64");

	lua_pushinteger(L, c_bool);
	lua_setfield(L, -2, "bool");

	lua_pushinteger(L, c_char);
	lua_setfield(L, -2, "char");

	lua_pushinteger(L, c_wchar);
	lua_setfield(L, -2, "wchar");

	lua_pushinteger(L, c_int);
	lua_setfield(L, -2, "int");

	lua_pushinteger(L, c_int64);
	lua_setfield(L, -2, "int64");

	lua_pushinteger(L, c_uint);
	lua_setfield(L, -2, "uint");

	lua_pushinteger(L, c_uint64);
	lua_setfield(L, -2, "uint64");

	lua_pushinteger(L, c_num);
	lua_setfield(L, -2, "num");

	lua_pushinteger(L, c_str);
	lua_setfield(L, -2, "str");

	lua_pushinteger(L, c_wstr);
	lua_setfield(L, -2, "wstr");

	lua_pushinteger(L, c_none);
	lua_setfield(L, -2, "none");

	lua_pushinteger(L, c_ptr);
	lua_setfield(L, -2, "ptr");

	lua_setfield(L, -2, "types");

	return 1;
}
