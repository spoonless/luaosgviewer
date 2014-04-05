#ifndef LUASTATE_H
#define LUASTATE_H
#include <climits>
#include <fstream>
#include <string>

extern "C"
{
    struct lua_State;
}

class LuaState;

/**
 * @brief The ScriptResultHandler class
 */
class LuaResultHandler
{
public:
    static const unsigned int UNLIMITED_RESULTS = UINT_MAX;

    LuaResultHandler(unsigned int getExpectedResults = UNLIMITED_RESULTS);
    virtual ~LuaResultHandler();

    virtual void handle(LuaState &luaState, unsigned int nbResults) = 0;

    unsigned int getExpectedResults()
    {
        return _expectedResults;
    }

protected:
    unsigned int _expectedResults;
};

/**
 * @brief The LuaState class
 */
class LuaState
{
public:
    LuaState();
    virtual ~LuaState();

    inline operator lua_State*() { return _luaState; }

    inline bool good() { return _luaState != 0; }

    bool exec(LuaResultHandler &handler, std::istream &istream, const char *streamname = 0);

    bool exec(std::istream &istream, const char *streamname = 0)
    {
        return this->exec(0, istream, streamname);
    }

    const std::string lastError() const {
        return _lastError;
    }

private:
    LuaState(const LuaState&);
    const LuaState& operator == (const LuaState&);

    bool assertEngineReady();
    bool load(std::istream &istream, const char *streamname);
    bool exec(unsigned int nbExpectedResults, std::istream &istream, const char *streamname = 0);

    lua_State *_luaState;
    std::string _lastError;
};

#endif // LUASTATE_H
