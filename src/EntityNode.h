#ifndef ENTITYNODE_H
#define ENTITYNODE_H

#include "osg/observer_ptr"
#include "osg/Group"
#include "LuaState.h"

class EntityNode;
class EventHandlers;

typedef int LuaEventHandlersRef;

class EntityLibrary : public LuaExtensionLibrary
{
public:
    EntityLibrary();

    virtual int open(LuaState &luaState);

    EventHandlers* createEventHandlers();

    void copyEventHandlers(const EventHandlers *from, EventHandlers *to);

    void fireEvent(EventHandlers *eventHandlers, const char *event, EntityNode *target);

    void freeInternalRef(LuaEventHandlersRef ref);

private:
    LuaState *_luaState;
};


class EventHandlers : public osg::Referenced
{
    friend class EntityLibrary;
public:
    EventHandlers(const EventHandlers&);

    inline void fireEvent(const char *event, EntityNode *target)
    {
        this->_entityLibrary->fireEvent(this, event, target);
    }

    operator LuaEventHandlersRef() const
    {
        return _eventHandlersRef;
    }

protected:
    EventHandlers(EntityLibrary *entityLibrary, LuaEventHandlersRef internalRef);
    virtual ~EventHandlers();

private:
    EntityLibrary *_entityLibrary;
    LuaEventHandlersRef _eventHandlersRef;
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

    EventHandlers* getOrCreateEventHandlers(EntityLibrary *entityLibrary);

    inline EventHandlers* getEventHandlers()
    {
        return _eventHandlers;
    }

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
