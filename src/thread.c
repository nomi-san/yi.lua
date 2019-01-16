#include "thread.h"

static bool thread_create(thread_handle *handle, thread_status (yi_CLBK *routine)(void*), void *data)
{
#if defined(yi_WIN32)
	DWORD id = 0;
	HANDLE _h = CreateThread(NULL, 0, routine, data, 0, &id);
	if (_h == NULL || id == 0) return 0;
	if (handle != NULL) (*handle) = _h;
	return 1;
#else
	int ret = pthread_create(handle, NULL, routine, data);
	if (ret <= 0) return 0;
	return 1;
#endif
}

static thread_status yi_CLBK yi_thread_routine(yi_Thread *thread)
{
	lua_State *L = thread->state;

	lua_rawgeti(L, LUA_REGISTRYINDEX, thread->fn);
	yiL_pushudata(L, thread, $__yi_thread);
	lua_rawgeti(L, LUA_REGISTRYINDEX, thread->data);
	lua_pcall(L, 2, 0, 0);

	return (thread_status)0u;
}

static int yi_thread_exit(lua_State *L)
{
	yi_Thread *thread = yiL_toudata(L, 1);
	if (!thread) return 0;

	thread_exit(thread->handle);
	return 0;
}

int yi_thread(lua_State *L)
{
	yi_Thread *thread = malloc(sizeof(yi_Thread));
	if (thread == NULL) return 0;

	int fn = -1, data = -1;
	thread->main_state = L;
	thread->state = lua_newthread(L);
	thread->data = thread->fn = 0;

#if defined(yi_WIN32)
	thread->handle = NULL;
#else
	memset(&thread->handle, '\0', sizeof(thread_handle));
#endif
	
	if (lua_isfunction(L, 1)) {
		fn = 1;
		data = 2;
	}
	else if (lua_isfunction(L, 2)) {
		fn = 2;
		data = 1;
	}

	lua_pushvalue(L, fn);
	thread->fn = luaL_ref(L, LUA_REGISTRYINDEX);

	if (lua_isnoneornil(L, data))
		lua_pushnil(L);
	else
		lua_pushvalue(L, data);
	thread->data = luaL_ref(L, LUA_REGISTRYINDEX);

	if (!thread_create(&thread->handle, yi_thread_routine, thread)) {
		free(thread);
		luaL_unref(L, LUA_REGISTRYINDEX, thread->fn);

		if (thread->data)
			luaL_unref(L, LUA_REGISTRYINDEX, thread->data);

		lua_pushnil(L);
	}
	else {
		yi_addMem(thread, yi_OTHREAD);
		yiL_pushudata(L, thread, $__yi_thread);
	}

	return 1;
}

static int yi_thread__index(lua_State *L)
{
	yi_Thread *thread = yiL_toudata(L, 1);
	if (!thread) return 0;

	uint32_t hash;
	const char *idx = yiL_tohstr(L, 2, &hash);

	switch (hash)
	{
		case hash(exit):
			lua_pushcfunction(L, yi_thread_exit);
			break;

		case hash(data):
			lua_rawgeti(L, LUA_REGISTRYINDEX, thread->data);

		case hash(func): case hash(fn):
			lua_rawgeti(L, LUA_REGISTRYINDEX, thread->fn);

		default:
			return 0;
	}

	return 1;
}

static int yi_thread__gc(lua_State *L)
{
	yi_printf("saasas...\n");
	return 0;
}

static const luaL_Reg yi_thread__meta[] = {

	{"__index",		yi_thread__index },
	//{"__gc",		yi_thread__gc },
	{ NULL, NULL }
};

void yi_thread_reg(lua_State *L)
{
	luaL_newmetatable(L, $__yi_thread);
	luaL_setfuncs(L, yi_thread__meta, 0);
	lua_pop(L, 1);
}