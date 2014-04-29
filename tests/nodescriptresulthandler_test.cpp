#include "gtest/gtest.h"
#include "osg/observer_ptr"
#include "luaX.h"
#include "EntityNode.h"
#include "osgLuaBinding.h"
#include "NodeScriptResultHandler.h"


class NodeScriptResultHandlerTest : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        luaState = new LuaState;
        luaState->openLibrary<EntityLibrary>("entity");
    }

    virtual void TearDown()
    {
        delete luaState;
        luaState = 0;
    }

    LuaState *luaState;
};

TEST_F(NodeScriptResultHandlerTest, handleNoResult)
{
    NodeScriptResultHandler handler;

    handler.handle(*luaState, 0);

    ASSERT_EQ(0, handler.getNode());
}

TEST_F(NodeScriptResultHandlerTest, handleOneResult)
{
    osg::ref_ptr<EntityNode> node = new EntityNode;
    lua_pushNode(*luaState, node);

    NodeScriptResultHandler handler;
    handler.handle(*luaState, 1);

    ASSERT_EQ(1, lua_gettop(*luaState));
    ASSERT_EQ(node, handler.getNode());
}

TEST_F(NodeScriptResultHandlerTest, handleOneResultAndMemoryHandledProperly)
{
    osg::observer_ptr<EntityNode> weakptr;
    {
        osg::ref_ptr<EntityNode> node = new EntityNode;
        lua_pushNode(*luaState, node);
        weakptr = node;
    }

    NodeScriptResultHandler handler;
    handler.handle(*luaState, 1);

    ASSERT_EQ(1, lua_gettop(*luaState));
    ASSERT_TRUE(weakptr.valid());
    ASSERT_EQ(weakptr.get(), handler.getNode());
}

TEST_F(NodeScriptResultHandlerTest, handleTwoResultsAsGroupNode)
{
    osg::ref_ptr<EntityNode> node1 = new EntityNode;
    osg::ref_ptr<EntityNode> node2 = new EntityNode;
    lua_pushNode(*luaState, node1);
    lua_pushNode(*luaState, node2);

    NodeScriptResultHandler handler;
    handler.handle(*luaState, 2);

    ASSERT_EQ(2, lua_gettop(*luaState));
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
    lua_pushnumber(*luaState, 999);
    lua_pushNode(*luaState, node);

    NodeScriptResultHandler handler;
    handler.handle(*luaState, 2);

    ASSERT_EQ(2, lua_gettop(*luaState));
    ASSERT_EQ(node, handler.getNode());
}

TEST_F(NodeScriptResultHandlerTest, handleNoNodeInResults)
{
    lua_pushnumber(*luaState, 1);
    lua_pushnumber(*luaState, 2);
    lua_pushnumber(*luaState, 3);

    NodeScriptResultHandler handler;
    handler.handle(*luaState, 3);

    ASSERT_EQ(3, lua_gettop(*luaState));
    ASSERT_EQ(0, handler.getNode());
}

TEST_F(NodeScriptResultHandlerTest, handleLuaTableResult)
{
    osg::ref_ptr<EntityNode> node1 = new EntityNode;
    osg::ref_ptr<EntityNode> node2 = new EntityNode;

    lua_newtable(*luaState);
    lua_pushnumber(*luaState, 1);
    lua_pushNode(*luaState, node1);
    lua_rawset(*luaState, -3);
    lua_pushnumber(*luaState, 2);
    lua_pushNode(*luaState, node2);
    lua_rawset(*luaState, -3);

    NodeScriptResultHandler handler;
    handler.handle(*luaState, 1);

    ASSERT_EQ(1, lua_gettop(*luaState));
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

    lua_newtable(*luaState);
    lua_pushnumber(*luaState, 1);
    lua_pushNode(*luaState, node);
    lua_rawset(*luaState, -3);
    lua_pushnumber(*luaState, 2);
    lua_pushstring(*luaState, "none node result");
    lua_rawset(*luaState, -3);

    NodeScriptResultHandler handler;
    handler.handle(*luaState, 1);

    ASSERT_EQ(1, lua_gettop(*luaState));
    ASSERT_EQ(node, handler.getNode());
}
