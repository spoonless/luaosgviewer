#include "lua.hpp"
#include "luabinding.h"
#include "gtest/gtest.h"
#include "osg/Group"
#include "nodescriptresulthandler.h"

class LuaFixture
{
public:
    LuaFixture()
    {
        _luaState = luaL_newstate();
        openlib("", luaopen_base);
        openlib("string", luaopen_string);
        openlib("table", luaopen_table);
        openlib("model", luaopen_model);
    }

    ~LuaFixture()
    {
        lua_close(_luaState);
    }

    operator lua_State*() { return _luaState; }

private:
    void openlib(const char *name, lua_CFunction func)
    {
        lua_pushcfunction(_luaState, func);
        lua_pushstring(_luaState, name);
        lua_call(_luaState, 1, 0);
    }

    lua_State *_luaState;
};


TEST(NodeScriptResultHandler, handleNoResult)
{
    NodeScriptResultHandler handler;
    handler.handle(0, 0);

    ASSERT_EQ(0, handler.getNode());
}

TEST(NodeScriptResultHandler, handleOneResult)
{
    LuaFixture luaFixture;
    osg::ref_ptr<osg::Node> node = new osg::Node;
    lua_pushOsgNode(luaFixture, node);

    NodeScriptResultHandler handler;
    handler.handle(luaFixture, 1);

    ASSERT_EQ(1, lua_gettop(luaFixture));
    ASSERT_EQ(node, handler.getNode());
}

TEST(NodeScriptResultHandler, handleTwoResultsAsGroupNode)
{
    LuaFixture luaFixture;
    osg::ref_ptr<osg::Node> node1 = new osg::Node;
    osg::ref_ptr<osg::Node> node2 = new osg::Node;
    lua_pushOsgNode(luaFixture, node1);
    lua_pushOsgNode(luaFixture, node2);

    NodeScriptResultHandler handler;
    handler.handle(luaFixture, 2);

    ASSERT_EQ(2, lua_gettop(luaFixture));
    osg::Node *result = handler.getNode();
    ASSERT_TRUE(result->asGroup() != 0);

    osg::Group *group = result->asGroup();
    ASSERT_EQ(static_cast<unsigned int>(2), group->getNumChildren());
    ASSERT_EQ(node1, group->getChild(0));
    ASSERT_EQ(node2, group->getChild(1));
}

TEST(NodeScriptResultHandler, handleCanReduceResultIfOnlyOneIsNode)
{
    LuaFixture luaFixture;
    osg::ref_ptr<osg::Node> node = new osg::Node;
    lua_pushnumber(luaFixture, 999);
    lua_pushOsgNode(luaFixture, node);

    NodeScriptResultHandler handler;
    handler.handle(luaFixture, 2);

    ASSERT_EQ(2, lua_gettop(luaFixture));
    ASSERT_EQ(node, handler.getNode());
}

TEST(NodeScriptResultHandler, handleNoNodeInResults)
{
    LuaFixture luaFixture;
    lua_pushnumber(luaFixture, 1);
    lua_pushnumber(luaFixture, 2);
    lua_pushnumber(luaFixture, 3);

    NodeScriptResultHandler handler;
    handler.handle(luaFixture, 3);

    ASSERT_EQ(3, lua_gettop(luaFixture));
    ASSERT_EQ(0, handler.getNode());
}

TEST(NodeScriptResultHandler, handleLuaTableResult)
{
    LuaFixture luaFixture;
    osg::ref_ptr<osg::Node> node1 = new osg::Node;
    osg::ref_ptr<osg::Node> node2 = new osg::Node;

    lua_newtable(luaFixture);
    lua_pushnumber(luaFixture, 1);
    lua_pushOsgNode(luaFixture, node1);
    lua_rawset(luaFixture, -3);
    lua_pushnumber(luaFixture, 2);
    lua_pushOsgNode(luaFixture, node2);
    lua_rawset(luaFixture, -3);

    NodeScriptResultHandler handler;
    handler.handle(luaFixture, 1);

    ASSERT_EQ(1, lua_gettop(luaFixture));
    osg::Node *result = handler.getNode();
    ASSERT_TRUE(result->asGroup() != 0);

    osg::Group *group = result->asGroup();
    ASSERT_EQ(static_cast<unsigned int>(2), group->getNumChildren());
    ASSERT_EQ(node1, group->getChild(0));
    ASSERT_EQ(node2, group->getChild(1));
}

TEST(NodeScriptResultHandler, handleLuaTableWithOneNodeElementResult)
{
    LuaFixture luaFixture;
    osg::ref_ptr<osg::Node> node = new osg::Node;

    lua_newtable(luaFixture);
    lua_pushnumber(luaFixture, 1);
    lua_pushOsgNode(luaFixture, node);
    lua_rawset(luaFixture, -3);
    lua_pushnumber(luaFixture, 2);
    lua_pushstring(luaFixture, "none node result");
    lua_rawset(luaFixture, -3);

    NodeScriptResultHandler handler;
    handler.handle(luaFixture, 1);

    ASSERT_EQ(1, lua_gettop(luaFixture));
    ASSERT_EQ(node, handler.getNode());
}
