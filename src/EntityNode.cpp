#include "osgDB/ReadFile"
#include "EntityNode.h"
#include "osgLuaBinding.h"

#include "lua.hpp"

extern "C" {

int luaopen_entity(lua_State*);

}

const char* EntityScriptLibrary::getName() const
{
    return "entity";
}

int EntityScriptLibrary::open(ScriptEngine *scriptEngine)
{
    return luaopen_entity(*scriptEngine);
}

EventHandlers::EventHandlers(ScriptEngine *scriptEngine) : _scriptEngine(scriptEngine), _handlersReference(LUA_NOREF)
{
}

EventHandlers::EventHandlers(const EventHandlers &eh) : _scriptEngine(eh._scriptEngine), _handlersReference(LUA_NOREF)
{
    if (eh._handlersReference != LUA_NOREF)
    {
        osg::ref_ptr<ScriptEngine> scriptEngine = this->lockScriptEngine();
        if (scriptEngine.valid() && lua_checkstack(*scriptEngine, 3))
        {
            lua_newtable(*scriptEngine);
            lua_createtable(*scriptEngine, 0, 1);
            lua_rawgeti(*scriptEngine, LUA_REGISTRYINDEX, eh._handlersReference);
            lua_setfield(*scriptEngine, -2, "__index");
            lua_setmetatable(*scriptEngine, -2);
            _handlersReference = luaL_ref(*scriptEngine, LUA_REGISTRYINDEX);
        }
    }
}

EventHandlers::~EventHandlers()
{
    if (_handlersReference != LUA_NOREF)
    {
        osg::ref_ptr<ScriptEngine> scriptEngine = this->lockScriptEngine();
        if (scriptEngine.valid())
        {
            luaL_unref(*scriptEngine, LUA_REGISTRYINDEX, _handlersReference);
        }
    }
}

void EventHandlers::fireEvent(const char *event, EntityNode *target)
{
    osg::ref_ptr<ScriptEngine> scriptEngine = this->lockScriptEngine();
    int nbPop = 0;
    if(scriptEngine.valid() && _handlersReference != LUA_NOREF && _handlersReference != LUA_REFNIL)
    {
        lua_rawgeti(*scriptEngine, LUA_REGISTRYINDEX, _handlersReference);
        ++nbPop;
        if(lua_istable(*scriptEngine, -1))
        {
            lua_getfield(*scriptEngine, -1, event);
            ++nbPop;
            if (lua_isfunction(*scriptEngine, -1))
            {
                lua_pushNode(*scriptEngine, target);
                --nbPop;
                if (lua_pcall(*scriptEngine, 1, 0, 0))
                {
                    ++nbPop;
                }
            }
        }
        lua_pop(*scriptEngine, nbPop);
    }
}

EventHandlersInternalRef EventHandlers::getEventHandlersInternalRef()
{
    if (_handlersReference == LUA_NOREF)
    {
        osg::ref_ptr<ScriptEngine> scriptEngine = this->lockScriptEngine();
        if (scriptEngine.valid())
        {
            lua_newtable (*scriptEngine);
            _handlersReference = luaL_ref(*scriptEngine, LUA_REGISTRYINDEX);
        }
    }
    return _handlersReference;
}


ScriptEngine* EventHandlers::lockScriptEngine()
{
    osg::ref_ptr<ScriptEngine> lock;
    _scriptEngine.lock(lock);
    return lock.release();
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

EventHandlers* EntityNode::getOrCreateEventHandlers(ScriptEngine *scriptEngine)
{
    if(_eventHandlers.valid())
    {
        return _eventHandlers;
    }
    else
    {
        _eventHandlers = new EventHandlers(scriptEngine);
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
