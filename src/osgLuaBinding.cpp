#include "lua.hpp"
#include <osg/MatrixTransform>
#include "osgLuaBinding.h"

osg::Node* swig_lua_toOsgNode(lua_State* L, int index);

osg::Node* lua_toOsgNode(lua_State *L, int index)
{
    osg::ref_ptr<osg::Node> node = swig_lua_toOsgNode(L, index);
    if (!node && lua_istable(L, index) && lua_checkstack(L, 2))
    {
        osg::ref_ptr<osg::Group> group = new osg::Group;

        lua_pushnil(L);  /* first key */
        int tableIndex = index >= 0 ? index : index-1;
        while (lua_next(L, tableIndex) != 0)
        {
            osg::ref_ptr<osg::Node> tableNode = lua_toOsgNode(L, -1);
            if (tableNode.valid())
            {
                osg::ref_ptr<osg::Group> indexNode = lua_toOsgMatrixTransform(L, -2);
                if (indexNode.valid())
                {
                    indexNode->addChild(tableNode);
                    tableNode = indexNode;
                }
                group->addChild(tableNode);
            }
            lua_pop(L, 1);
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

osg::Group* lua_toOsgMatrixTransform(lua_State *L, int index)
{
    if(lua_istable(L, index))
    {
        osg::Vec3 vec3 = lua_toOsgVec3(L, index);
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
const V lua_toOsgVec(lua_State *L, int index, char firstComponentName)
{
    V vec;
    if (lua_istable(L, index) && lua_checkstack(L, 2))
    {
        unsigned int vecComponentMask = 0;
        const unsigned int vecComponentCompleteMask = (1 << vec.num_components) - 1;

        lua_pushnil(L);  /* first key */
        int tableIndex = index >= 0 ? index : index-1;
        while (lua_next(L, tableIndex) != 0)
        {
            int vecComponentPos = -1;
            if (lua_isnumber(L, -2))
            {
                vecComponentPos = lua_tonumber(L, -2) - 1;
            }
            else if(lua_isstring(L, -2) && lua_objlen(L, -2) == 1)
            {
                vecComponentPos = lua_tostring(L, -2)[0] - firstComponentName;
            }

            if (vecComponentPos >= 0 && vecComponentPos < vec.num_components)
            {
                vecComponentMask |= 1 << vecComponentPos;
                if (lua_isnumber(L, -1))
                {
                    vec[vecComponentPos] = lua_tonumber(L, -1);
                }
                else
                {
                    vec[vecComponentPos] = NAN;
                }
            }

            lua_pop(L, 1);
            if (vecComponentMask == vecComponentCompleteMask)
            {
                lua_pop(L, 1);
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
