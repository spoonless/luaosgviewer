#include "osgDB/ReadFile"
#include "EntityNode.h"

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
        lua_rawgeti(*_scriptEngine, LUA_REGISTRYINDEX, eh._handlersReference);
        _handlersReference = luaL_ref(*_scriptEngine, LUA_REGISTRYINDEX);
    }
}

EventHandlers::~EventHandlers()
{
    if (_handlersReference != LUA_NOREF)
    {
        luaL_unref(*_scriptEngine, LUA_REGISTRYINDEX, _handlersReference);
    }
}

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
