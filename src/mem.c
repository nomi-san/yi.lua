#include "yi.h"
#include "thread.h"

typedef struct _yi_Mem {

	yi_ObjType type;
	void		*ptr;

	struct _yi_Mem
				*next;

} yi_Mem;

static yi_Mem *yi_Objects = NULL;

void *yi_alloc(size_t size)
{
	void *ptr = malloc(size);
	yi_addMem(ptr, yi_OBLOCK);
	return ptr;
}

void *yi_alloc2(size_t count, size_t size)
{
	void *ptr = calloc(count, size);
	yi_addMem(ptr, yi_OBLOCK);
	return ptr;
}

void *yi_realloc(void *ptr, size_t size)
{
	if (ptr == NULL)
		return yi_alloc(size);

	yi_Mem *mem = yi_Objects;

	while (mem != NULL) {
		yi_Mem *next = mem->next;

		if (mem->ptr == ptr) {
			if (size > 0)
				mem->ptr = realloc(ptr, size);
			else {
				free(ptr);
				mem->ptr = NULL;
			}
			return mem->ptr;
		}

		mem = next;
	}

	return NULL;
}

void yi_free(void *ptr)
{
	yi_realloc(ptr, 0);
}

void yi_addMem(void *ptr, yi_ObjType type)
{
	if (ptr != NULL) {
		yi_Mem *mem = malloc(sizeof(yi_Mem));
		if (mem == NULL) return;
		mem->type = type;
		mem->ptr = ptr;
		mem->next = yi_Objects;
		yi_Objects = mem;
	}
}

void yi_cleanMem()
{
	yi_Mem *mem = yi_Objects;
#if defined(yi_DEBUG)
	uint32_t co = 0;
#endif
	while (mem != NULL) {
		yi_Mem *next = mem->next;

		switch (mem->type)
		{
			case yi_OTHREAD: {
				yi_Thread *thread = mem->ptr;
				lua_State *L = thread->main_state;

				luaL_unref(L, LUA_REGISTRYINDEX, thread->fn);

				if (thread->data)
					luaL_unref(L, LUA_REGISTRYINDEX, thread->data);
				
				free(thread);
				break;
			}

			case yi_OSTRING: {
				yi_String *string = mem->ptr;

				if (!string->ist)
					free(string->str);

				free(string);
				break;
			}

			default:
				if (mem->ptr != NULL)
					free(mem->ptr);
		}

		free(mem);
		mem = next;

#if defined(yi_DEBUG)
		co++;
#endif
	}

	yi_Objects = NULL;
#if defined(yi_DEBUG)
	yi_printf("cleaned %d objects!\n", co);
#endif
}

