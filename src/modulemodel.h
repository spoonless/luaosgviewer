#ifndef MODULEMODEL_H
#define MODULEMODEL_H

#include "osg/ref_ptr"
#include "osg/Node"

namespace binding
{

class Model
{
public:
    Model(osg::Node *node);
    Model(const Model &model);

    osg::Node* getNode()
    {
        return _node.get();
    }

    const osg::Node* getNode() const
    {
        return _node.get();
    }

    void setName(const char *name)
    {
        if (_node.valid())
        {
            _node->setName(name);
        }
    }

    const char* getName() const
    {
        return !_node ? 0 : _node->getName().c_str();
    }

private:
    osg::ref_ptr<osg::Node> _node;
};

Model* loadModel(const char* filename);

}

#endif // MODULEMODEL_H
