#ifndef MILO_TYPES_H
#define MILO_TYPES_H
#pragma once
#include <stdlib.h>

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

typedef enum {
	c_u8, c_u16, c_u32, c_u64,
	c_i8, c_i16, c_i32, c_i64,
	c_f32, c_f64,
	c_bool, c_char, c_wchar,
	c_int, c_int64, c_uint, c_uint64, c_num,
	c_str, c_wstr,
	c_none, c_ptr
} milo_type_t;

typedef void* (*milo_fn_t)();
typedef void* (__stdcall* milo_fn_std_t)();

typedef struct milo_value_t {
	milo_type_t	t;
	void* val;
} milo_value_t;

typedef struct {
	char*			name;
	void*			addr;
	unsigned char	std;
	unsigned short	n;
	milo_value_t	ret;
	milo_value_t*	args;
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

#endif