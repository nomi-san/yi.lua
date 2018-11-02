#ifndef MILO_TYPES_H
#define MILO_TYPES_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include "lauxlib.h"

typedef unsigned char		c_u8_t,		c_bool_t;
typedef unsigned short		c_u16_t;
typedef unsigned int		c_u32_t,	c_uint_t;
typedef unsigned long long	c_u64_t,	c_uint64_t;

typedef signed char			c_i8_t;
typedef signed short		c_i16_t;
typedef signed int			c_i32_t,	c_int_t;
typedef signed long long	c_i64_t,	c_int64_t;

typedef unsigned short		c_wchar_t;
typedef char				c_char_t;
typedef char*				c_str_t;
typedef unsigned short*		c_wstr_t;

typedef float				c_f32_t;
typedef double				c_f64_t,	c_num_t;

typedef void*				c_ptr_t;

#define nil ((void*)0)

typedef struct {
	signed char		__i8;
	signed short	__i16;
	signed int		__i32;
	signed long long 
					__i64;

	unsigned char	__u8;
	unsigned short	__u16;
	unsigned int	__u32;
	unsigned long long 
					__u64;

	char			__char;

	float			__f32;
	double			__f64;
	long double		__num;

	char*			__str;
	unsigned short*	__wstr;
	void*			__ptr;
	void*			__args[];
} milo_not_union_t;

typedef enum {
	c_u8, c_u16, c_u32, c_u64,
	c_i8, c_i16, c_i32, c_i64,
	c_f32, c_f64,
	c_bool, c_char, c_wchar,
	c_int, c_int64, c_uint, c_uint64, c_num,
	c_str, c_wstr,
	c_none, c_ptr
} milo_type_t;

typedef union {
	c_u8_t 		u8_v;
	c_u16_t 	u16_v;
	c_u32_t 	u32_v;
	c_u64_t 	u64_v;
	c_i8_t 		i8_v;
	c_i16_t 	i16_v;
	c_i32_t 	i32_v;
	c_i64_t 	i64_v;
	c_f32_t 	f32_v;
	c_f64_t 	f64_v;
	c_num_t 	num_v;
	c_bool_t 	bool_v;
	c_char_t 	char_v;
	c_wchar_t 	wchar_v;
	c_int_t 	int_v;
	c_int64_t 	int64_v;
	c_uint_t 	uint_v;
	c_uint64_t 	uint64_v;
	
	c_str_t 	str_v;
	c_wstr_t	wstr_v;
	c_ptr_t 	ptr_v;
} milo_variant_t;

typedef struct {
	milo_type_t type;
	size_t offset;
} milo_arg_t;

typedef struct milo_value_t {
	milo_type_t	t;
	void* val;
} milo_value_t;

typedef struct {
	char*			name;
	void*			addr;
	unsigned char	std;
	unsigned short	nargs;
	
	milo_type_t type;

	milo_variant_t	ret;

	milo_arg_t	*args;
	size_t size;

} milo_func_t;

typedef struct {
	char*	name;
	void*	addr;
} milo_lib_t;

static const char* __milo_lib__ = "lib";
static const char* __milo_func__ = "func";

static const char
	*c_u8_n 	= "u8", 
	*c_u16_n 	= "u16",
	*c_u32_n 	= "u32",
	*c_u64_n 	= "u64",
	*c_i8_n 	= "i8",
	*c_i16_n 	= "i16",
	*c_i32_n 	= "i32",
	*c_i64_n 	= "i64",
	*c_f32_n 	= "f32",
	*c_f64_n 	= "f64",
	*c_bool_n 	= "bool",
	*c_char_n 	= "char",
	*c_wchar_n 	= "wchar",
	*c_int_n 	= "int",
	*c_int64_n 	= "int64",
	*c_uint_n 	= "uint",
	*c_uint64_n = "uint64",
	*c_num_n 	= "num",
	*c_str_n 	= "str",
	*c_wstr_n 	= "wstr",
	*c_none_n 	= "none",
	*c_ptr_n 	= "ptr";

const char* milo_type_tostring(milo_type_t type);
size_t milo_type_tosize(milo_type_t type);

int milo_pushvariant(lua_State *L, milo_variant_t val, milo_type_t type);

#ifdef __cplusplus
}
#endif

#endif