#ifndef NODESCRIPTRESULTHANDLER_H
#define NODESCRIPTRESULTHANDLER_H

#include "osg/ref_ptr"
#include "osg/Node"
#include "ScriptEngine.h"

class NodeScriptResultHandler : public ScriptResultHandler
{
public:
    NodeScriptResultHandler();
    ~NodeScriptResultHandler();

    void handle(LuaState &luaState, unsigned int nbResults);

    osg::Node* getNode()
    {
        return _node.get();
    }

private:
    osg::ref_ptr<osg::Node> _node;
};

#endif // NODESCRIPTRESULTHANDLER_H
