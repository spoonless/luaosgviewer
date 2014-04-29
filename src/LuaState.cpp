#include <algorithm>
#include "LuaState.h"
#include "luaX.h"

static const char* luaX_extensionLibraryTable = "_X_internal_extlib";
static const char* luaX_selfreference = "_X_self";

static void luaX_openlib(lua_State *L, const char *name, lua_CFunction func)
{
    lua_pushcfunction(L, func);
    lua_pushstring(L, name);
    lua_call(L, 1, 0);
}

static void luaX_setSelfReference(LuaState *luaState)
{
    lua_pushlightuserdata(*luaState, luaState);
    lua_setfield(*luaState, LUA_REGISTRYINDEX, luaX_selfreference);
}


static int openLuaExtensionLibrary(lua_State *L)
{
    LuaState *luaState = reinterpret_cast<LuaState*>(lua_touserdata(L, lua_upvalueindex(1)));
    LuaExtensionLibrary *library = reinterpret_cast<LuaExtensionLibrary*>(lua_touserdata(L, lua_upvalueindex(2)));
    return library->open(*luaState);
}


#define BUFFER_READER 512

class LuaReader {
public :
    LuaReader(std::istream &istream);

    int read();

    const char* buffer() const
    {
        return this->_buffer;
    }

private:
    std::istream &_istream;
    char _buffer[BUFFER_READER];
};

LuaReader::LuaReader(std::istream &istream) : _istream(istream)
{
}

int LuaReader::read()
{
    if (!_istream.good())
    {
        return 0;
    }
    _istream.read(_buffer, BUFFER_READER);
    return _istream.gcount();
}

static const char * luaReadFromIstream (lua_State *L, void *data, size_t *size)
{
    LuaReader* readerState = static_cast<LuaReader*>(data);
    *size = readerState->read();
    return *size > 0 ? readerState->buffer() : 0;
}

LuaResultHandler::LuaResultHandler(unsigned int expectedResults) : _expectedResults(expectedResults)
{
}

LuaResultHandler::~LuaResultHandler()
{
}

LuaExtensionLibrary::~LuaExtensionLibrary()
{

}

LuaState::LuaState()
{
    _luaState = luaL_newstate();
    if (good())
    {
        luaX_openlib(_luaState, "", luaopen_base);
        luaX_openlib(_luaState, "math", luaopen_math);
        luaX_openlib(_luaState, "string", luaopen_string);
        luaX_openlib(_luaState, "table", luaopen_table);
        luaX_openlib(_luaState, "debug", luaopen_debug);
        luaX_openlib(_luaState, "jit", luaopen_jit);
        luaX_setSelfReference(this);
    }
}

LuaState::~LuaState()
{
    deleteExtensionLibraryRegistry();
    lua_close(_luaState);
    _luaState = 0;
}

LuaState* LuaState::from(lua_State *L)
{
    if (!L)
    {
        return 0;
    }
    MARK_LUA_STACK(L);
    lua_getfield(L, LUA_REGISTRYINDEX, luaX_selfreference);
    LuaState *luaState = 0;
    if (lua_islightuserdata(L, -1))
    {
        luaState = reinterpret_cast<LuaState*>(lua_touserdata(L, -1));
    }
    lua_pop(L, 1);
    CHECK_LUA_STACK(L);
    return luaState;
}

LuaExtensionLibrary *LuaState::getLibrary(const char* internalLibname)
{
    if (!this->assertEngineReady())
    {
        return false;
    }
    LuaExtensionLibrary *lib = 0;
    MARK_LUA_STACK(_luaState);
    pushExtensionLibraryRegistry();
    lua_getfield(_luaState, -1, internalLibname);
    lib = reinterpret_cast<LuaExtensionLibrary*>(lua_touserdata(_luaState, -1));
    lua_pop(_luaState, 2);
    CHECK_LUA_STACK(_luaState);
    return lib;
}

bool LuaState::openLibrary(LuaExtensionLibrary *library, const char *internalLibname, const char *libname)
{
    if (!this->assertEngineReady())
    {
        return false;
    }
    MARK_LUA_STACK(_luaState);

    lua_pushlightuserdata(_luaState, this);
    lua_pushlightuserdata(_luaState, library);
    lua_pushcclosure(_luaState, ::openLuaExtensionLibrary, 2);
    lua_pushstring(_luaState, libname);
    if(lua_pcall(_luaState, 1, 0, 0))
    {
        _lastError = lua_tostring(_luaState, -1);
        lua_pop(_luaState, 1);
        CHECK_LUA_STACK(_luaState);
        return false;
    }

    pushExtensionLibraryRegistry();
    lua_pushlightuserdata(_luaState, library);
    lua_setfield(_luaState, -2, internalLibname);
    lua_pop(_luaState, 1);

    CHECK_LUA_STACK(_luaState);
    return true;
}

void LuaState::pushExtensionLibraryRegistry()
{
    MARK_LUA_STACK(_luaState);
    lua_getfield(_luaState, LUA_REGISTRYINDEX, luaX_extensionLibraryTable);
    if(lua_isnil(_luaState, -1))
    {
        lua_pop(_luaState, 1);
        lua_newtable(_luaState);
        lua_pushvalue(_luaState,-1);
        lua_setfield(_luaState, LUA_REGISTRYINDEX, luaX_extensionLibraryTable);
    }
    CHECK_LUA_STACK_CHANGE(_luaState, 1);
}

void LuaState::deleteExtensionLibraryRegistry()
{
    MARK_LUA_STACK(_luaState);
    lua_getfield(_luaState, LUA_REGISTRYINDEX, luaX_extensionLibraryTable);
    if(lua_istable(_luaState, -1))
    {
        lua_pushnil(_luaState);
        while (lua_next(_luaState, -2))
        {
            delete reinterpret_cast<LuaExtensionLibrary*>(lua_touserdata(_luaState, -1));
            lua_pop(_luaState, 1);
        }
    }
    lua_pop(_luaState, 1);
    lua_pushnil(_luaState);
    lua_setfield(_luaState, LUA_REGISTRYINDEX, luaX_extensionLibraryTable);
    CHECK_LUA_STACK(_luaState);
}

bool LuaState::assertEngineReady()
{
    if (! this->good())
    {
        this->_lastError = "Script engine is not ready!";
        return false;
    }
    this->_lastError.clear();
    return true;
}

bool LuaState::load(std::istream &istream, const char *streamname)
{
    MARK_LUA_STACK(_luaState);
    if (!streamname)
    {
        streamname = "unknown";
    }

    LuaReader *luaReader = new LuaReader(istream);
    int error = lua_load(_luaState, luaReadFromIstream, luaReader, streamname);
    delete luaReader;

    if(!istream.eof() && istream.fail())
    {
        _lastError.append("Error while reading '").append(streamname).append("'!");
        // now, one error message or one function is on the stack
        // we must remove it
        lua_pop(_luaState, 1);
        CHECK_LUA_STACK(_luaState);
        return false;
    }
    if (error)
    {
        _lastError = lua_tostring(_luaState, -1);
        lua_pop(_luaState, 1);
        CHECK_LUA_STACK(_luaState);
        return false;
    }
    CHECK_LUA_STACK_CHANGE(_luaState, 1);
    return true;
}

bool LuaState::exec(unsigned int nbExpectedResults, std::istream &istream, const char *streamname)
{
    if (!this->assertEngineReady())
    {
        return false;
    }
    MARK_LUA_STACK(_luaState);
    if (! this->load(istream, streamname))
    {
        return false;
    }

    unsigned int nbResults = (nbExpectedResults == LuaResultHandler::UNLIMITED_RESULTS) ? LUA_MULTRET : nbExpectedResults;
    if (lua_pcall(_luaState, 0, nbResults, 0))
    {
        _lastError = lua_tostring(_luaState, -1);
        lua_pop(_luaState, 1);
        CHECK_LUA_STACK(_luaState);
        return false;
    }
    return true;
}

bool LuaState::exec(LuaResultHandler &handler, std::istream &istream, const char *streamname)
{
    if (!this->assertEngineReady())
    {
        return false;
    }
    MARK_LUA_STACK(_luaState);
    bool result = false;
    int beginStackSize = lua_gettop(this->_luaState);
    if (this->exec(handler.getExpectedResults(), istream, streamname))
    {
        int nbElementsToPop = std::max(0, lua_gettop(this->_luaState) - beginStackSize);
        handler.handle(*this, nbElementsToPop);
        lua_pop(this->_luaState, nbElementsToPop);

        result = true;
    }
    CHECK_LUA_STACK(_luaState);
    return result;
}
