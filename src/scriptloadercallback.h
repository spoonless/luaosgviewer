#ifndef SCRIPTLOADERCALLBACK_H
#define SCRIPTLOADERCALLBACK_H

#include <time.h>
#include "osg/NodeCallback"
#include "scriptengine.h"

class ScriptLoaderCallback : public osg::NodeCallback
{
public:
    ScriptLoaderCallback(ScriptEngine *scriptEngine, bool reloadable = false);
    ~ScriptLoaderCallback();

    void operator()(osg::Node* node, osg::NodeVisitor* nv);

    void setFilename(const std::string &filename)
    {
        _filename = filename;
        _lastModificationDate = 0;
    }

private:
    time_t getLastModificationDate();

    osg::ref_ptr<ScriptEngine> _scriptEngine;
    bool _reloadable;
    bool _fileLoaded;
    time_t _lastModificationDate;
    std::string _filename;
};

#endif // SCRIPTLOADERCALLBACK_H
