#ifndef LUABINDING_H
#define LUABINDING_H

#include "osg/Node"
#include "osg/Group"
#include "osg/Vec3"

extern "C" {

struct lua_State;

}

osg::Node* lua_toOsgNode(lua_State *L, int index);
void lua_pushOsgNode(lua_State* L, osg::Node *node);

osg::Group* lua_toOsgMatrixTransform(lua_State *L, int index);

template<class V>
const V lua_toOsgVec(lua_State *L, int index, char firstComponentName);

inline const osg::Vec3 lua_toOsgVec3(lua_State *L, int index)
{
    return lua_toOsgVec<osg::Vec3>(L, index, 'x');
}

#endif // LUABINDING_H
