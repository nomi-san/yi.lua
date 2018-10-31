#include "milo.h"
#include "types.h"

typedef struct {
	milo_type_t type;
	void*		val;
} milo_closure_arg_t;

typedef struct {
	int ref;
	size_t index;
	size_t n;
	milo_type_t type;
	void*		ret;

	milo_closure_arg_t*
				args;
} milo_closure_t;

static milo_closure_t** milo_closure_list;

void* milo_closure_proxy(void* args[])
{

	return nil;
}

void* __stdcall milo_closure_proxy_std(void* args[])
{

	return nil;
}