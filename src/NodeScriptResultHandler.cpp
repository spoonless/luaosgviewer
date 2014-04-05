#include "osgLuaBinding.h"
#include "NodeScriptResultHandler.h"


NodeScriptResultHandler::NodeScriptResultHandler() :
    LuaResultHandler(LuaResultHandler::UNLIMITED_RESULTS)
{

}

NodeScriptResultHandler::~NodeScriptResultHandler()
{

}

void NodeScriptResultHandler::handle(LuaState &luaState, unsigned int nbResults)
{
    if (nbResults == 0)
    {
        _node = 0;
    }
    else if (nbResults == 1)
    {
        _node = lua_toOsgNode(luaState, -1);
    }
    else
    {
        osg::ref_ptr<osg::Group> group = new osg::Group;
        for (unsigned int i = nbResults; i > 0; --i)
        {
            osg::Node *node = lua_toOsgNode(luaState, -i);
            group->addChild(node);
        }

        if (group->getNumChildren() == 1)
        {
            _node = group->getChild(0);
        }
        else if (group->getNumChildren() > 0)
        {
            _node = group;
        }
    }
}
