#ifndef ENTITYNODE_H
#define ENTITYNODE_H

#include "osg/Group"


class EntityNode : public osg::Group
{
public:
    EntityNode(osg::Node* node = 0);
    EntityNode(const EntityNode&,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

    META_Node(binding, EntityNode)

    /* tiny hack to make bindings easier to implement */
    inline const char* getNameAsCharArray() const
    {
        return this->getName().c_str();
    }

protected:
    ~EntityNode();
};

class EntityLoader
{
public:
    EntityNode* operator()(const char* filename);
};


#endif // ENTITYNODE_H
