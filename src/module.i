%module geometry

%include "std_string.i"

%nodefaultctor;

#ifndef %mangle
#define %mangle(Type...) #@Type
#endif

%include <typemaps/attribute.swg>

%{

#include <stdio.h>
#include "osg/ref_ptr"
#include "osg/Node"

osg::Node* geometryLoad(const char* name);

osg::Node* lua_toNode(lua_State* L)
{
    osg::ref_ptr<osg::Node> *ptr = 0;
    int res = SWIG_ConvertPtr(L, -1, (void **)(&ptr),SWIGTYPE_p_osg__ref_ptrT_osg__Node_t, 0);
    if (SWIG_IsOK(res) && ptr)
    {
        return ptr->get();
    }
    else
    {
        return 0;
    }
}

static osg::ref_ptr<osg::Node> *swig_geometryLoad(const char* name)
{
    osg::Node *node = geometryLoad(name);
    if (! node)
    {
        return 0;
    }

    osg::ref_ptr<osg::Node> *ref_ptr = new osg::ref_ptr<osg::Node>(node);
    return ref_ptr;
}


static const std::string & osg_ref_ptr_Sl_osg_Node_Sg__name_get(osg::ref_ptr<osg::Node> *ptr)
{
    return (*ptr)->getName();
}

static void osg_ref_ptr_Sl_osg_Node_Sg__name_set(osg::ref_ptr<osg::Node> *ptr, const std::string &name)
{
    (*ptr)->setName(name);
}


%}


%attribute(osg::Node, std::string, name, getName, setName);


namespace osg
{

template<class T>
class ref_ptr
{
public:
    T* operator->() const;
};

class Node
{
public:
    const std::string& getName() const;
    void setName(const std::string&);

protected:
    virtual ~Node();
};

}


%template(osgRefPtrNode) osg::ref_ptr<osg::Node>;

%rename(load) swig_geometryLoad;
%newobject swig_geometryLoad;
osg::ref_ptr<osg::Node> *swig_geometryLoad(const char* name);
