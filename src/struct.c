#include "struct.h"

void milo_struct_setvalue(lua_State *L, void *val, milo_type_t type, int idx)
{
	switch (type) 
	{
		case c_bool: 	*(c_bool_t*)val = lua_toboolean(L, idx);	break;

		case c_u8: 		*(c_u8_t*)val = lua_tointeger(L, idx);		break;
		case c_u16: 	*(c_u16_t*)val = lua_tointeger(L, idx);		break;
		case c_u32: 	*(c_u32_t*)val = lua_tointeger(L, idx);		break;
		case c_u64: 	*(c_u64_t*)val = lua_tointeger(L, idx);		break;

		case c_uint: 	*(c_uint_t*)val = lua_tointeger(L, idx);	break;
		case c_uint64:	*(c_uint64_t*)val = lua_tointeger(L, idx);	break;

		case c_i8: 		*(c_i8_t*)val = lua_tointeger(L, idx);  	break;
		case c_i16: 	*(c_i16_t*)val = lua_tointeger(L, idx);		break;
		case c_i32: 	*(c_i32_t*)val = lua_tointeger(L, idx); 	break;
		case c_i64: 	*(c_i64_t*)val = lua_tointeger(L, idx); 	break;

		case c_int: 	*(c_int_t*)val = lua_tointeger(L, idx); 	break;
		case c_int64: 	*(c_int64_t*)val = lua_tointeger(L, idx); 	break;

		case c_f32: 	*(c_f32_t*)val = lua_tonumber(L, idx);		break;
		case c_f64: 	*(c_f64_t*)val = lua_tonumber(L, idx);		break;
		case c_num: 	*(c_num_t*)val = lua_tonumber(L, idx);		break;
	
		case c_str: 	*(c_str_t*)val = _strdup(lua_tostring(L, idx));	break;
		case c_wstr: 	*(c_wstr_t*)val = 
								str_to_wstr(lua_tostring(L, idx));	break;
		case c_ptr:		*(c_ptr_t*)val = lua_touserdata(L, idx);	break;

		//case c_char: 	return sizeof(c_char_t);
		//case c_wchar: 	return sizeof(c_wchar_t);
	}
}

int milo_struct_pushvalue(lua_State *L, void *val, milo_type_t type)
{
	switch (type) 
	{
		case c_bool: 	lua_pushboolean(L, *(c_bool_t*)val);	break;

		case c_u8: 		lua_pushinteger(L, *(c_u8_t*)val);		break;
		case c_u16: 	lua_pushinteger(L, *(c_u16_t*)val);		break;
		case c_u32: 	lua_pushboolean(L, *(c_u8_t*)val);		break;
		case c_u64: 	lua_pushboolean(L, *(c_u8_t*)val);		break;

		case c_uint: 	lua_pushinteger(L, *(c_uint_t*)val);	break;
		case c_uint64:	lua_pushinteger(L, *(c_uint64_t*)val);	break;

		case c_i8: 		lua_pushinteger(L, *(c_i8_t*)val);		break;
		case c_i16: 	lua_pushinteger(L, *(c_i16_t*)val);		break;
		case c_i32: 	lua_pushinteger(L, *(c_i32_t*)val);		break;
		case c_i64: 	lua_pushinteger(L, *(c_i64_t*)val);		break;

		case c_int: 	lua_pushinteger(L, *(c_int_t*)val);		break;
		case c_int64: 	lua_pushinteger(L, *(c_int64_t*)val);	break;

		case c_f32: 	lua_pushnumber(L, *(c_f32_t*)val);		break;
		case c_f64: 	lua_pushnumber(L, *(c_f64_t*)val);		break;
		case c_num: 	lua_pushnumber(L, *(c_num_t*)val);		break;
	
		case c_str: 	lua_pushstring(L, *(c_str_t*)val);			break;
		case c_wstr: 	lua_pushwstring(L, *(c_wstr_t*)val);		break;
		case c_ptr:		lua_pushlightuserdata(L, *(c_ptr_t*)val);	break;

		//case c_char: 	lua_pushinteger(L, *(c_char_t*)val);	break;
		//case c_wchar: 	lua_pushinteger(L, *(c_wchar_t*)val);	break;

		default: return 0;
	}

	return 1;
}

void *milo_struct_getelement(void* addr, size_t offset)
{
	void* p = (void*)((char*)addr + offset);
	return p;
}

/*
	struct = milo.struct {
		milo.types.int,
		milo.types.int
	}

	-- for sugar syntax

	struct = milo.struct {
		'x', milo.types.int;
		'y', milo.types.int;
	}
*/
int milo_struct(lua_State *L)
{
	if (!lua_istable(L, 1)) return 0;
	size_t len = lua_rawlen(L, 1);

	if (!len) return 0;
	lua_rawgeti(L, 1,  1);

	if (lua_isinteger(L, -1)) {
		lua_pop(L, 1);
		milo_struct_t* st = alloc_t(milo_struct_t);
		st->len = len;
		st->els = alloc(sizeof(milo_struct_element_t) * len);

		for (size_t i = 0; i < len; i++) {
			lua_rawgeti(L, 1, i + 1);
			milo_type_t t = lua_tointeger(L, -1);
			size_t sz = milo_type_tosize(t);

			st->els[i].type = t;
			st->els[i].size = sz;
			st->els[i].offset = st->size;

			st->size += sz;
		}

		st->addr = alloc(st->size);
		milo_pushudata(L, st, __milo_struct__);
		return 1;
	}
	else if (lua_isstring(L, -1)) {
		lua_pop(L, 1);
		if (len % 2 != 0) return 0;

		milo_struct_t* st = alloc_t(milo_struct_t);
		lua_pushvalue(L, 1);
		st->ref = luaL_ref(L, LUA_REGISTRYINDEX);

		st->len = len / 2;
		st->els = alloc(sizeof(milo_struct_element_t) * st->len);

		for (size_t i = 0; i < len; i+=2) {
			lua_rawgeti(L, 1, i + 1);
			const char* n = lua_tostring(L, -1);
			lua_rawgeti(L, 1, i + 2);
			long t = lua_tointeger(L, -1);

			size_t sz = milo_type_tosize(t);

			st->els[i/2].type = t;
			st->els[i/2].size = sz;
			st->els[i/2].offset = st->size;

			st->size += sz;

			lua_pushinteger(L, i/2);
			lua_setfield(L, 1, n);
		}

		st->addr = alloc(st->size);
		milo_pushudata(L, st, __milo_struct__);
		return 1;
	}

	return 0;
}

/*
	struct[0] --> sizeof struct (in byte)
	struct[n] --> element n (n > 0, n <= #struct)

	-- for sugar syntax

	struct.name --> name is string of element's name
*/
int milo_struct_index(lua_State *L)
{
	milo_struct_t* st = milo_getudata(L, 1);

	if (lua_isinteger(L, 2)) {
		int idx = lua_tointeger(L, 2);

		switch (idx) {
			case -1: {
				lua_pushlightuserdata(L, st->addr);
				return 1;
			}
			case 0 :{
				lua_pushinteger(L, st->size);
				return 1;
			}
			default: {
				if (idx > st->len) return 0;

				void* val = milo_struct_getelement(
					st->addr,
					st->els[idx - 1].offset
				);
				return milo_struct_pushvalue(L, val, st->els[idx - 1].type);
			}
		}
	}
	
	else if (lua_isstring(L, 2)) {
		const char* idx = lua_tostring(L, 2);
		lua_rawgeti(L, LUA_REGISTRYINDEX, st->ref);
		if (!lua_getfield(L, -1, idx)) return 0;
		int i = lua_tointeger(L, -1);

		void* val = milo_struct_getelement(
			st->addr,
			st->els[i].offset
		);
		return milo_struct_pushvalue(L, val, st->els[i].type);
	}

	return 0;
}

int milo_struct_newindex(lua_State *L)
{
	milo_struct_t* st = milo_getudata(L, 1);

	if (lua_isinteger(L, 2)) {

		int idx = lua_tointeger(L, 2);

		if (idx == -1) {
			if (lua_isinteger(L, 3)) {
				lua_pushlightuserdata(L, (void*)lua_tointeger(L, 3));
				st->addr = lua_touserdata(L, -1);
			}
			else st->addr = lua_touserdata(L, 3);
			return 0;
		}

		void* val = milo_struct_getelement(
			st->addr,
			st->els[idx - 1].offset
		);
		milo_struct_setvalue(L, val, st->els[idx - 1].type, 3);
	}

	else if (lua_isstring(L, 2)) {
		const char* idx = lua_tostring(L, 2);
		lua_rawgeti(L, LUA_REGISTRYINDEX, st->ref);
		if (!lua_getfield(L, -1, idx)) return 0;
		int i = lua_tointeger(L, -1);

		void* val = milo_struct_getelement(
			st->addr,
			st->els[i].offset
		);
		milo_struct_setvalue(L, val, st->els[i].type, 3);
	}

	return 0;
}

/* 
	#struct --> count of element
*/
int milo_struct_len(lua_State *L)
{
	milo_struct_t* st = milo_getudata(L, 1);
	if (!st) return 0;

	lua_pushinteger(L, st->len);
	return 1;
}

/*
	-struct --> pointer address of struct (as lightudata)
*/
int milo_struct_unm(lua_State *L)
{
	milo_struct_t* st = milo_getudata(L, 1);
	if (!st) return 0;

	lua_pushlightuserdata(L, st->addr);
	return 1;
}

/*
	struct {value_e1, value_e2, ..., value_en}
		--> set value (n <= #struct)
*/
int milo_struct_call(lua_State *L)
{
	milo_struct_t* st = milo_getudata(L, 1);
	if (!st) return 0;
	if (!lua_istable(L, 2)) return 0;

	size_t len = lua_rawlen(L, 2);
	if (len > st->len) len = st->len;
	void* val = nil;

	for (size_t i = 0; i < len; i++) {
		val = milo_struct_getelement(
			st->addr,
			st->els[i].offset
		);
		lua_rawgeti(L, 2, i + 1);
		milo_struct_setvalue(L, val, st->els[i].type, -1);
	}

	return 0;
}

/*
	tostring(struct)
	print(struct)
		--> struct: 0123abcd
*/
int milo_struct_tostring(lua_State *L)
{
	milo_struct_t* st = milo_getudata(L, 1);
	if (!st) return 0;

	char buf[MILO_TS_LEN];
	snprintf(buf, MILO_TS_LEN, "%s: %08x", __milo_struct__, (unsigned int)st->addr);
	lua_pushstring(L, buf);

	return 1;
}

static const luaL_Reg milo_struct_meta[] = {
	{ "__len", milo_struct_len },
	{ "__unm", milo_struct_unm },
	{ "__call", milo_struct_call },
	{ "__tostring", milo_struct_tostring },
	{ "__index", milo_struct_index },
	{ "__newindex", milo_struct_newindex },
	{ NULL, NULL }
};

void milo_struct_reg(lua_State *L)
{
	luaL_newmetatable(L, __milo_struct__);
	luaL_setfuncs(L, milo_struct_meta, 0);
	lua_pop(L, 1);

	lua_pushcfunction(L, milo_struct);
	lua_setfield(L, -2, "struct");
}