#include <string.h>
#include <iostream>
#include "osg/ref_ptr"
#include "scriptengine.h"

class NoopScriptResultHandler : public ScriptResultHandler
{
public:
    virtual void handle(lua_State *luaState)
    {

    }
};

int main (void) {
    osg::ref_ptr<ScriptEngine> scriptEngine = new ScriptEngine();
    NoopScriptResultHandler handler;
    if (! scriptEngine->exec(handler, std::cin, "stdin"))
    {
        std::cerr << scriptEngine->lastError() << std::endl;
    }
    return 0;
}
