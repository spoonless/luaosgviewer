#include "lua.hpp"
#include "gtest/gtest.h"
#include "osg/observer_ptr"
#include "osg/Group"
#include "ScriptEngine.h"
#include "osgLuaBinding.h"
#include "NodeScriptResultHandler.h"


class NodeScriptResultHandlerTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        scriptEngine = new ScriptEngine;
        EntityScriptLibrary entityScriptLibrary;
        scriptEngine->addLibrary(entityScriptLibrary);
    }

    osg::ref_ptr<ScriptEngine> scriptEngine;
};

TEST_F(NodeScriptResultHandlerTest, handleNoResult)
{
    NodeScriptResultHandler handler;

    handler.handle(*scriptEngine, 0);

    ASSERT_EQ(0, handler.getNode());
}

TEST_F(NodeScriptResultHandlerTest, handleOneResult)
{
    osg::ref_ptr<EntityNode> node = new EntityNode;
    lua_pushNode(*scriptEngine, node);

    NodeScriptResultHandler handler;
    handler.handle(*scriptEngine, 1);

    ASSERT_EQ(1, lua_gettop(*scriptEngine));
    ASSERT_EQ(node, handler.getNode());
}

TEST_F(NodeScriptResultHandlerTest, handleOneResultAndMemoryHandledProperly)
{
    osg::observer_ptr<EntityNode> weakptr;
    {
        osg::ref_ptr<EntityNode> node = new EntityNode;
        lua_pushNode(*scriptEngine, node);
        weakptr = node;
    }

    NodeScriptResultHandler handler;
    handler.handle(*scriptEngine, 1);

    ASSERT_EQ(1, lua_gettop(*scriptEngine));
    ASSERT_TRUE(weakptr.valid());
    ASSERT_EQ(weakptr.get(), handler.getNode());
}

TEST_F(NodeScriptResultHandlerTest, handleTwoResultsAsGroupNode)
{
    osg::ref_ptr<EntityNode> node1 = new EntityNode;
    osg::ref_ptr<EntityNode> node2 = new EntityNode;
    lua_pushNode(*scriptEngine, node1);
    lua_pushNode(*scriptEngine, node2);

    NodeScriptResultHandler handler;
    handler.handle(*scriptEngine, 2);

    ASSERT_EQ(2, lua_gettop(*scriptEngine));
    osg::Node *result = handler.getNode();
    ASSERT_TRUE(result->asGroup() != 0);

    osg::Group *group = result->asGroup();
    ASSERT_EQ(static_cast<unsigned int>(2), group->getNumChildren());
    ASSERT_EQ(node1, group->getChild(0));
    ASSERT_EQ(node2, group->getChild(1));
}

TEST_F(NodeScriptResultHandlerTest, handleCanReduceResultIfOnlyOneIsNode)
{
    osg::ref_ptr<EntityNode> node = new EntityNode;
    lua_pushnumber(*scriptEngine, 999);
    lua_pushNode(*scriptEngine, node);

    NodeScriptResultHandler handler;
    handler.handle(*scriptEngine, 2);

    ASSERT_EQ(2, lua_gettop(*scriptEngine));
    ASSERT_EQ(node, handler.getNode());
}

TEST_F(NodeScriptResultHandlerTest, handleNoNodeInResults)
{
    lua_pushnumber(*scriptEngine, 1);
    lua_pushnumber(*scriptEngine, 2);
    lua_pushnumber(*scriptEngine, 3);

    NodeScriptResultHandler handler;
    handler.handle(*scriptEngine, 3);

    ASSERT_EQ(3, lua_gettop(*scriptEngine));
    ASSERT_EQ(0, handler.getNode());
}

TEST_F(NodeScriptResultHandlerTest, handleLuaTableResult)
{
    osg::ref_ptr<EntityNode> node1 = new EntityNode;
    osg::ref_ptr<EntityNode> node2 = new EntityNode;

    lua_newtable(*scriptEngine);
    lua_pushnumber(*scriptEngine, 1);
    lua_pushNode(*scriptEngine, node1);
    lua_rawset(*scriptEngine, -3);
    lua_pushnumber(*scriptEngine, 2);
    lua_pushNode(*scriptEngine, node2);
    lua_rawset(*scriptEngine, -3);

    NodeScriptResultHandler handler;
    handler.handle(*scriptEngine, 1);

    ASSERT_EQ(1, lua_gettop(*scriptEngine));
    osg::Node *result = handler.getNode();
    ASSERT_TRUE(result->asGroup() != 0);

    osg::Group *group = result->asGroup();
    ASSERT_EQ(static_cast<unsigned int>(2), group->getNumChildren());
    ASSERT_EQ(node1, group->getChild(0));
    ASSERT_EQ(node2, group->getChild(1));
}

TEST_F(NodeScriptResultHandlerTest, handleLuaTableWithOneNodeElementResult)
{
    osg::ref_ptr<EntityNode> node = new EntityNode;

    lua_newtable(*scriptEngine);
    lua_pushnumber(*scriptEngine, 1);
    lua_pushNode(*scriptEngine, node);
    lua_rawset(*scriptEngine, -3);
    lua_pushnumber(*scriptEngine, 2);
    lua_pushstring(*scriptEngine, "none node result");
    lua_rawset(*scriptEngine, -3);

    NodeScriptResultHandler handler;
    handler.handle(*scriptEngine, 1);

    ASSERT_EQ(1, lua_gettop(*scriptEngine));
    ASSERT_EQ(node, handler.getNode());
}
