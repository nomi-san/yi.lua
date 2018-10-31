#include "milo.h"
#include "types.h"

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

void milo_cast(lua_State *L, int idx, milo_value_t *val)
{
	switch (val->t)
	{
		case c_bool:
			*(c_bool_t*)&val->val = lua_toboolean(L, idx);
			break;

		case c_u8:
			*(c_u8_t*)&val->val = lua_tointeger(L, idx);
			break;

		case c_u16:
			*(c_u16_t*)&val->val = lua_tointeger(L, idx);
			break;

		case c_u32:
		case c_uint:
			*(c_u32_t*)&val->val = lua_tointeger(L, idx);
			break;

		case c_u64:
		case c_uint64:
			*(c_u64_t*)&val->val = lua_tointeger(L, idx);
			break;

		case c_i8:
			*(c_i8_t*)&val->val = lua_tointeger(L, idx);
			break;

		case c_i16:
			*(c_i16_t*)&val->val = lua_tointeger(L, idx);
			break;

		case c_i32:
		case c_int:
			*(c_i32_t*)&val->val = lua_tointeger(L, idx);
			break;

		case c_i64:
		case c_int64:
			*(c_i64_t*)&val->val = lua_tointeger(L, idx);
			break;

		case c_f32:
			*(c_f32_t*)&val->val = lua_tonumber(L, idx);
			break;

		case c_f64:
		case c_num:
			*(c_f64_t*)&val->val = lua_tonumber(L, idx);
			break;

		case c_str:
			*(c_str_t*)&val->val = _strdup(lua_tostring(L, idx));
			break;

		case c_wstr:
			*(c_wstr_t*)&val->val = str_to_wstr(lua_tostring(L, idx));
			break;

		case c_ptr:
			*(c_ptr_t*)&val->val = lua_touserdata(L, idx);
			break;

		case c_none:
			*(c_ptr_t*)&val->val = NULL;
			break;

		//case c_char:
		//	break;
		//case c_wchar:
		//	break;
	}
}

int milo_return(lua_State *L, milo_value_t *val)
{
	switch (val->t)
	{
		case c_bool:
			lua_pushboolean(L, *(c_bool_t*)&val->val);
			break;

		case c_u8:
			lua_pushinteger(L, *(c_u8_t*)&val->val);
			break;

		case c_u16:
			lua_pushinteger(L,  *(c_u16_t*)&val->val);
			break;

		case c_u32:
		case c_uint:
			lua_pushinteger(L, *(c_u32_t*)&val->val);
			break;

		case c_u64:
		case c_uint64:
			lua_pushinteger(L, *(c_u64_t*)&val->val);
			break;

		case c_i8:
			lua_pushinteger(L, *(c_i8_t*)&val->val);
			break;

		case c_i16:
			lua_pushinteger(L, *(c_i16_t*)&val->val);
			break;

		case c_i32:
		case c_int:
			lua_pushinteger(L, *(c_i32_t*)&val->val);
			break;

		case c_i64:
		case c_int64:
			lua_pushinteger(L, *(c_i64_t*)&val->val);
			break;

		case c_f32:
			lua_pushnumber(L, *(c_f32_t*)&val->val);
			break;

		case c_f64:
		case c_num:
			lua_pushnumber(L, *(c_f64_t*)&val->val);
			break;

		case c_str:
			lua_pushstring(L, *(c_str_t*)&val->val);
			break;

		case c_wstr:
			lua_pushwstring(L, *(c_wstr_t*)&val->val);
			break;

		case c_ptr:
			lua_pushlightuserdata(L, *(c_ptr_t*)&val->val);
			break;

		case c_none:
			lua_pushnil(L);
			break;

		//case c_char:
		//	break;
		//case c_wchar:
		//	break;
		default: return 0;
	}

	return 1;
}

/* metatable: func
--------------------------------------------------*/

int milo_func_def(lua_State *L)
{
	milo_func_t* fn = milo_getudata(L, 1);
	fn->ret.t = lua_tointeger(L, 2);
	fn->n = lua_rawlen(L, 3);

	if (!lua_isnoneornil(L, 4)
		|| lua_toboolean(L, 4))
		fn->std = 1;

	fn->args = alloc(sizeof(milo_value_t) * (fn->n));

	if ((fn->n != 0) || !lua_isnoneornil(L, 3))
		for (int i = 0; i < fn->n; i++) {
			lua_rawgeti(L, 3, i + 1);
			fn->args[i].t = lua_tointeger(L, -1);
		}

	milo_pushudata(L, fn, __milo_func__);
	return 1;
}

int milo_func_call(lua_State *L)
{
	milo_func_t* fn = milo_getudata(L, 1);

	if ((lua_gettop(L)) < 1) return 0;
	if (fn->n != (lua_gettop(L) - 1))
		return 0;
	else
		for (int i = 0; i < fn->n; i++)
			milo_cast(L, i+2, &fn->args[i]);

	switch (fn->n) {
		case 0: {
			fn->ret.val = fn->std ? ((milo_fn_std_t)fn->addr)() 
				: ((milo_fn_t)fn->addr)();
			break;
			}
		case 1: {
			fn->ret.val = fn->std ? ((milo_fn_std_t)fn->addr)(
				fn->args[0].val
			) : ((milo_fn_t)fn->addr)(
					fn->args[0].val
				);
			break;
		}
		case 2: {
			fn->ret.val = fn->std ? ((milo_fn_std_t)fn->addr)(
				fn->args[0].val,
				fn->args[1].val
			) : ((milo_fn_t)fn->addr)(
				fn->args[0].val,
				fn->args[1].val
			);
			break;
		}
		case 3: {
			fn->ret.val = fn->std ? ((milo_fn_std_t)fn->addr)(
				fn->args[0].val,
				fn->args[1].val, fn->args[2].val
			) : ((milo_fn_t)fn->addr)(
				fn->args[0].val,
				fn->args[1].val, fn->args[2].val
			);
			break;
		}
		case 4: {
			fn->ret.val = fn->std ? ((milo_fn_std_t)fn->addr)(
				fn->args[0].val, fn->args[1].val,
				fn->args[2].val, fn->args[3].val
			) : ((milo_fn_t)fn->addr)(
				fn->args[0].val, fn->args[1].val,
				fn->args[2].val, fn->args[3].val
			);
			break; 
		}
		case 5: {
			fn->ret.val = fn->std ? ((milo_fn_std_t)fn->addr)(
				fn->args[0].val, fn->args[1].val,
				fn->args[2].val, fn->args[3].val, fn->args[4].val
			) : ((milo_fn_t)fn->addr)(
				fn->args[0].val, fn->args[1].val,
				fn->args[2].val, fn->args[3].val, fn->args[4].val
			);
			break;
		}
		case 6: {
			fn->ret.val = fn->std ? ((milo_fn_std_t)fn->addr)(
				fn->args[0].val, fn->args[1].val,fn->args[2].val, 
				fn->args[3].val, fn->args[4].val, fn->args[5].val
			) : ((milo_fn_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val,
				fn->args[3].val, fn->args[4].val, fn->args[5].val
			);
			break;
		}
		case 7: {
			fn->ret.val = fn->std ? ((milo_fn_std_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val,
				fn->args[3].val, fn->args[4].val, fn->args[5].val, fn->args[6].val
			) : ((milo_fn_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val,
				fn->args[3].val, fn->args[4].val, fn->args[5].val, fn->args[6].val
			);
			break;
		}
		case 8: {
			fn->ret.val = fn->std ? ((milo_fn_std_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val,
				fn->args[4].val, fn->args[5].val, fn->args[6].val, fn->args[7].val
			) : ((milo_fn_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val,
				fn->args[4].val, fn->args[5].val, fn->args[6].val, fn->args[7].val
			);
			break;
		}
		case 9: {
			fn->ret.val = fn->std ? ((milo_fn_std_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val,
				fn->args[4].val, fn->args[5].val, fn->args[6].val, fn->args[7].val, fn->args[8].val
			) : ((milo_fn_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val,
				fn->args[4].val, fn->args[5].val, fn->args[6].val, fn->args[7].val, fn->args[8].val
			);
			break;
		}
		case 10: {
			fn->ret.val = fn->std ? ((milo_fn_std_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val, fn->args[4].val,
				fn->args[5].val, fn->args[6].val, fn->args[7].val, fn->args[8].val, fn->args[9].val
			) : ((milo_fn_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val, fn->args[4].val,
				fn->args[5].val, fn->args[6].val, fn->args[7].val, fn->args[8].val, fn->args[9].val
			);
			break;
		}
		case 11: {
			fn->ret.val = fn->std ? ((milo_fn_std_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val, fn->args[4].val,
				fn->args[5].val, fn->args[6].val, fn->args[7].val, fn->args[8].val, fn->args[9].val, fn->args[10].val
			) : ((milo_fn_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val, fn->args[4].val,
				fn->args[5].val, fn->args[6].val, fn->args[7].val, fn->args[8].val, fn->args[9].val, fn->args[10].val
			);
			break;
		}
		case 12: {
			fn->ret.val = fn->std ? ((milo_fn_std_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val, fn->args[4].val, fn->args[5].val,
				fn->args[6].val, fn->args[7].val, fn->args[8].val, fn->args[9].val, fn->args[10].val, fn->args[11].val
			) : ((milo_fn_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val, fn->args[4].val, fn->args[5].val,
				fn->args[6].val, fn->args[7].val, fn->args[8].val, fn->args[9].val, fn->args[10].val, fn->args[11].val
			);
			break;
		}
		case 13: {
			fn->ret.val = fn->std ? ((milo_fn_std_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val, fn->args[4].val, fn->args[5].val,
				fn->args[6].val, fn->args[7].val, fn->args[8].val, fn->args[9].val, fn->args[10].val, fn->args[11].val, fn->args[12].val
			) : ((milo_fn_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val, fn->args[4].val, fn->args[5].val,
				fn->args[6].val, fn->args[7].val, fn->args[8].val, fn->args[9].val, fn->args[10].val, fn->args[11].val, fn->args[12].val
			);
			break;
		}
		case 14: {
			fn->ret.val = fn->std ? ((milo_fn_std_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val, fn->args[4].val, fn->args[5].val, fn->args[6].val, 
				fn->args[7].val, fn->args[8].val, fn->args[9].val, fn->args[10].val, fn->args[11].val, fn->args[12].val, fn->args[13].val
			) : ((milo_fn_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val, fn->args[4].val, fn->args[5].val, fn->args[6].val,
				fn->args[7].val, fn->args[8].val, fn->args[9].val, fn->args[10].val, fn->args[11].val, fn->args[12].val, fn->args[13].val
			);
			break;
		}
		case 15: {
			fn->ret.val = fn->std ? ((milo_fn_std_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val, fn->args[4].val, fn->args[5].val, fn->args[6].val,
				fn->args[7].val, fn->args[8].val, fn->args[9].val, fn->args[10].val, fn->args[11].val, fn->args[12].val, fn->args[13].val, fn->args[14].val
			) : ((milo_fn_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val, fn->args[4].val, fn->args[5].val, fn->args[6].val,
				fn->args[7].val, fn->args[8].val, fn->args[9].val, fn->args[10].val, fn->args[11].val, fn->args[12].val, fn->args[13].val, fn->args[14].val
			);
			break;
		}
		case 16: {
			fn->ret.val = fn->std ? ((milo_fn_std_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val, fn->args[4].val, fn->args[5].val, fn->args[6].val, fn->args[7].val,
				fn->args[8].val, fn->args[9].val, fn->args[10].val, fn->args[11].val, fn->args[12].val, fn->args[13].val, fn->args[14].val, fn->args[15].val
			) : ((milo_fn_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val, fn->args[4].val, fn->args[5].val, fn->args[6].val, fn->args[7].val,
				fn->args[8].val, fn->args[9].val, fn->args[10].val, fn->args[11].val, fn->args[12].val, fn->args[13].val, fn->args[14].val, fn->args[15].val
			);
			break;
		}
		case 17: {
			fn->ret.val = fn->std ? ((milo_fn_std_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val, fn->args[4].val, fn->args[5].val, fn->args[6].val, fn->args[7].val,
				fn->args[8].val, fn->args[9].val, fn->args[10].val, fn->args[11].val, fn->args[12].val, fn->args[13].val, fn->args[14].val, fn->args[15].val,
				fn->args[16].val
			) : ((milo_fn_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val, fn->args[4].val, fn->args[5].val, fn->args[6].val, fn->args[7].val,
				fn->args[8].val, fn->args[9].val, fn->args[10].val, fn->args[11].val, fn->args[12].val, fn->args[13].val, fn->args[14].val, fn->args[15].val,
				fn->args[16].val
			);
			break;
		}
		case 18: {
			fn->ret.val = fn->std ? ((milo_fn_std_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val, fn->args[4].val, fn->args[5].val, fn->args[6].val, fn->args[7].val,
				fn->args[8].val, fn->args[9].val, fn->args[10].val, fn->args[11].val, fn->args[12].val, fn->args[13].val, fn->args[14].val, fn->args[15].val,
				fn->args[16].val, fn->args[17].val
			) : ((milo_fn_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val, fn->args[4].val, fn->args[5].val, fn->args[6].val, fn->args[7].val,
				fn->args[8].val, fn->args[9].val, fn->args[10].val, fn->args[11].val, fn->args[12].val, fn->args[13].val, fn->args[14].val, fn->args[15].val,
				fn->args[16].val, fn->args[17].val
			);
			break;
		}
		case 19: {
			fn->ret.val = fn->std ? ((milo_fn_std_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val, fn->args[4].val, fn->args[5].val, fn->args[6].val, fn->args[7].val,
				fn->args[8].val, fn->args[9].val, fn->args[10].val, fn->args[11].val, fn->args[12].val, fn->args[13].val, fn->args[14].val, fn->args[15].val,
				fn->args[16].val, fn->args[17].val, fn->args[18].val
			) : ((milo_fn_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val, fn->args[4].val, fn->args[5].val, fn->args[6].val, fn->args[7].val,
				fn->args[8].val, fn->args[9].val, fn->args[10].val, fn->args[11].val, fn->args[12].val, fn->args[13].val, fn->args[14].val, fn->args[15].val,
				fn->args[16].val, fn->args[17].val, fn->args[18].val
			);
			break;
		}
		case 20: {
			fn->ret.val = fn->std ? ((milo_fn_std_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val, fn->args[4].val, fn->args[5].val, fn->args[6].val, fn->args[7].val,
				fn->args[8].val, fn->args[9].val, fn->args[10].val, fn->args[11].val, fn->args[12].val, fn->args[13].val, fn->args[14].val, fn->args[15].val,
				fn->args[16].val, fn->args[17].val, fn->args[18].val, fn->args[19].val
			) : ((milo_fn_t)fn->addr)(
				fn->args[0].val, fn->args[1].val, fn->args[2].val, fn->args[3].val, fn->args[4].val, fn->args[5].val, fn->args[6].val, fn->args[7].val,
				fn->args[8].val, fn->args[9].val, fn->args[10].val, fn->args[11].val, fn->args[12].val, fn->args[13].val, fn->args[14].val, fn->args[15].val,
				fn->args[16].val, fn->args[17].val, fn->args[18].val, fn->args[19].val
			);
			break;
		}
	}

	return milo_return(L, &fn->ret);
}

int milo_func_index(lua_State *L)
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

int milo_load(lua_State *L)
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

int milo_lib_index(lua_State *L)
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

	int milo_test(lua_State *L);

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