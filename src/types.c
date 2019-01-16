#include "yi.h"

static int yi_types__index(lua_State *L)
{
	uint32_t hash;
	const char *idx = yiL_tohstr(L, 2, &hash);

	switch (hash) {
		case hash(bool): case hash(boolean):
			lua_pushinteger(L, yiT_BOOL);
			break;

		case hash(i8): case hash(char): case hash(chr):
			lua_pushinteger(L, yiT_I8);
			break;

		case hash(i16): case hash(short):
			lua_pushinteger(L, yiT_I16);
			break;

		case hash(i32): case hash(int): case hash(int32): case hash(long): case hash(signed):
			lua_pushinteger(L, yiT_I32);
			break;

		case hash(i64): case hash(int64): case hash(longlong): case hash(llong):
			lua_pushinteger(L, yiT_I64);
			break;

		case hash(u8): case hash(uchar): case hash(uchr):
			lua_pushinteger(L, yiT_U8);
			break;

		case hash(u16): case hash(ushort): case hash(wchar): case hash(wchr):
			lua_pushinteger(L, yiT_U16);
			break;

		case hash(u32): case hash(uint32): case hash(uint): case hash(ulong): case hash(unsigned):
			lua_pushinteger(L, yiT_U32);
			break;

		case hash(u64): case hash(uint64): case hash(ulonglong): case hash(ullong):
			lua_pushinteger(L, yiT_U64);
			break;

		case hash(f32): case hash(float):
			lua_pushinteger(L, yiT_F32);
			break;

		case hash(f64): case hash(num): case hash(double): case hash(number):
			lua_pushinteger(L, yiT_F64);
			break;

		case hash(str): case hash(string):
			lua_pushinteger(L, yiT_STR);
			break;

		case hash(wstr): case hash(wstring):
			lua_pushinteger(L, yiT_WSTR);
			break;

		case hash(ptr): case hash(pointer):
			lua_pushinteger(L, yiT_PTR);
			break;

		case hash(size):
			lua_pushinteger(L, yiT_SIZE);
			break;

		case hash(void): case hash(none):
			lua_pushinteger(L, yiT_NONE);
			break;

		default:
			return 0;
	}

	return 1;
}

void yi_types_reg(lua_State *L)
{
	luaL_newmetatable(L, $__yi_types);
	lua_pushcfunction(L, yi_types__index);
	lua_setfield(L, -2, "__index");
	lua_pop(L, 1);

	lua_pushnil(L);
	luaL_setmetatable(L, $__yi_types);

	lua_rawseti(L, -2, 1);
}

uint32_t yi_sizeofType(yi_Type type)
{
	switch (type)
	{
		case yiT_BOOL: case yiT_I8: case yiT_U8:
			return sizeof(yiT_i8);

		case yiT_I16: case yiT_U16:
			return sizeof(yiT_i16);

		case yiT_I32: case yiT_U32:
			return sizeof(yiT_i32);

		case yiT_I64: case yiT_U64:
			return sizeof(yiT_i64);

		case yiT_F32:
			return sizeof(yiT_f32);

		case yiT_F64:
			return sizeof(yiT_f64);

		case yiT_STR: case yiT_WSTR: case yiT_PTR:
			return sizeof(yiT_ptr);

		case yiT_SIZE:
			return sizeof(yiT_size);
	}

	return 0u;
}

uint32_t yi_alignofType(yi_Type type)
{
	switch (type) {
		case yiT_BOOL: case yiT_I8: case yiT_U8:
			return sizeof(yiT_i8);

		case yiT_I16: case yiT_U16:
			return sizeof(yiT_i16);

		case yiT_I32: case yiT_U32:
			return sizeof(yiT_i32);

		case yiT_I64: case yiT_U64:
			return sizeof(yiT_i64);

		case yiT_F32:
			return sizeof(yiT_f32);

		case yiT_F64:
			return sizeof(yiT_f64);

		case yiT_STR: case yiT_WSTR: case yiT_PTR:
			return sizeof(yiT_ptr);

		case yiT_SIZE:
			return sizeof(yiT_size);
	}
}

const char *yi_typeofType(yi_Type type)
{
	switch (type)
	{
		case yiT_BOOL:
			return $__yi_bool;
		case yiT_I8:
			return $__yi_i8;
		case yiT_U8:
			return $__yi_u8;
		case yiT_I16:
			return $__yi_i16;
		case yiT_U16:
			return $__yi_u16;
		case yiT_I32:
			return $__yi_i32;
		case yiT_U32:
			return $__yi_u32;
		case yiT_I64:
			return $__yi_i64;
		case yiT_U64:
			return $__yi_u64;
		case yiT_F32:
			return $__yi_f32;
		case yiT_F64:
			return $__yi_f64;
		case yiT_STR:
			return $__yi_str;
		case yiT_WSTR:
			return $__yi_wstr;
		case yiT_PTR:
			return $__yi_ptr;
		case yiT_SIZE:
			return $__yi_size;
	}

	return $__yi_void;
}

static void __yi_setPtr(lua_State *L, void* addr, yi_Type type, int idx)
{
	switch (type)
	{
		case yiT_BOOL:
			*(yiT_bool*)addr = (yiT_bool)lua_tointeger(L, idx);
			break;

		case yiT_U8:
			*(yiT_u8*)addr = lua_tointeger(L, idx);
			break;
		
		case yiT_U16:
			*(yiT_u16*)addr = lua_tointeger(L, idx);
			break;
		
		case yiT_U32:
			*(yiT_u32*)addr = lua_tointeger(L, idx);
			break;
		
		case yiT_U64:
			*(yiT_u64*)addr = lua_tointeger(L, idx);
			break;

		case yiT_I8:
			*(yiT_i8*)addr = lua_tointeger(L, idx);
			break;
		
		case yiT_I16:
			*(yiT_i16*)addr = lua_tointeger(L, idx);
			break;
		
		case yiT_I32:
			*(yiT_i32*)addr = lua_tointeger(L, idx);
			break;
		
		case yiT_I64:
			*(yiT_i64*)addr = lua_tointeger(L, idx);
			break;

		case yiT_F32:
			*(yiT_f32*)addr = lua_tonumber(L, idx);
			break;

		case yiT_F64:
			*(yiT_f64*)addr = lua_tonumber(L, idx);
			break;

		case yiT_STR: {
			yiT_str str = strdup(lua_tostring(L, idx));
			yi_addMem(str, 0);
			*(yiT_str*)addr = str;
			break;
		}
			
		case yiT_WSTR:
			*(yiT_wstr*)addr = yiL_towstr(L, idx);;
			break;
		
		case yiT_PTR:
			*(yiT_ptr*)addr = lua_touserdata(L, idx);
			break;
		
		case yiT_SIZE:
			*(yiT_size*)addr = lua_tointeger(L, idx);
			break;

		default:
			*(yiT_ptr*)&addr = NULL;
	}
}

#define __setU(L, ptr, t) \
	lua_lock(L); \
	{ TValue *io = (L->top); val_(io).i = (*(t*)ptr); settt_(io, LUA_TNUMINT); } \
	api_incr_top(L); \
	lua_unlock(L);

static void __yi_pushPtr(lua_State *L, void* addr, yi_Type type)
{
	switch (type)
	{
		case yiT_BOOL:
			lua_pushboolean(L, *(yiT_bool*)addr);
			break;

			//lua_pushinteger(L, *(uint64_t*)addr);
			//break;
		case yiT_U8: {
			__setU(L, addr, yiT_u8);
			break;
		}
		
		case yiT_U16: {
			__setU(L, addr, yiT_u16);
			break;
		}
		
		case yiT_U32: {
			__setU(L, addr, yiT_u32);
			break;
		}
		
		case yiT_U64: {
			__setU(L, addr, yiT_u64);
			break;
		}

		case yiT_SIZE: {
			__setU(L, addr, yiT_size);
			break;
		}

		case yiT_I8:
			lua_pushinteger(L, *(yiT_i8*)addr);
			break;

		case yiT_I16:
			lua_pushinteger(L, *(yiT_i16*)addr);
			break;

		case yiT_I32:
			lua_pushinteger(L, *(yiT_i32*)addr);
			break;

		case yiT_I64:
			lua_pushinteger(L, *(yiT_i64*)addr);
			break;

		case yiT_F32:
			lua_pushnumber(L, *(yiT_f32*)addr);
			break;

		case yiT_F64:
			lua_pushnumber(L, *(yiT_f64*)addr);
			break;

		case yiT_STR:
			lua_pushstring(L, *(yiT_str*)addr);
			break;
		
		case yiT_WSTR:
			yiL_pushwstr(L, *(yiT_wstr*)addr);
			break;

		case yiT_PTR:
			lua_pushlightuserdata(L, *(yiT_ptr*)addr);
			break;

		default:
			lua_pushnil(L);
	}
}

void yi_setArg(lua_State *L, void *args, yi_Arg *arg, int idx)
{
	void *addr = (void*)((char*)args + arg->offset);
	__yi_setPtr(L, addr, arg->type, idx);
}

void yi_pushArg(lua_State *L, void *args, yi_Arg *arg)
{
	void *addr = (void*)((char*)args + arg->offset);
	__yi_pushPtr(L, addr, arg->type);
}

void yi_setElement(lua_State *L, void *args, yi_Element *elm, int idx)
{
	void *addr = (void*)((char*)args + elm->offset);
	__yi_setPtr(L, addr, elm->type, idx);
}

void yi_pushElement(lua_State *L, void *args, yi_Element *elm)
{
	void *addr = (void*)((char*)args + elm->offset);
	__yi_pushPtr(L, addr, elm->type);
}