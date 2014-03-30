#ifndef LUABINDING_H
#define LUABINDING_H

#include "osg/Node"

struct lua_State;

osg::Node* lua_toOsgNode(lua_State* L, int index);


#endif // LUABINDING_H
