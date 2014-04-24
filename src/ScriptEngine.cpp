#include "osg/ref_ptr"
#include "ScriptEngine.h"
#include "lua.hpp"


static void setSelfReference(ScriptEngine *scriptEngine)
{
    lua_pushlightuserdata(*scriptEngine, scriptEngine);
    lua_setfield(*scriptEngine, LUA_REGISTRYINDEX, "scriptEngine");
}

static ScriptEngine* getSelfReference(lua_State *L)
{
    lua_getfield(L, LUA_REGISTRYINDEX, "scriptEngine");
    ScriptEngine *scriptEngine = 0;
    if (lua_islightuserdata(L, -1))
    {
        scriptEngine = reinterpret_cast<ScriptEngine*>(lua_touserdata(L, -1));
    }
    lua_pop(L, 1);
    return scriptEngine;
}

static int openLibrary(lua_State *L)
{
    ScriptLibrary* scriptLibrary = reinterpret_cast<ScriptLibrary*>(lua_touserdata(L, lua_upvalueindex(1)));
    osg::ref_ptr<ScriptEngine> scriptEngine = getSelfReference(L);
    if (scriptLibrary && scriptEngine.valid())
    {
        return scriptLibrary->open(scriptEngine);
    }
    return 0;
}

ScriptEngine::ScriptEngine()
{
    setSelfReference(this);
}

ScriptEngine::~ScriptEngine()
{

}

void ScriptEngine::addLibrary(ScriptLibrary &library)
{
    if (lua_checkstack(*this, 3))
    {
        lua_pushlightuserdata(*this, &library);
        lua_pushcclosure(*this, openLibrary, 1);
        lua_pushstring(*this, library.getName());
        lua_call(*this, 1, 0);
    }
}

