%nodefaultctor;

%include <attribute.i>

%module entity
%{

#include "osgLuaBinding.h"
#include "EntityNode.h"

typedef int LuaStackIndex;

EntityNode* swig_lua_toEntityNode(lua_State* L, int index)
{
    EntityNode *entity = 0;
    int res = SWIG_ConvertPtr(L, index, (void **)(&entity),SWIGTYPE_p_EntityNode, 0);
    return SWIG_IsOK(res) ? entity : 0;
}

static void swig_lua_pusherrormessage(lua_State* L, const char* func_name, LuaStackIndex argnum, const char* type)
{
    lua_pushfstring(L,"Error in %s (arg %d), expected '%s' got '%s'", func_name,argnum,type,SWIG_Lua_typename(L,argnum));
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

%typemap(in) LuaStackIndex
%{ $1=$input; %}

%feature("ref") EntityNode "$this->ref();"
%feature("unref") EntityNode "$this->unref();"

%attribute(EntityNode, const char*, name, getNameAsCharArray, setName);

class EntityNode
{
public:
    void fireEvent(const char* event);

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

    EntityNode* addEventHandlers(lua_State *L, LuaStackIndex tableIndex)
    {
        if(!lua_istable(L, tableIndex))
        {
            swig_lua_pusherrormessage(L, "EntityNode:addEventHandlers", tableIndex, "table");
            lua_error(L);
        }

        osg::ref_ptr<EventHandlers> eventHandlers = $self->getEventHandlers();
        if (! eventHandlers)
        {
            LuaState *luaState = LuaState::from(L);
            if (luaState)
            {
                eventHandlers = $self->getOrCreateEventHandlers(luaState->getLibrary<EntityLibrary>());
            }
        }

        if (eventHandlers.valid() && lua_checkstack(L,4))
        {
            lua_rawgeti(L, LUA_REGISTRYINDEX, static_cast<LuaEventHandlersRef>(*eventHandlers));
            lua_pushnil(L);  /* first key */
            while (lua_next(L, tableIndex) != 0)
            {
                lua_pushvalue(L, -2);
                lua_pushvalue(L, -2);
                lua_settable (L, -5);
                lua_pop(L,1);
            }
            lua_pop(L,1);
        }
        return $self;
    }
}

class EntityLoader
{
public:
    %newobject operator();
    EntityNode* operator()(const char* filename);
};
