%nodefaultctor;

%include <attribute.i>

%module entity
%{

#include "osgLuaBinding.h"
#include "EntityNode.h"

EntityNode* swig_lua_toEntityNode(lua_State* L, int index)
{
    EntityNode *entity = 0;
    int res = SWIG_ConvertPtr(L, index, (void **)(&entity),SWIGTYPE_p_EntityNode, 0);
    return SWIG_IsOK(res) ? entity : 0;
}

void lua_pushNode(lua_State* L, EntityNode *entity)
{
    if (!entity)
    {
        lua_pushnil(L);
    }
    else
    {
        entity->ref();
        SWIG_NewPointerObj(L, entity, SWIGTYPE_p_EntityNode, 1);
    }
}

%}

%init
%{
    SWIG_NewPointerObj(L, new EntityLoader, SWIGTYPE_p_EntityLoader, 1);
    lua_setfield(L, -2, "load");
%}

%feature("ref") EntityNode "$this->ref();"
%feature("unref") EntityNode "$this->unref();"

%attribute(EntityNode, const char*, name, getNameAsCharArray, setName);

class EntityNode
{
protected:
    ~EntityNode();
};

%extend EntityNode
{
    EntityNode* clone() const
    {
        EntityNode* entity = static_cast<EntityNode*>($self->clone(osg::CopyOp::SHALLOW_COPY));
        entity->ref();
        return entity;
    }
}

class EntityLoader
{
public:
    %newobject operator();
    EntityNode* operator()(const char* filename);
};
