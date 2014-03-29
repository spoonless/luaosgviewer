#include "osg/ref_ptr"
#include "osg/Node"
#include "osgDB/ReadFile"

osg::Node* geometryLoad(const char* name) {
    std::string filename(name);
    return osgDB::readNodeFile(filename);
}
