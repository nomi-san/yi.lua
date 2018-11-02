#ifndef MILO_STRUCT_H
#define MILO_STRUCT_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "milo.h"
#include "types.h"

typedef struct {
	size_t		size;
	size_t		offset;
	milo_type_t type;
	//char*		name;
} milo_struct_element_t;

typedef struct {
	size_t		size;
	size_t		len;
	//char*		name;
	void*		addr;
	milo_struct_element_t* 
				els;
	int			ref;
} milo_struct_t;

static const char* __milo_struct__ = "struct";

void milo_struct_setvalue(lua_State *L, void *val, milo_type_t type, int idx);
int milo_struct_pushvalue(lua_State *L, void *val, milo_type_t type);
void *milo_struct_getelement(void* addr, size_t offset);

#ifdef __cplusplus
}
#endif

#endif