#ifndef LUAX_H
#define LUAX_H

#include <cassert>
#include "lua.hpp"

#ifdef	NDEBUG

#define MARK_LUA_STACK(L)
#define CHECK_LUA_STACK(L)
#define CHECK_LUA_STACK_CHANGE(L, delta)

#else

#define MARK_LUA_STACK(L) int _________stacksize = lua_gettop(L);
#define CHECK_LUA_STACK(L) assert(lua_gettop(L) == _________stacksize);
#define CHECK_LUA_STACK_CHANGE(L, delta) assert(lua_gettop(L) == (_________stacksize + (delta)));

#endif

#endif // LUAX_H
