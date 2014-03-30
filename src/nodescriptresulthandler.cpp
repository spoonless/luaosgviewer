#include "osg/Group"
#include "lua.hpp"
#include "nodescriptresulthandler.h"
#include "luabinding.h"

static osg::Node* convertToOsgNode(lua_State *luaState, int index);

NodeScriptResultHandler::NodeScriptResultHandler() :
    ScriptResultHandler(ScriptResultHandler::UNLIMITED_RESULTS)
{

}

NodeScriptResultHandler::~NodeScriptResultHandler()
{

}

void NodeScriptResultHandler::handle(lua_State *luaState, unsigned int nbResults)
{
    if (nbResults == 0)
    {
        _node = 0;
    }
    else if (nbResults == 1)
    {
        _node = convertToOsgNode(luaState, -1);
    }
    else
    {
        osg::ref_ptr<osg::Group> group = new osg::Group;
        for (unsigned int i = nbResults; i > 0; --i)
        {
            osg::Node *node = convertToOsgNode(luaState, -i);
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

static osg::Node* convertToOsgNode(lua_State *luaState, int index)
{
    osg::ref_ptr<osg::Node> node = lua_toOsgNode(luaState, index);
    if (!node && lua_istable(luaState, index) && lua_checkstack(luaState, 2))
    {
        int tableIndex = index >= 0 ? index : index-1;
        osg::ref_ptr<osg::Group> group = new osg::Group;
        lua_pushnil(luaState);  /* first key */
        while (lua_next(luaState, tableIndex) != 0) {
            osg::Node *tableNode = convertToOsgNode(luaState, -1);
            group->addChild(tableNode);
            lua_pop(luaState, 1);
        }
        if (group->getNumChildren() == 1) {
            node = group->getChild(0);
        }
        else if (group->getNumChildren() > 0)
        {
            node = group;
        }
    }
    return node.release();
}
