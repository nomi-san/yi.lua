#ifndef _yi_types_
#define _yi_types_
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

static char $__yi_bool[]	= "bool";
static char $__yi_i8[]		= "i8";
static char $__yi_i16[]	= "i16";
static char $__yi_i32[]	= "i32";
static char $__yi_i64[]	= "i64";
static char $__yi_u8[]		= "u8";
static char $__yi_u16[]	= "u16";
static char $__yi_u32[]	= "u32";
static char $__yi_u64[]	= "u64";
static char $__yi_f32[]	= "f32";
static char $__yi_f64[]	= "f64";
static char $__yi_ptr[]	= "ptr";
static char $__yi_str[]	= "str";
static char $__yi_wstr[]	= "wstr";
static char $__yi_void[]	= "void";
static char $__yi_size[]	= "size";

typedef int8_t		yiT_i8,	yiT_chr,	yiT_char;
typedef int16_t		yiT_i16;
typedef int32_t		yiT_i32,	yiT_int,	yiT_int32;
typedef int64_t		yiT_i64,	yiT_int64;

typedef uint8_t		yiT_u8,	yiT_bool,	yiT_boolean;
typedef uint16_t	yiT_u16,	yiT_wchr,	yiT_wchar;
typedef uint32_t	yiT_u32,	yiT_uint,	yiT_uint32;
typedef uint64_t	yiT_u64,	yiT_uint64;

typedef int8_t		*yiT_str,	*yiT_string;
typedef uint16_t	*yiT_wstr, *yiT_wstring;
typedef void		*yiT_ptr,	*yiT_pointer;

typedef float		yiT_f32,	yiT_float;
typedef double		yiT_f64,	yiT_num;

typedef size_t		yiT_size;

typedef void		yiT_none,	yiT_void;

#define yiT_nil	((void*)0)

typedef enum {

	yiT_NIL = -2,
	yiT_NONE, yiT_VOID = yiT_NONE,

	yiT_BOOL = 1, yiT_BOOLEAN = yiT_BOOL,

	yiT_I8,	yiT_CHR = yiT_I8,		yiT_CHAR = yiT_I8,
	yiT_I16,
	yiT_I32,	yiT_INT = yiT_I32,	yiT_INT32 = yiT_I32,
	yiT_I64,	yiT_INT64 = yiT_I64,

	yiT_U8,
	yiT_U16,	yiT_WCHR = yiT_U16,	yiT_WCHAR = yiT_U16,
	yiT_U32,	yiT_UINT = yiT_U32,	yiT_UINT32 = yiT_U32,
	yiT_U64,	yiT_UINT64 = yiT_U64,

	yiT_F32,	yiT_FLOAT = yiT_F32,
	yiT_F64,	yiT_DOUBLE = yiT_F64, yiT_NUM = yiT_F64,

	yiT_STR,	yiT_STRING = yiT_STR,
	yiT_WSTR,	yiT_WSTRING = yiT_WSTR,
	yiT_PTR,	yiT_POINTER = yiT_PTR,
	yiT_SIZE

} yi_Type;

typedef struct _yi_Arg {

	yi_Type	type;
	uint32_t	offset;

} yi_Arg;

typedef struct _yi_Closure {

	void		*addr;
	int			ref;
	lua_State	*state;
	yi_Type	type;
	uint32_t	nargs;
	yi_Arg		*args;

} yi_Closure;

typedef struct _yi_String {

	char		*str;
	bool		ist;
	uint32_t	hash;

} yi_String;

typedef struct _yi_Element {

	yi_String	*name;
	yi_Type	type;
	uint32_t	size;
	uint32_t	offset;

} yi_Element;

typedef struct _yi_Struct {

	void		*addr;
	char		*name;
	uint32_t	size;
	uint32_t	len;
	yi_Element *elms;

	bool		changed;

} yi_Struct;

typedef enum _yi_ObjType {

	yi_OBLOCK = 0,
	yi_OSTRING,
	yi_OCFUN,
	yi_OCLIB,
	yi_OTHREAD

} yi_ObjType;

uint32_t yi_sizeofType(yi_Type type);
const char *yi_typeofType(yi_Type type);
void yi_setArg(lua_State *L, void *args, yi_Arg *arg, int idx);
void yi_pushArg(lua_State *L, void *args, yi_Arg *arg);
void yi_setElement(lua_State *L, void *args, yi_Element *elm, int idx);
void yi_pushElement(lua_State *L, void *args, yi_Element *elm);

#ifdef __cplusplus
}
#endif

#endif // !_yi_types_