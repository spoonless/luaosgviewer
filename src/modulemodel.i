%nodefaultctor;

#ifndef %mangle
#define %mangle(Type...) #@Type
#endif

%include <typemaps/attribute.swg>

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

%attribute(binding::Model, const char*, name, getName, setName);

namespace binding
{

class Model
{
public:
    const char* getName() const;

    void setName(const char *name);
};

%newobject loadModel;
%rename(load) loadModel;
Model* loadModel(const char* filename);

}
