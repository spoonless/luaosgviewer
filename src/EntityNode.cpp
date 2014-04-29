#include "osgDB/ReadFile"
#include "luaX.h"
#include "EntityNode.h"
#include "osgLuaBinding.h"

extern "C" {

int luaopen_entity(lua_State*);

}

EntityLibrary::EntityLibrary() : _luaState(0)
{
}

int EntityLibrary::open(LuaState &luaState)
{
    _luaState = &luaState;
    return luaopen_entity(static_cast<lua_State*>(*_luaState));
}

void EntityLibrary::freeInternalRef(LuaEventHandlersRef ref)
{
    if (_luaState->good() && ref != LUA_NOREF)
    {
        luaL_unref(*_luaState, LUA_REGISTRYINDEX, ref);
    }
}

EventHandlers* EntityLibrary::createEventHandlers()
{
    if (!_luaState->good())
    {
        return 0;
    }

    lua_newtable (*_luaState);
    LuaEventHandlersRef ref = luaL_ref(*_luaState, LUA_REGISTRYINDEX);

    return new EventHandlers(this, ref);
}

void EntityLibrary::fireEvent(EventHandlers *eventHandlers, const char *event, EntityNode *target)
{
    if (!_luaState->good())
    {
        return;
    }

    MARK_LUA_STACK(*_luaState);
    int nbPop = 0;
    if(eventHandlers->_eventHandlersRef != LUA_NOREF && eventHandlers->_eventHandlersRef != LUA_REFNIL)
    {
        lua_rawgeti(*_luaState, LUA_REGISTRYINDEX, eventHandlers->_eventHandlersRef);
        ++nbPop;
        if(lua_istable(*_luaState, -1))
        {
            lua_getfield(*_luaState, -1, event);
            ++nbPop;
            if (lua_isfunction(*_luaState, -1))
            {
                lua_pushNode(*_luaState, target);
                --nbPop;
                if (lua_pcall(*_luaState, 1, 0, 0))
                {
                    ++nbPop;
                }
            }
        }
        lua_pop(*_luaState, nbPop);
    }
    CHECK_LUA_STACK(*_luaState);
}

void EntityLibrary::copyEventHandlers(const EventHandlers *from, EventHandlers *to)
{
    MARK_LUA_STACK(*_luaState);
    if(_luaState->good() && from->_eventHandlersRef != LUA_NOREF && lua_checkstack(*_luaState, 6))
    {
        if (to->_eventHandlersRef == LUA_NOREF)
        {
            lua_newtable (*_luaState);
            lua_pushvalue(*_luaState, -1);
            to->_eventHandlersRef = luaL_ref(*_luaState, LUA_REGISTRYINDEX);
        }
        else
        {
            lua_rawgeti(*_luaState, LUA_REGISTRYINDEX, to->_eventHandlersRef);
        }
        lua_rawgeti(*_luaState, LUA_REGISTRYINDEX, from->_eventHandlersRef);
        lua_pushnil(*_luaState);
        while (lua_next(*_luaState, -2))
        {
            lua_pushvalue(*_luaState, -2);
            lua_pushvalue(*_luaState, -2);
            lua_rawset(*_luaState, -6);
            lua_pop(*_luaState, 1);
        }
        lua_pop(*_luaState, 2);
    }
    CHECK_LUA_STACK(*_luaState);
}

EventHandlers::EventHandlers(EntityLibrary *entityLibrary, LuaEventHandlersRef internalRef) : _entityLibrary(entityLibrary), _eventHandlersRef(internalRef)
{
}

EventHandlers::EventHandlers(const EventHandlers &eh) : _entityLibrary(eh._entityLibrary), _eventHandlersRef(LUA_NOREF)
{
    _entityLibrary->copyEventHandlers(&eh, this);
}

EventHandlers::~EventHandlers()
{
    _entityLibrary->freeInternalRef(_eventHandlersRef);
}

EntityNode::EntityNode(osg::Node *node)
{
    this->addChild(node);
}

EntityNode::EntityNode(const EntityNode &entity,const osg::CopyOp& copyop) : osg::Group(entity, copyop)
{
    if (entity._eventHandlers.valid())
    {
        this->_eventHandlers = new EventHandlers(*entity._eventHandlers);
    }
}

EntityNode::~EntityNode()
{

}

EventHandlers* EntityNode::getOrCreateEventHandlers(EntityLibrary *entityLibrary)
{
    if(_eventHandlers.valid())
    {
        return _eventHandlers;
    }
    else
    {
        _eventHandlers = entityLibrary->createEventHandlers();
    }
    return _eventHandlers.get();
}


EntityNode* EntityLoader::operator()(const char* filename)
{
    osg::ref_ptr<osgDB::Options> options = new osgDB::Options;
    options->setObjectCacheHint(osgDB::Options::CACHE_ALL);
    osg::Node *node = osgDB::readNodeFile(filename, options);
    return node == 0 ? 0 : new EntityNode(node);
}
