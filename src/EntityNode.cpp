#include "osgDB/ReadFile"
#include "EntityNode.h"

EntityNode::EntityNode(osg::Node *node)
{
    this->addChild(node);
}

EntityNode::EntityNode(const EntityNode &entity,const osg::CopyOp& copyop) : osg::Group(entity, copyop)
{

}

EntityNode::~EntityNode()
{

}

EntityNode* EntityLoader::operator()(const char* filename)
{
    osg::ref_ptr<osgDB::Options> options = new osgDB::Options;
    options->setObjectCacheHint(osgDB::Options::CACHE_ALL);
    osg::Node *node = osgDB::readNodeFile(filename, options);
    return node == 0 ? 0 : new EntityNode(node);
}
