#include <cassert>
#include <algorithm>
#include "lua.hpp"
#include "ScriptEngine.h"

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

ScriptResultHandler::ScriptResultHandler(unsigned int expectedResults) : _expectedResults(expectedResults)
{
}

ScriptResultHandler::~ScriptResultHandler()
{
}

ScriptEngine::ScriptEngine()
{
}

ScriptEngine::~ScriptEngine()
{
}

bool ScriptEngine::assertEngineReady()
{
    if (! _luaState.good())
    {
        _lastError = "Script engine is not ready!";
        return false;
    }
    return true;
}

bool ScriptEngine::load(std::istream &istream, const char *streamname)
{
    if (!this->assertEngineReady())
    {
        return false;
    }
    _lastError.clear();
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
        return false;
    }
    if (error)
    {
        _lastError = lua_tostring(_luaState, -1);
        lua_pop(_luaState, 1);
        return false;
    }
    return true;
}

bool ScriptEngine::exec(unsigned int nbExpectedResults, std::istream &istream, const char *streamname)
{
    if (! this->load(istream, streamname))
    {
        return false;
    }

    unsigned int nbResults = (nbExpectedResults == ScriptResultHandler::UNLIMITED_RESULTS) ? LUA_MULTRET : nbExpectedResults;
    if (lua_pcall(_luaState, 0, nbResults, 0))
    {
        _lastError = lua_tostring(_luaState, -1);
        lua_pop(_luaState, 1);
        return false;
    }
    return true;
}

bool ScriptEngine::exec(ScriptResultHandler &handler, std::istream &istream, const char *streamname)
{
    bool result = false;
    int finalStackSize = lua_gettop(this->_luaState);
    if (this->exec(handler.getExpectedResults(), istream, streamname))
    {
        int nbElementsToPop = std::max(0, lua_gettop(this->_luaState) - finalStackSize);
        handler.handle(this->_luaState, nbElementsToPop);
        lua_pop(this->_luaState, nbElementsToPop);

        result = true;
    }
    assert(lua_gettop(this->_luaState) == finalStackSize);
    return result;
}
