#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H
#include <climits>
#include <string>
#include <istream>
#include "osg/Referenced"

struct lua_State;

/**
 * @brief The ScriptResultHandler class
 */
class ScriptResultHandler
{
public:
    static const unsigned int UNLIMITED_RESULTS = UINT_MAX;

    ScriptResultHandler(unsigned int getExpectedResults = UNLIMITED_RESULTS);
    virtual ~ScriptResultHandler();

    virtual void handle(lua_State* luaState, unsigned int nbResults) = 0;

    unsigned int getExpectedResults()
    {
        return _expectedResults;
    }

protected:
    unsigned int _expectedResults;
};

/**
 * @brief The ScriptEngine class
 */
class ScriptEngine : public osg::Referenced
{
public:
    ScriptEngine();

    bool exec(ScriptResultHandler &handler, std::istream &istream, const char *streamname = 0);

    bool exec(std::istream &istream, const char *streamname = 0)
    {
        return this->exec(0, istream, streamname);
    }

    const std::string lastError() const {
        return _lastError;
    }

protected:
    virtual ~ScriptEngine();

private:
    ScriptEngine(const ScriptEngine&);
    ScriptEngine& operator = (const ScriptEngine&);
    void init();
    void destroy();
    bool assertEngineReady();
    bool load(std::istream &istream, const char *streamname);
    bool exec(unsigned int nbExpectedResults, std::istream &istream, const char *streamname = 0);

    lua_State *_luaState;
    std::string _lastError;
};

#endif // SCRIPTENGINE_H
