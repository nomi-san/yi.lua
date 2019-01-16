#include "yi.h"
#include "inc/clofn.h"
#include <stdarg.h>

static void __yi_pushvalist(lua_State *L, va_list *ap, yi_Type type);

def_clofn(int64_t, yi_closure_I, size_t, data, (void *args, ...),
{
	yi_Closure *cl = (yi_Closure*)data;
	lua_State *L = cl->state;

	int64_t ret = 0;

	lua_rawgeti(L, LUA_REGISTRYINDEX, cl->ref);
	if (!lua_isfunction(L, -1))
		return ret;

	va_list ap;
	va_start(ap, args);

	yi_pushArg(L, &args, &cl->args[0]);
	for (int i = 1; i < cl->nargs; i++)
		__yi_pushvalist(L, &ap, cl->args[i].type);

	va_end(ap);

	//for (int i = 0; i < cl->nargs; i++)
	//	yi_pushArg(L, &args, &cl->args[i]);

	lua_pcall(L, cl->nargs, 1, 0);
	ret = lua_tointeger(L, -1);
	sizeof(WPARAM);
	return ret;
})

def_clofn(uint64_t, yi_closure_U, size_t, data, (void *args, ...),
{
	yi_Closure *cl = (yi_Closure*)data;
	lua_State *L = cl->state;

	uint64_t ret = 0;

	lua_rawgeti(L, LUA_REGISTRYINDEX, cl->ref);
	if (!lua_isfunction(L, -1))
		return ret;

	va_list ap;
	va_start(ap, args);

	yi_pushArg(L, &args, &cl->args[0]);
	for (int i = 1; i < cl->nargs; i++)
		__yi_pushvalist(L, &ap, cl->args[i].type);

	va_end(ap);

	//for (int i = 0; i < cl->nargs; i++)
	//	yi_pushArg(L, &args, &cl->args[i]);

	lua_pcall(L, cl->nargs, 1, 0);
	ret = lua_tointeger(L, -1);

	return ret;
})

def_clofn(float, yi_closure_F32, size_t, data, (void *args, ...),
{
	yi_Closure *cl = (yi_Closure*)data;
	lua_State *L = cl->state;

	float ret = 0.f;

	lua_rawgeti(L, LUA_REGISTRYINDEX, cl->ref);
	if (!lua_isfunction(L, -1))
		return ret;

	va_list ap;
	va_start(ap, args);

	yi_pushArg(L, &args, &cl->args[0]);
	for (int i = 1; i < cl->nargs; i++)
		__yi_pushvalist(L, &ap, cl->args[i].type);

	va_end(ap);

	//for (int i = 0; i < cl->nargs; i++)
	//	yi_pushArg(L, &args, &cl->args[i]);

	lua_pcall(L, cl->nargs, 1, 0);
	ret = lua_tonumber(L, -1);

	return ret;
})

def_clofn(double, yi_closure_F64, size_t, data, (void *args, ...),
{
	yi_Closure *cl = (yi_Closure*)data;
	lua_State *L = cl->state;

	double ret = 0.0;

	lua_rawgeti(L, LUA_REGISTRYINDEX, cl->ref);
	if (!lua_isfunction(L, -1))
		return ret;

	va_list ap;
	va_start(ap, args);

	yi_pushArg(L, &args, &cl->args[0]);
	for (int i = 1; i < cl->nargs; i++)
		__yi_pushvalist(L, &ap, cl->args[i].type);

	va_end(ap);

	//for (int i = 0; i < cl->nargs; i++)
	//	yi_pushArg(L, &args, &cl->args[i]);

	lua_pcall(L, cl->nargs, 1, 0);
	ret = lua_tonumber(L, -1);

	return ret;
})

def_clofn(void, yi_closure, size_t, data, (void *args, ...),
{
	yi_Closure *cl = (yi_Closure*)data;
	lua_State *L = cl->state;

	lua_rawgeti(L, LUA_REGISTRYINDEX, cl->ref);
	if (!lua_isfunction(L, -1))
		return;

	va_list ap;
	va_start(ap, args);

	yi_pushArg(L, &args, &cl->args[0]);
	for (int i = 1; i < cl->nargs; i++)
		__yi_pushvalist(L, &ap, cl->args[i].type);

	va_end(ap);

	//for (int i = 0; i < cl->nargs; i++)
	//	yi_pushArg(L, &args, &cl->args[i]);

	lua_pcall(L, cl->nargs, 0, 0);
})

int yi_closure(lua_State *L)
{
	yi_Closure *cl = malloc(sizeof(yi_Closure));
	if (cl == NULL) return 0;

	cl->state = L;

	lua_pushvalue(L, 1);
	cl->ref = luaL_ref(L, LUA_REGISTRYINDEX);
	cl->type = lua_tointeger(L, 2);
	cl->nargs = lua_istable(L, 3) ? lua_rawlen(L, 3) : 0;
	cl->args = NULL;

	if (cl->nargs > 0)
		cl->args = malloc(sizeof(yi_Arg) * cl->nargs);

	uint32_t size = 0;

	for (int i = 0; i < cl->nargs; i++) {
		lua_rawgeti(L, 3, i + 1);
		yi_Type type = lua_tointeger(L, -1);

		cl->args[i].offset = size;
		cl->args[i].type = type;

		size += //yi_sizeofType(type);
#if defined(yi_X64)
		sizeof(void*);
#else
		yi_sizeofType(type) > sizeof(int) ? sizeof(double) : sizeof(int);
#endif	

	}

	switch (cl->type) {
		case yiT_I8: case yiT_I16: case yiT_I32: case yiT_I64:
			cl->addr = new_clofn(yi_closure_I, cl);
			break;

		case yiT_U8: case yiT_U16: case yiT_U32: case yiT_U64:
		case yiT_STR: case yiT_WSTR: case yiT_PTR: case yiT_SIZE:
			cl->addr = new_clofn(yi_closure_U, cl);
			break;

		case yiT_F32:
			cl->addr = new_clofn(yi_closure_F32, cl);
			break;

		case yiT_F64:
			cl->addr = new_clofn(yi_closure_F32, cl);
			break;

		case yiT_NONE:
		default:
			cl->addr = new_clofn(yi_closure, cl);
			break;
	}

	yiL_pushudata(L, cl, $__yi_closure);
	return 1;
}

static int yi_closure__gc(lua_State *L)
{
	yi_Closure *cl = yiL_toudata(L, 1);
	if (!cl) return 0;

	luaL_unref(L, LUA_REGISTRYINDEX, cl->ref);

	if (cl->args != NULL)
		free(cl->args);

	free(cl->addr);
	free(cl);
}

static int yi_closure__call(lua_State *L)
{
	yi_Closure *cl = yiL_toudata(L, 1);

	lua_rawgeti(L, LUA_REGISTRYINDEX, cl->ref);
	if (!lua_isfunction(L, -1)) return 0;

	for (int i = 0; i < cl->nargs; i++)
		lua_pushvalue(L, i + 2);

	if (cl->type != yiT_NONE) {
		lua_call(L, cl->nargs, 1);
		lua_pushvalue(L, -1);
	}
	else {
		lua_call(L, cl->nargs, 0);
		lua_pushnil(L);
	}

	return 1;
}

static int yi_closure__len(lua_State *L)
{
	yi_Closure *cl = yiL_toudata(L, 1);
	if (!cl) return 0;

	lua_pushinteger(L, cl->nargs);
	return 1;
}

static int yi_closure__unm(lua_State *L)
{
	yi_Closure *cl = yiL_toudata(L, 1);
	if (!cl) return 0;

	lua_pushlightuserdata(L, cl->addr);
	return 1;
}

static int yi_closure__tostring(lua_State *L)
{
	yi_Closure *cl = yiL_toudata(L, 1);
	if (!cl) return 0;

	char buf[MAX_PATH];
	char *str = buf;

	str += sprintf(str, "%s: %p | func(", $__yi_closure, cl->addr);
	
	if (cl->nargs <= 0)
		str += sprintf(str, "%s", $__yi_void);
	else
		for (int i = 0; i < cl->nargs; i++) {
			const char *type = yi_typeofType(cl->args[i].type);

			if (i == cl->nargs - 1)
				str += sprintf(str, "%s", type);
			else
				str += sprintf(str, "%s, ", type);
		}

	str += sprintf(str, ") -> %s", yi_typeofType(cl->type));

	lua_pushstring(L, buf);
	return 1;
}

static const luaL_Reg yi_closure__meta[] = 
{
	{ "__call",		yi_closure__call },
	{ "__len",		yi_closure__len },
	{ "__unm",		yi_closure__unm },
	{ "__tostring",	yi_closure__tostring },

	{ NULL, NULL }
};

void yi_closure_reg(lua_State *L)
{
	luaL_newmetatable(L, $__yi_closure);
	luaL_setfuncs(L, yi_closure__meta, 0);
	lua_pop(L, 1);
}

#define __setU(L, u64) \
	lua_lock(L); \
	{ TValue *io = (L->top); val_(io).i = (u64); settt_(io, LUA_TNUMINT); } \
	api_incr_top(L); \
	lua_unlock(L);

static void __yi_pushvalist(lua_State *L, va_list *ap, yi_Type type)
{
	switch (type)
	{
		case yiT_BOOL:
			lua_pushboolean(L, va_arg(*ap, yiT_bool));
			break;

		case yiT_U8:
			lua_pushinteger(L, va_arg(*ap, yiT_u8));
			break;

		case yiT_U16:
			lua_pushinteger(L, va_arg(*ap, yiT_u16));
			break;

		case yiT_U32:
			__setU(L, va_arg(*ap, yiT_u32));
			break;

		case yiT_U64: 
			__setU(L, va_arg(*ap, yiT_u64));
			break;
		
		case yiT_SIZE:
			__setU(L, va_arg(*ap, yiT_size));
			break;

		case yiT_I8:
			lua_pushinteger(L, va_arg(*ap, yiT_i8));
			break;

		case yiT_I16:
			lua_pushinteger(L, va_arg(*ap, yiT_i16));
			break;

		case yiT_I32:
			lua_pushinteger(L, va_arg(*ap, yiT_i32));
			break;

		case yiT_I64:
			lua_pushinteger(L, va_arg(*ap, yiT_i64));
			break;

		case yiT_F32:
			lua_pushnumber(L, va_arg(*ap, yiT_f32));
			break;

		case yiT_F64:
			lua_pushnumber(L, va_arg(*ap, yiT_f64));
			break;

		case yiT_WSTR:
			yiL_pushwstr(L, va_arg(*ap, yiT_wstr));
			break;

		case yiT_STR:
			lua_pushstring(L, va_arg(*ap, yiT_str));
			break;

		case yiT_PTR:
			lua_pushlightuserdata(L, va_arg(*ap, yiT_ptr));
			break;

		case yiT_NONE:
		default:
			lua_pushnil(L);
			break;
	}
}