#include "lua.hpp"
#include "gtest/gtest.h"
#include "osg/Group"
#include "luabinding.h"
#include "NodeScriptResultHandler.h"

TEST(NodeScriptResultHandler, handleNoResult)
{
    LuaState luaState;
    NodeScriptResultHandler handler;

    handler.handle(luaState, 0);

    ASSERT_EQ(0, handler.getNode());
}

TEST(NodeScriptResultHandler, handleOneResult)
{
    LuaState luaState;
    osg::ref_ptr<osg::Node> node = new osg::Node;
    lua_pushOsgNode(luaState, node);

    NodeScriptResultHandler handler;
    handler.handle(luaState, 1);

    ASSERT_EQ(1, lua_gettop(luaState));
    ASSERT_EQ(node, handler.getNode());
}

TEST(NodeScriptResultHandler, handleTwoResultsAsGroupNode)
{
    LuaState luaState;
    osg::ref_ptr<osg::Node> node1 = new osg::Node;
    osg::ref_ptr<osg::Node> node2 = new osg::Node;
    lua_pushOsgNode(luaState, node1);
    lua_pushOsgNode(luaState, node2);

    NodeScriptResultHandler handler;
    handler.handle(luaState, 2);

    ASSERT_EQ(2, lua_gettop(luaState));
    osg::Node *result = handler.getNode();
    ASSERT_TRUE(result->asGroup() != 0);

    osg::Group *group = result->asGroup();
    ASSERT_EQ(static_cast<unsigned int>(2), group->getNumChildren());
    ASSERT_EQ(node1, group->getChild(0));
    ASSERT_EQ(node2, group->getChild(1));
}

TEST(NodeScriptResultHandler, handleCanReduceResultIfOnlyOneIsNode)
{
    LuaState luaState;
    osg::ref_ptr<osg::Node> node = new osg::Node;
    lua_pushnumber(luaState, 999);
    lua_pushOsgNode(luaState, node);

    NodeScriptResultHandler handler;
    handler.handle(luaState, 2);

    ASSERT_EQ(2, lua_gettop(luaState));
    ASSERT_EQ(node, handler.getNode());
}

TEST(NodeScriptResultHandler, handleNoNodeInResults)
{
    LuaState luaState;
    lua_pushnumber(luaState, 1);
    lua_pushnumber(luaState, 2);
    lua_pushnumber(luaState, 3);

    NodeScriptResultHandler handler;
    handler.handle(luaState, 3);

    ASSERT_EQ(3, lua_gettop(luaState));
    ASSERT_EQ(0, handler.getNode());
}

TEST(NodeScriptResultHandler, handleLuaTableResult)
{
    LuaState luaState;
    osg::ref_ptr<osg::Node> node1 = new osg::Node;
    osg::ref_ptr<osg::Node> node2 = new osg::Node;

    lua_newtable(luaState);
    lua_pushnumber(luaState, 1);
    lua_pushOsgNode(luaState, node1);
    lua_rawset(luaState, -3);
    lua_pushnumber(luaState, 2);
    lua_pushOsgNode(luaState, node2);
    lua_rawset(luaState, -3);

    NodeScriptResultHandler handler;
    handler.handle(luaState, 1);

    ASSERT_EQ(1, lua_gettop(luaState));
    osg::Node *result = handler.getNode();
    ASSERT_TRUE(result->asGroup() != 0);

    osg::Group *group = result->asGroup();
    ASSERT_EQ(static_cast<unsigned int>(2), group->getNumChildren());
    ASSERT_EQ(node1, group->getChild(0));
    ASSERT_EQ(node2, group->getChild(1));
}

TEST(NodeScriptResultHandler, handleLuaTableWithOneNodeElementResult)
{
    LuaState luaState;
    osg::ref_ptr<osg::Node> node = new osg::Node;

    lua_newtable(luaState);
    lua_pushnumber(luaState, 1);
    lua_pushOsgNode(luaState, node);
    lua_rawset(luaState, -3);
    lua_pushnumber(luaState, 2);
    lua_pushstring(luaState, "none node result");
    lua_rawset(luaState, -3);

    NodeScriptResultHandler handler;
    handler.handle(luaState, 1);

    ASSERT_EQ(1, lua_gettop(luaState));
    ASSERT_EQ(node, handler.getNode());
}
