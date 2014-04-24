#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H
#include <string>
#include "osg/Referenced"
#include "LuaState.h"

class ScriptEngine;

class ScriptLibrary
{
public:
    virtual const char* getName() const = 0;

    virtual int open(ScriptEngine *scriptEngine) = 0;
};


/**
 * @brief The ScriptEngine class
 */
class ScriptEngine : public LuaState, public osg::Referenced
{
public:
    ScriptEngine();

    void addLibrary(ScriptLibrary &library);

protected:
    virtual ~ScriptEngine();

private:
    ScriptEngine(const ScriptEngine&);
    ScriptEngine& operator = (const ScriptEngine&);
};

#endif // SCRIPTENGINE_H
