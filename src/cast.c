#include "yi.h"

int yi_p2i(lua_State *L)
{
	void *p = lua_touserdata(L, 1);

	lua_pushinteger(L, (lua_Integer)p);
	return 1;
}

int yi_i2p(lua_State *L)
{
	lua_Integer i = lua_tointeger(L, 1);

	lua_pushlightuserdata(L, (void*)i);
	return 1;
}

int yi_cast(lua_State *L)
{


	return 0;
}