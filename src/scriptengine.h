#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H
#include <string>
#include <istream>

struct lua_State;

class ScriptEngine
{
public:
    ScriptEngine();
    virtual ~ScriptEngine();

    bool exec(std::istream &istream, const char *streamname = 0);

    const std::string lastError() const {
        return _lastError;
    }

private:
    ScriptEngine(const ScriptEngine&);
    void init();
    void destroy();
    bool assertEngineReady();

    lua_State *_luaState;
    std::string _lastError;
};

#endif // SCRIPTENGINE_H
