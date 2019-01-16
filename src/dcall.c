#include "yi.h"

#if defined(yi_WIN32)
#include <malloc.h>
#if defined(_MSC_VER)

#if defined(yi_DEBUG)
#define alloc_stack(size) _alloca(size)
#else
#define alloc_stack(size) _malloca(size)  // instead of _alloca, cannot use it on debug mode
#endif
#define free_stack(ptr) _freea(ptr)
#elif defined(__GNUC__)
#define alloc_stack(size) alloca(size)
#define free_stack(ptr)
#endif

#define dl_open(name) (LoadLibraryA(name))
#define dl_load(lib, name) ((void*)GetProcAddress(lib, name))
#define dl_close(lib) FreeLibrary(lib)
typedef HMODULE dl_handle;
#else
#include <alloca.h>
#define alloc_stack(size) alloca(size)
#define free_stack(ptr)

#define dl_open(name) dlopen(name, RTLD_LAZY)
#define dl_load(lib, name) dlsym(lib, name)
#define dl_close(lib) dlclose(lib)
typedef void *dl_handle;
#endif

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

	long double __9;

};

#if defined(yi_X64)
#define yi_fakeStack(block) (*(struct yi_FakeStackT*)(block))
#else
#define yi_fakeStack(block) (*(struct yi_FakeStackT*)(block))
#endif

typedef union _yi_Value {

	bool		b;

	uint8_t		u8;
	uint16_t	u16;
	uint32_t	u32;
	uint64_t	u64;

	int8_t		i8;
	int16_t		i16;
	int32_t		i32;
	int64_t		i64;

	float		f32;
	double		f64;

	void 		*ptr;
	char		*str;
	wchar_t		*wstr;
	size_t		size;

} yi_Value;

typedef struct _yi_Cfun {

	void		*addr;
	yi_String	*name;
	yi_Type	type;
	uint32_t	size;
	uint32_t	nargs;
	yi_Arg		*args;
#if defined(yi_WIN32)
	bool		std;
#endif
} yi_Cfun;

typedef struct _yi_Clib {

	void		*addr;
	char		*name;

	uint32_t	count;
	yi_Cfun	**funs;

} yi_Clib;

// anonymous / unsafe function

typedef void				(*yi_AUFN)();

typedef long long			(*yi_AUFN_I)();
typedef unsigned long long	(*yi_AUFN_U)();

typedef float				(*yi_AUFN_F32)();
typedef double				(*yi_AUFN_F64)();

// stdcall

#if defined(yi_WIN32)
typedef void				(yi_CLBK *yi_AUFNstd)();

typedef long long			(yi_CLBK *yi_AUFNstd_I)();
typedef unsigned long long	(yi_CLBK *yi_AUFNstd_U)();

typedef float				(yi_CLBK *yi_AUFNstd_F32)();
typedef double				(yi_CLBK *yi_AUFNstd_F64)();
#endif

int yi_check(lua_State *L)
{
	bool ret = 1;

	if (lua_isstring(L, 1)) {

		const char *name = lua_tostring(L, 1);
		const char *fn = lua_tostring(L, 2);

		dl_handle lib = dl_open(name);
		if (!lib) ret = 0;

		void *f = dl_load(lib, fn);
		if (!f) ret = 0;

		lua_pushboolean(L, ret);
		dl_close(lib);
		return 1;
	}

	return 0;
}

int yi_load(lua_State *L)
{
	int nlibs = lua_gettop(L);

	for (int i = 1; i <= nlibs; i++)
	{
		size_t len = 0;
		const char *lib_name = lua_tolstring(L, i, &len);

		dl_handle lib = dl_open(lib_name);

		if (!lib)
			lua_pushnil(L);
		else {
			yi_Clib *clib = malloc(sizeof(yi_Clib));
			clib->addr = lib;
			clib->funs = NULL;
			clib->count = 0;
			
			// parse lib name
			int x;
			for (x = len - 1; x >= 0; --x)
				if (lib_name[x] == '/' || lib_name[x] == '\\') {
					break;
				}

			clib->name = malloc(len + 1);
			clib->name = memcpy(clib->name, lib_name + x + 1, len - x);
			clib->name[len] = '\0'; // break string

			yiL_pushudata(L, clib, $__yi_clib);
		}
	}

	return nlibs;
}

static int yi_clib__index(lua_State *L)
{

	return 0;
}

int a = sizeof(struct { int a; int b; double c; });

static int yi_clib__call(lua_State *L)
{
	yi_Clib *clib = yiL_toudata(L, 1);
	if (!clib) return 0;

	if (lua_isstring(L, 2)) {
		uint32_t hash = 0;
		const char *fn_name = yiL_tohstr(L, 2, &hash);

		void *proc = dl_load(clib->addr, fn_name);
		if (!proc) return 0;

		yi_Cfun *cfun = malloc(sizeof(yi_Cfun));

		cfun->addr = proc;
		cfun->args = NULL;
#if defined(yi_X64)
		cfun->size = 16u;
#else
		cfun->size = 0u;
#endif
		cfun->type = lua_tointeger(L, 3);
		cfun->nargs = lua_rawlen(L, 4);

#if defined(yi_WIN32)
		cfun->std = lua_isnoneornil(L, 5) ? 0 : lua_toboolean(L, 5);
#endif

		cfun->name = malloc(sizeof(yi_String));
		cfun->name->str = strdup(fn_name);
		cfun->name->hash = hash;

		if (cfun->nargs > 0)
			cfun->args = malloc(sizeof(yi_Arg) * cfun->nargs);

		for (int i = 0; i < cfun->nargs; ++i) {
			lua_rawgeti(L, 4, i + 1);

			yi_Type type = lua_tointeger(L, -1);

			cfun->args[i].offset = cfun->size;
			cfun->args[i].type = type;

			cfun->size +=
#if defined(yi_X64)
				sizeof(void*);
#else
				yi_sizeofType(type) > sizeof(int) ? sizeof(double) : sizeof(int);
#endif	
		}

		clib->funs = realloc(clib->funs, sizeof(yi_Cfun*) * (clib->count + 1));
		clib->funs[clib->count++] = cfun;
		yiL_pushudata(L, cfun, $__yi_cfun);
		return 1;
	}

	return 0;
}

static int yi_clib__gc(lua_State *L)
{
	yi_Clib *clib = yiL_toudata(L, 1);
	if (!clib) return 0;

	dl_close(clib->addr);

	if (clib->funs)
		free(clib->funs);

	if (clib->name)
		free(clib->name);

	free(clib);
	return 0;
}

static int yi_clib__len(lua_State *L)
{
	yi_Clib *clib = yiL_toudata(L, 1);
	if (!clib) return 0;

	lua_pushinteger(L, clib->count);
	return 1;
}

static int yi_clib__unm(lua_State *L)
{
	yi_Clib *clib = yiL_toudata(L, 1);
	if (!clib) return 0;

	lua_pushlightuserdata(L, clib);
	return 1;
}

static int yi_clib__tostring(lua_State *L)
{
	yi_Clib *clib = yiL_toudata(L, 1);
	if (!clib) return 0;

	char buf[MAX_PATH];
	sprintf(buf, "%s: %p | %s", $__yi_clib, clib, clib->name);

	lua_pushstring(L, buf);
	return 1;
}

static const luaL_Reg yi_clib__meta[] = {

	{ "__index",	yi_clib__index },
	{ "__call",		yi_clib__call },
	{ "__len",		yi_clib__len },
	{ "__unm",		yi_clib__unm },
	{ "__gc",		yi_clib__gc },
	{ "__tostring", yi_clib__tostring },

	{ NULL, NULL }
};

#if defined(yi_X64)
void __fastcall __yi_x64_caller(int _a, int _b, int _c, int _d, int _e, int _f,
	yi_Cfun *cfun, yi_Value *val, 
	double _01, double _02, double _03, double _04, double _05, double _06, double _07, double _08, double _09, double _10,
	double _11, double _12, double _13, double _14, double _15, double _16, double _17, double _18, double _19, double _20)
{
#if defined(yi_WIN32)
	if (!cfun->std) {
#endif
		switch (cfun->type)
		{
			case yiT_BOOL:
			case yiT_U8: case yiT_U16: case yiT_U32: case yiT_U64:
			case yiT_STR: case yiT_WSTR: case yiT_PTR: case yiT_SIZE:
				*(yiT_u64*)val = ((yi_AUFN_U)cfun->addr)(_01, _02, _03, _04, _05, _06, _07, _08, _09, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20);
				break;

			case yiT_I8: case yiT_I16: case yiT_I32: case yiT_I64:
				*(yiT_i64*)val = ((yi_AUFN_I)cfun->addr)(_01, _02, _03, _04, _05, _06, _07, _08, _09, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20);
				break;

			case yiT_F32:
				*(yiT_f32*)val = ((yi_AUFN_F32)cfun->addr)(_01, _02, _03, _04, _05, _06, _07, _08, _09, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20);
				break;

			case yiT_F64:
				*(yiT_f64*)val = ((yi_AUFN_F64)cfun->addr)(_01, _02, _03, _04, _05, _06, _07, _08, _09, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20);
				break;

			case yiT_VOID:
			default:
				((yi_AUFN)cfun->addr)(_01, _02, _03, _04, _05, _06, _07, _08, _09, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20);
		}
#if defined(yi_WIN32)
	}
	else {
		switch (cfun->type)
		{
			case yiT_BOOL:
			case yiT_U8: case yiT_U16: case yiT_U32: case yiT_U64:
			case yiT_STR: case yiT_WSTR: case yiT_PTR: case yiT_SIZE:
				*(yiT_u64*)val = ((yi_AUFNstd_U)cfun->addr)(_01, _02, _03, _04, _05, _06, _07, _08, _09, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20);
				break;

			case yiT_I8: case yiT_I16: case yiT_I32: case yiT_I64:
				*(yiT_i64*)val = ((yi_AUFNstd_I)cfun->addr)(_01, _02, _03, _04, _05, _06, _07, _08, _09, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20);
				break;

			case yiT_F32:
				*(yiT_f32*)val = ((yi_AUFNstd_F32)cfun->addr)(_01, _02, _03, _04, _05, _06, _07, _08, _09, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20);
				break;

			case yiT_F64:
				*(yiT_f64*)val = ((yi_AUFNstd_F64)cfun->addr)(_01, _02, _03, _04, _05, _06, _07, _08, _09, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20);
				break;

			case yiT_VOID:
			default:
				((yi_AUFNstd)cfun->addr)(_01, _02, _03, _04, _05, _06, _07, _08, _09, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20);
		}
	}
#endif
}
#endif

static int yi_cfun__call(lua_State *L)
{
	yi_Cfun *cfun = yiL_toudata(L, 1);
	if (!cfun) return 0;

	void *fn = cfun->addr;

	int top = lua_gettop(L);
	if (top < 1) return 0;

	if (cfun->nargs != (top - 1)) {
		return 0;
	}
	else {

		// on windows, must use stack instead of heap
		void *args = alloc_stack(cfun->size);
		memset(args, '\0', cfun->size);

		yi_Value ret = { 0 };

#if defined(yi_X64)
		*(yi_Cfun**)((char*)args + 0) = cfun;
		*(yi_Value**)((char*)args + 8) = &ret;
		// lock -> 16
#endif

		for (int i = 0; i < cfun->nargs; i++)
			yi_setArg(L, args, &cfun->args[i], i + 2);

#if defined(yi_X64)
		((void(*)())__yi_x64_caller)(yi_fakeStack(args));
#else
#if defined(yi_WIN32)
		if (!cfun->std) {
#endif
			switch (cfun->type)
			{
				case yiT_BOOL:
				case yiT_U8: case yiT_U16: case yiT_U32: case yiT_U64:
				case yiT_STR: case yiT_WSTR: case yiT_PTR: case yiT_SIZE:
					*(yiT_u64*)&ret = ((yi_AUFN_U)fn)(yi_fakeStack(args));
					break;

				case yiT_I8: case yiT_I16: case yiT_I32: case yiT_I64:
					*(yiT_i64*)&ret = ((yi_AUFN_I)fn)(yi_fakeStack(args));
					break;

				case yiT_F32:
					*(yiT_f32*)&ret = ((yi_AUFN_F32)fn)(yi_fakeStack(args));
					break;

				case yiT_F64:
					*(yiT_f64*)&ret = ((yi_AUFN_F64)fn)(yi_fakeStack(args));
					break;

				case yiT_VOID:
				default:
					((yi_AUFN)fn)(yi_fakeStack(args));
			}
#if defined(yi_WIN32)
		}
		else {
			switch (cfun->type)
			{
				case yiT_BOOL:
				case yiT_U8: case yiT_U16: case yiT_U32: case yiT_U64:
				case yiT_STR: case yiT_WSTR: case yiT_PTR: case yiT_SIZE:
					*(yiT_u64*)&ret = ((yi_AUFNstd_U)fn)(yi_fakeStack(args));
					break;

				case yiT_I8: case yiT_I16: case yiT_I32: case yiT_I64:
					*(yiT_i64*)&ret = ((yi_AUFNstd_I)fn)(yi_fakeStack(args));
					break;

				case yiT_F32:
					*(yiT_f32*)&ret = ((yi_AUFNstd_F32)fn)(yi_fakeStack(args));
					break;

				case yiT_F64:
					*(yiT_f64*)&ret = ((yi_AUFNstd_F64)fn)(yi_fakeStack(args));
					break;

				case yiT_VOID:
				default:
					((yi_AUFNstd)fn)(yi_fakeStack(args));
			}
		}
#endif
#endif
		switch (cfun->type)
		{
			case yiT_BOOL:
				lua_pushboolean(L, ret.b);
				break;

			case yiT_U8:
				lua_pushinteger(L, ret.u8);
				break;

			case yiT_U16:
				lua_pushinteger(L, ret.u16);
				break;

			case yiT_U32:
				lua_pushinteger(L, ret.u32);
				break;

			case yiT_U64:
				lua_pushinteger(L, ret.u64);
				break;

			case yiT_I8:
				lua_pushinteger(L, ret.i8);
				break;
			
			case yiT_I16: 
				lua_pushinteger(L, ret.i16);
				break;

			case yiT_I32:
				lua_pushinteger(L, ret.i32);
				break;
			
			case yiT_I64:
				lua_pushinteger(L, ret.i64);
				break;

			case yiT_F32:
				lua_pushnumber(L, ret.f32);
				break;

			case yiT_F64:
				lua_pushnumber(L, ret.f64);
				break;

			case yiT_STR:
				lua_pushstring(L, ret.str);
				break;

			case yiT_WSTR:
				yiL_pushwstr(L, ret.wstr);
				break;

			case yiT_PTR:
				lua_pushlightuserdata(L, ret.ptr);
				break;

			case yiT_SIZE:
				lua_pushinteger(L, ret.size);
				break;

			case yiT_VOID:
			default:
				return 0;
		}

		free_stack(args);
		return 1;
	}

	return 0;
}

static int yi_cfun__len(lua_State *L)
{
	yi_Cfun *cfun = yiL_toudata(L, 1);
	if (!cfun) return 0;

	lua_pushinteger(L, cfun->nargs);
	return 1;
}

static int yi_cfun__unm(lua_State *L)
{
	yi_Cfun *cfun = yiL_toudata(L, 1);
	if (!cfun) return 0;

	lua_pushlightuserdata(L, cfun);
	return 1;
}

static int yi_cfun__gc(lua_State *L)
{
	yi_Cfun *cfun = yiL_toudata(L, 1);
	if (!cfun) return 0;

	if (cfun->args)
		free(cfun->args);

	if (cfun->name) {
		if (cfun->name->str
			&& !cfun->name->ist)
			free(cfun->name->str);
		free(cfun->name);
	}

	free(cfun);
	return 0;
}

static int yi_cfun__tostring(lua_State *L)
{
	yi_Cfun *cfun = yiL_toudata(L, 1);
	if (!cfun) return 0;

	char buf[MAX_PATH];
	char *str = buf;

	str += sprintf(str, "%s: %p | %s(", $__yi_cfun, cfun->addr, cfun->name->str);

	if (cfun->nargs <= 0)
		str += sprintf(str, "%s", $__yi_void);
	else
		for (int i = 0; i < cfun->nargs; i++) {
			const char *type = yi_typeofType(cfun->args[i].type);

			if (i == cfun->nargs - 1)
				str += sprintf(str, "%s", type);
			else
				str += sprintf(str, "%s, ", type);
		}

#if defined(yi_WIN32)
	if (cfun->std)
		str += sprintf(str, ") -> %s [std]", yi_typeofType(cfun->type));
	else
#endif
	str += sprintf(str, ") -> %s", yi_typeofType(cfun->type));

	lua_pushstring(L, buf);
	return 1;
}

static const luaL_Reg yi_cfun__meta[] = {

	{ "__call",		yi_cfun__call },
	{ "__len",		yi_cfun__len },
	{ "__unm",		yi_cfun__unm },
	{ "__gc",		yi_cfun__gc },
	{ "__tostring", yi_cfun__tostring },

	{ NULL, NULL }
};

void yi_dcall_reg(lua_State *L)
{
	luaL_newmetatable(L, $__yi_clib);
	luaL_setfuncs(L, yi_clib__meta, 0);
	lua_pop(L, 1);

	luaL_newmetatable(L, $__yi_cfun);
	luaL_setfuncs(L, yi_cfun__meta, 0);
	lua_pop(L, 1);
}