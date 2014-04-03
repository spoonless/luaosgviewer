#ifndef LUASTATE_H
#define LUASTATE_H

struct lua_State;

class LuaState
{
public:
    LuaState();
    ~LuaState();

    inline operator lua_State*() { return _luaState; }

    inline bool good() { return _luaState != 0; }

private:
    LuaState(const LuaState&);
    const LuaState& operator == (const LuaState&);

    lua_State *_luaState;
};


#endif // LUASTATE_H
