#include "lua.hpp"
#include "gtest/gtest.h"
#include "LuaState.h"
#include "osgLuaBinding.h"

static void pushTestString(LuaState &luaState, const char *test)
{
    std::stringstream stream;
    stream << "test = " << test;

    ASSERT_TRUE(luaState.exec(stream));
    lua_getglobal(luaState, "test");
}

inline static void assertVec3(LuaState &luaState, const osg::Vec3 &expectedVec, const char *luaForm)
{
    int topIndex = lua_gettop(luaState);
    pushTestString(luaState, luaForm);
    osg::Vec3 result = lua_toOsgVec3(luaState, -1);
    lua_pop(luaState, 1);

    ASSERT_EQ(topIndex, lua_gettop(luaState));
    ASSERT_TRUE(result.valid());
    ASSERT_EQ(expectedVec, result);
}

inline static void assertVec3Invalid(LuaState &luaState, const char *luaForm)
{
    int topIndex = lua_gettop(luaState);
    pushTestString(luaState, luaForm);
    osg::Vec3 result = lua_toOsgVec3(luaState, -1);
    lua_pop(luaState, 1);

    ASSERT_EQ(topIndex, lua_gettop(luaState));
    ASSERT_FALSE(result.valid());
}

TEST(osgLuaBinding, canConvertTableToVec3)
{
    LuaState luaState;

    assertVec3(luaState, osg::Vec3(0,0,0), "{}");
    assertVec3(luaState, osg::Vec3(1,2,3), "{1,2,3}");
    assertVec3(luaState, osg::Vec3(1,2,3), "{\"1\",\"2\",\"3\"}");
    assertVec3(luaState, osg::Vec3(1.1,2.2,3.3), "{1.1,2.2,3.3}");
    assertVec3(luaState, osg::Vec3(1,2,3), "{1,2,3,4,5}");
    assertVec3(luaState, osg::Vec3(1,2,0), "{1,2}");
    assertVec3(luaState, osg::Vec3(1,0,0), "{1}");
    assertVec3(luaState, osg::Vec3(0,0,0), "{[4]=4}");

    assertVec3(luaState, osg::Vec3(1,2,3), "{x=1,y=2,z=3}");
    assertVec3(luaState, osg::Vec3(1,2,3), "{z=3,x=1,y=2}");
    assertVec3(luaState, osg::Vec3(1,0,0), "{x=1}");
    assertVec3(luaState, osg::Vec3(0,2,0), "{y=2}");
    assertVec3(luaState, osg::Vec3(0,0,3), "{z=3}");
    assertVec3(luaState, osg::Vec3(0,2,3), "{y=2, z=3}");
    assertVec3(luaState, osg::Vec3(0,2,3), "{xx=1, y=2, z=3}");
    assertVec3(luaState, osg::Vec3(1,2,3), "{xx=11, yy=22, zz=33, 1, 2, 3}");

    assertVec3(luaState, osg::Vec3(1,2,0), "{1,y=2,3}");
}

TEST(osgLuaBinding, cannotConvertInvalidFormToVec3)
{
    LuaState luaState;

    assertVec3Invalid(luaState, "1");
    assertVec3Invalid(luaState, "{x=\"invalid representation\"}");
}
