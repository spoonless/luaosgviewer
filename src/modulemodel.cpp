#include "osgDB/ReadFile"
#include "modulemodel.h"

binding::Model::Model(osg::Node *node) : _node(node)
{
}

binding::Model::Model(const Model &model) : _node(model._node)
{
}

binding::Model* binding::ModelLoader::operator()(const char* filename)
{
    osg::Node *node = osgDB::readNodeFile(filename);
    return node == 0 ? 0 : new binding::Model(node);
}
