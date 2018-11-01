#include "struct.h"

typedef struct {

	int a;

} milo_func_arg_t;

typedef struct {
	void* addr;
	void* name;

	unsigned char std;
	void* params;



} milo_func_t_;

typedef struct {
	void *addr;
	char* name;
} milo_lib_t_;


int milo_load_(lua_State *L)
{
	milo_lib_t* lib = alloc_t(milo_lib_t);
	const char* name = lua_tostring(L, 1);

	lib->addr = LoadLibraryA(name);

	if (!lib->addr)
		return 0;
	else
		lib->name = _strdup(name);

	milo_pushudata(L, lib, __milo_lib__);
	return 1;
}

int milo_lib_call(lua_State *L)
{

}