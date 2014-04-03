#include "osgDB/ReadFile"
#include "ModelLibrary.h"

binding::Model::Model(osg::Node *node) : _node(node)
{
}

binding::Model::Model(const Model &model) : _node(model._node)
{
}

binding::Model* binding::ModelLoader::operator()(const char* filename)
{
    osg::ref_ptr<osgDB::Options> options = new osgDB::Options;
    options->setObjectCacheHint(osgDB::Options::CACHE_ALL);
    osg::Node *node = osgDB::readNodeFile(filename, options);
    return node == 0 ? 0 : new binding::Model(node);
}
