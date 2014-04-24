#ifndef ENTITYNODE_H
#define ENTITYNODE_H

#include "osg/observer_ptr"
#include "osg/Group"
#include "ScriptEngine.h"


class EntityScriptLibrary : public ScriptLibrary
{
public:
    virtual const char* getName() const;

    virtual int open(ScriptEngine *scriptEngine);
};

class EntityNode;

typedef int EventHandlersInternalRef;

class EventHandlers : public osg::Referenced
{
public:
    EventHandlers(ScriptEngine *scriptEngine);
    EventHandlers(const EventHandlers&);

    void fireEvent(const char *event, EntityNode *target);

    EventHandlersInternalRef getEventHandlersInternalRef();

protected:
    ScriptEngine* lockScriptEngine();
    virtual ~EventHandlers();

private:
    osg::observer_ptr<ScriptEngine> _scriptEngine;
    EventHandlersInternalRef _handlersReference;
};


class EntityNode : public osg::Group
{
public:
    EntityNode(osg::Node* node = 0);
    EntityNode(const EntityNode&, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

    META_Node(binding, EntityNode)

    /* tiny hack to make bindings easier to implement */
    inline const char* getNameAsCharArray() const
    {
        return this->getName().c_str();
    }

    inline void fireEvent(const char* event)
    {
        if(_eventHandlers.valid()) _eventHandlers->fireEvent(event, this);
    }

    EventHandlers* getOrCreateEventHandlers(ScriptEngine *scriptEngine);

protected:
    virtual ~EntityNode();

private:
    osg::ref_ptr<EventHandlers> _eventHandlers;
};

class EntityLoader
{
public:
    EntityNode* operator()(const char* filename);
};


#endif // ENTITYNODE_H
