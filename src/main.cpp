#include <string.h>
#include <iostream>
#include <fstream>
#include "osg/Group"
#include "osgViewer/Viewer"
#include "ScriptLoaderCallback.h"

#ifdef OSG_LIBRARY_STATIC

#include <osgDB/ReadFile>

USE_OSGPLUGIN(osg)
USE_OSGPLUGIN(osg2)

// include the platform specific GraphicsWindow implementation.
USE_GRAPHICSWINDOW()

#endif

int main (int argc, const char** argv) {
    if (argc > 1) {
        osg::ref_ptr<osg::Group> group = new osg::Group;
        group->setDataVariance(osg::Object::DYNAMIC);
        osg::ref_ptr<ScriptLoaderCallback> callback = new ScriptLoaderCallback(new ScriptEngine, true);
        callback->setFilename(argv[1]);
        group->addUpdateCallback(callback);
        osgViewer::Viewer viewer;
        viewer.setSceneData(group);
        viewer.setUpViewInWindow(0, 0, 800, 600);
        return viewer.run();
    }
    return 0;
}
