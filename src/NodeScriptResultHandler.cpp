#include "osg/Group"
#include "osg/MatrixTransform"
#include "lua.hpp"
#include "NodeScriptResultHandler.h"
#include "luabinding.h"

static osg::Node* convertToOsgNode(LuaState &luaState, int index);
static osg::Group* convertIndexTable(LuaState &luaState, int index);

template<class V>
static const V lua_toOsgVec(LuaState &luaState, int index, char firstComponentName);

inline osg::Vec3 lua_toOsgVec3(LuaState &luaState, int index)
{
    return lua_toOsgVec<osg::Vec3>(luaState, index, 'x');
}

NodeScriptResultHandler::NodeScriptResultHandler() :
    ScriptResultHandler(ScriptResultHandler::UNLIMITED_RESULTS)
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

static osg::Node* convertToOsgNode(LuaState &luaState, int index)
{
    osg::ref_ptr<osg::Node> node = lua_toOsgNode(luaState, index);
    if (!node && lua_istable(luaState, index) && lua_checkstack(luaState, 2))
    {
        osg::ref_ptr<osg::Group> group = new osg::Group;

        lua_pushnil(luaState);  /* first key */
        int tableIndex = index >= 0 ? index : index-1;
        while (lua_next(luaState, tableIndex) != 0)
        {
            osg::ref_ptr<osg::Node> tableNode = convertToOsgNode(luaState, -1);
            if (tableNode.valid())
            {
                osg::ref_ptr<osg::Group> indexNode = convertIndexTable(luaState, -2);
                if (indexNode.valid())
                {
                    indexNode->addChild(tableNode);
                    tableNode = indexNode;
                }
                group->addChild(tableNode);
            }
            lua_pop(luaState, 1);
        }

        if (group->getNumChildren() == 1)
        {
            node = group->getChild(0);
        }
        else if (group->getNumChildren() > 0)
        {
            node = group;
        }
    }
    return node.release();
}

static osg::Group* convertIndexTable(LuaState &luaState, int index)
{
    if(lua_istable(luaState, index))
    {
        osg::Vec3 vec3 = lua_toOsgVec3(luaState, index);
        if (vec3.valid())
        {
            osg::Matrix matrix;
            matrix.setTrans(vec3);
            osg::ref_ptr<osg::MatrixTransform> matrixTransform = new osg::MatrixTransform(matrix);
            return matrixTransform.release();
        }
    }
    return 0;
}

template <class V>
static const V lua_toOsgVec(LuaState &luaState, int index, char firstComponentName)
{
    V vec;
    if (lua_istable(luaState, index) && lua_checkstack(luaState, 2))
    {
        unsigned int vecComponentMask = 0;
        const unsigned int vecComponentCompleteMask = (1 << vec.num_components) - 1;

        lua_pushnil(luaState);  /* first key */
        int tableIndex = index >= 0 ? index : index-1;
        while (lua_next(luaState, tableIndex) != 0)
        {
            int vecComponentPos = -1;
            if (lua_isnumber(luaState, -2))
            {
                vecComponentPos = lua_tonumber(luaState, -2) - 1;
            }
            else if(lua_isstring(luaState, -2) && lua_objlen(luaState, -2) == 1)
            {
                vecComponentPos = lua_tostring(luaState, -2)[0] - firstComponentName;
            }

            if (vecComponentPos >= 0 && vecComponentPos < vec.num_components)
            {
                vecComponentMask |= 1 << vecComponentPos;
                if (lua_isnumber(luaState, -1))
                {
                    vec[vecComponentPos] = lua_tonumber(luaState, -1);
                }
                else
                {
                    vec[vecComponentPos] = NAN;
                }
            }

            lua_pop(luaState, 1);
            if (vecComponentMask == vecComponentCompleteMask)
            {
                lua_pop(luaState, 1);
                break;
            }
        }
    }
    else
    {
        vec[0] = NAN;
    }
    return vec;
}
