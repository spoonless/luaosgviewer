#include "lua.hpp"
#include "LuaState.h"

extern "C" {

int luaopen_model(lua_State*);

}

static void openlib(lua_State *L, const char *name, lua_CFunction func)
{
    lua_pushcfunction(L, func);
    lua_pushstring(L, name);
    lua_call(L, 1, 0);
}

LuaState::LuaState()
{
    _luaState = luaL_newstate();
    if (good())
    {
        openlib(_luaState, "", luaopen_base);
        openlib(_luaState, "math", luaopen_math);
        openlib(_luaState, "string", luaopen_string);
        openlib(_luaState, "table", luaopen_table);
        openlib(_luaState, "debug", luaopen_debug);
        openlib(_luaState, "jit", luaopen_jit);
        openlib(_luaState, "model", luaopen_model);
    }
}

LuaState::~LuaState()
{
    lua_close(_luaState);
    _luaState = 0;
}

