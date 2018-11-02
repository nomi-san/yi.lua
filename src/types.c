#include "types.h"
#include "lauxlib.h"

const char* milo_type_tostring(milo_type_t type)
{
	switch(type)
	{
		case c_u8: 		return c_u8_n;
		case c_u16: 	return c_u16_n;
		case c_u32: 	return c_u32_n;
		case c_u64: 	return c_u64_n;
		case c_i8: 		return c_i8_n;
		case c_i16: 	return c_i16_n;
		case c_i32: 	return c_i32_n;
		case c_i64: 	return c_i64_n;
		case c_f32: 	return c_f32_n;
		case c_f64: 	return c_f64_n;
		case c_bool: 	return c_bool_n;
		case c_char: 	return c_char_n;
		case c_wchar: 	return c_wchar_n;
		case c_int: 	return c_int_n;
		case c_int64: 	return c_int64_n;
		case c_uint: 	return c_uint_n;
		case c_uint64: 	return c_uint64_n;
		case c_num: 	return c_num_n;
		case c_str: 	return c_str_n;
		case c_wstr: 	return c_wstr_n;
		case c_none: 	return c_none_n;
		case c_ptr: 	return c_ptr_n;
	}
	return nil;
}

size_t milo_type_tosize(milo_type_t type)
{
	switch (type)
	{
		case c_u8: 		return sizeof(c_u8_t);
		case c_u16: 	return sizeof(c_u16_t);
		case c_u32: 	return sizeof(c_u32_t);
		case c_u64: 	return sizeof(c_u64_t);
		case c_i8: 		return sizeof(c_i8_t);
		case c_i16: 	return sizeof(c_i16_t);
		case c_i32: 	return sizeof(c_i32_t);
		case c_i64: 	return sizeof(c_i64_t);
		case c_f32: 	return sizeof(c_f32_t);
		case c_f64: 	return sizeof(c_f64_t);
		case c_bool: 	return sizeof(c_bool_t);
		case c_char: 	return sizeof(c_char_t);
		case c_wchar: 	return sizeof(c_wchar_t);
		case c_int: 	return sizeof(c_int_t);
		case c_int64: 	return sizeof(c_int64_t);
		case c_uint: 	return sizeof(c_uint_t);
		case c_uint64: 	return sizeof(c_uint64_t);
		case c_num: 	return sizeof(c_num_t);
		case c_str: 	return sizeof(c_str_t);
		case c_wstr: 	return sizeof(c_wstr_t);
		case c_ptr:		return sizeof(c_ptr_t);
	}
	return 0;
}

int milo_pushvariant(lua_State *L, milo_variant_t val, milo_type_t type)
{
	switch (type) 
	{
		case c_bool: 	lua_pushboolean(L, val.bool_v);	break;

		case c_u8: 		lua_pushinteger(L, val.u8_v);		break;
		case c_u16: 	lua_pushinteger(L, val.u16_v);		break;
		case c_u32: 	lua_pushboolean(L, val.u32_v);		break;
		case c_u64: 	lua_pushboolean(L, val.u64_v);		break;

		case c_uint: 	lua_pushinteger(L, val.u32_v);		break;
		case c_uint64:	lua_pushinteger(L, val.u64_v);		break;

		case c_i8: 		lua_pushinteger(L, val.i8_v);		break;
		case c_i16: 	lua_pushinteger(L, val.i16_v);		break;
		case c_i32: 	lua_pushinteger(L, val.i32_v);		break;
		case c_i64: 	lua_pushinteger(L, val.i64_v);		break;

		case c_int: 	lua_pushinteger(L, val.int_v);		break;
		case c_int64: 	lua_pushinteger(L, val.int64_v);	break;

		case c_f32: 	lua_pushnumber(L, val.f32_v);		break;
		case c_f64: 	lua_pushnumber(L, val.f64_v);		break;
		case c_num: 	lua_pushnumber(L, val.num_v);		break;
	
		case c_str: 	lua_pushstring(L, val.str_v);			break;
		case c_wstr: 	lua_pushwstring(L, val.wstr_v);			break;
		case c_ptr:		lua_pushlightuserdata(L, val.ptr_v);	break;

		//case c_char: 	lua_pushinteger(L, *(c_char_t*)val);	break;
		//case c_wchar: 	lua_pushinteger(L, *(c_wchar_t*)val);	break;

		default: return 0;
	}

	return 1;
}