#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include "osg/Group"
#include "scriptloadercallback.h"

osg::Node* lua_toNode(lua_State* L);

class NodeScriptResultHandler : public ScriptResultHandler
{
public:
    NodeScriptResultHandler();
    void handle(lua_State* luaState);

    osg::ref_ptr<osg::Node> node;
};

NodeScriptResultHandler::NodeScriptResultHandler() : ScriptResultHandler(1)
{

}

void NodeScriptResultHandler::handle(lua_State* luaState)
{
    node = lua_toNode(luaState);
}

ScriptLoaderCallback::ScriptLoaderCallback(ScriptEngine *scriptEngine, bool reloadable) :
    _scriptEngine(scriptEngine), _reloadable(reloadable), _fileLoaded(false), _lastModificationDate(0)
{
}

ScriptLoaderCallback::~ScriptLoaderCallback()
{
}

void ScriptLoaderCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
    osg::ref_ptr<osg::Group> groupNode =  node->asGroup();
    if (!groupNode || !this->_scriptEngine || this->_filename.empty()) {
        osg::NodeCallback::operator ()(node, nv);
        return;
    }

    if (! this->_fileLoaded)
    {
        std::ifstream ifstream(this->_filename.c_str());
        NodeScriptResultHandler handler;
        if (this->_scriptEngine->exec(handler, ifstream, this->_filename.c_str()))
        {
            groupNode->addChild(handler.node);
            this->_lastModificationDate = this->getLastModificationDate();
            this->_fileLoaded = true;
        }
    }
    else if (this->_reloadable)
    {
        if (nv->getTraversalNumber() % 60 == 0)
        {
            time_t lastModificationDate = this->getLastModificationDate();
            if (this->_lastModificationDate != lastModificationDate)
            {
                groupNode->removeChildren(0, groupNode->getNumChildren());
                this->_lastModificationDate = lastModificationDate;
                this->_fileLoaded = false;
            }
        }
    }
    osg::NodeCallback::operator ()(node, nv);
}

#ifdef WIN32
typedef struct __stat64 Stat64;
#define stat64 _stat64
#else
typedef struct stat64 Stat64;
#endif

time_t ScriptLoaderCallback::getLastModificationDate()
{
    Stat64 filestat;
    if(stat64(this->_filename.c_str(), &filestat) == 0)
    {
        return filestat.st_mtime;
    }
    return 0;
}

