#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H
#include "osg/Referenced"
#include "LuaState.h"


/**
 * @brief The ScriptEngine class
 */
class ScriptEngine : public LuaState, public osg::Referenced
{
public:
    ScriptEngine();

protected:
    virtual ~ScriptEngine();

private:
    ScriptEngine(const ScriptEngine&);
    ScriptEngine& operator = (const ScriptEngine&);
};

#endif // SCRIPTENGINE_H
