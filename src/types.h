#ifndef MILO_TYPES_H
#define MILO_TYPES_H
#pragma once

typedef unsigned char		c_u8_t,		c_bool_t;
typedef unsigned short		c_u16_t;
typedef unsigned int		c_u32_t,	c_uint_t;
typedef unsigned long long	c_u64_t,	c_uint64_t;

typedef signed char			c_i8_t;
typedef signed short		c_i16_t;
typedef signed int			c_i32_t,	c_int_t;
typedef signed long long	c_i64_t,	c_int64_t;

typedef char				c_char_t;
typedef wchar_t				c_wchar_t;
typedef char*				c_str_t;
typedef wchar_t*			c_wstr_t;

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

typedef struct {
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

#endif