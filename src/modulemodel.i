%nodefaultctor;

%include <attribute.i>

%module model
%{

#include "modulemodel.h"
#include "luabinding.h"

osg::Node* lua_toOsgNode(lua_State* L, int index)
{
    binding::Model *model = 0;
    int res = SWIG_ConvertPtr(L, index, (void **)(&model),SWIGTYPE_p_binding__Model, 0);
    return (SWIG_IsOK(res) && model) ? model->getNode() : 0;
}

void lua_pushOsgNode(lua_State* L, osg::Node *node)
{
    if (!node)
    {
        lua_pushnil(L);
    }
    else
    {
        binding::Model *model = new binding::Model(node);
        SWIG_NewPointerObj(L, model, SWIGTYPE_p_binding__Model, 1);
    }
}

%}

%init
%{
    SWIG_NewPointerObj(L, new binding::ModelLoader, SWIGTYPE_p_binding__ModelLoader, 1);
    lua_setfield(L, -2, "load");
%}

%attribute(binding::Model, const char*, name, getName, setName);

namespace binding
{

class Model
{
public:
    const char* getName() const;

    void setName(const char *name);
};

class ModelLoader
{
public:
    %newobject operator();
    Model* operator()(const char* filename);
};

}
