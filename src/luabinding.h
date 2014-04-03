#ifndef LUABINDING_H
#define LUABINDING_H

#include "osg/Node"

extern "C" {

struct lua_State;

int luaopen_model(lua_State*);

}

osg::Node* lua_toOsgNode(lua_State* L, int index);

void lua_pushOsgNode(lua_State* L, osg::Node *node);

#endif // LUABINDING_H
