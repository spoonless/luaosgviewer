#include <string.h>
#include <iostream>
#include <fstream>
#include "osg/Group"
#include "osgViewer/Viewer"
#include "ScriptLoaderCallback.h"
#include "StaticImport.h"


#include "osgText/Text"
#include "osgUtil/LineSegmentIntersector"
#include "EntityNode.h"


class OnSightCallback : public osg::NodeCallback
{
public:

    OnSightCallback()
    {
        _intersector = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::VIEW, osg::Vec3(0.5,0.5,0.0), osg::Vec3(0.5,0.5,-20.0));
        _intersector->setIntersectionLimit(osgUtil::Intersector::LIMIT_NEAREST);
        _intersectionVisitor = new osgUtil::IntersectionVisitor(_intersector);
    }

    void setSightLimit(double limit)
    {
        _intersector->setEnd(osg::Vec3(0.5,0.5,-limit));
    }

    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
    {
        node->accept(*_intersectionVisitor);
        if ( _intersector->containsIntersections() )
        {
            doUserOperations(_intersector->getFirstIntersection().nodePath);
        }
        else
        {
            reset();
        }
        _intersectionVisitor->reset();
        _intersector->reset();
    }

protected:
    virtual void doUserOperations(const osg::NodePath &nodePath) = 0;
    virtual void reset() = 0;

private:
    osg::ref_ptr<osgUtil::IntersectionVisitor> _intersectionVisitor;
    osg::ref_ptr<osgUtil::LineSegmentIntersector> _intersector;

};

class ExperimentalSightHandler : public OnSightCallback
{
public:
    ExperimentalSightHandler(osgText::Text *text) : _console(text)
    {
        this->setSightLimit(3.0);
    }

protected:
    virtual void doUserOperations(const osg::NodePath &nodePath)
    {
        bool nodeFound = true;
        if (_nodeOnSight != nodePath.back())
        {
            nodeFound = false;
            _nodeOnSight = nodePath.back();
            for (osg::NodePath::const_reverse_iterator it = nodePath.rbegin(); !nodeFound && it != nodePath.rend() ; ++it)
            {
                osg::Node *node = *it;
                _entityOnSight = dynamic_cast<EntityNode*>(node);
                if (_entityOnSight.valid())
                {
                    nodeFound = true;
                    _entityOnSight->fireEvent("focusin");
                    _console->setText(_entityOnSight->getName());
                }
            }
        }
        if (! nodeFound)
        {
            reset();
        }
    }

    virtual void reset()
    {
        _nodeOnSight = 0;
        _entityOnSight = 0;
        _console->setText("");
    }

private:
    osg::ref_ptr<osgText::Text> _console;
    osg::ref_ptr<EntityNode> _entityOnSight;
    osg::ref_ptr<osg::Node> _nodeOnSight;
};

osgText::Text* createText( const osg::Vec3& pos, const std::string& content, float size )
{
    osg::ref_ptr<osgText::Text> text = new osgText::Text;
    text->setDataVariance( osg::Object::DYNAMIC );
    text->setCharacterSize( size );
    text->setAxisAlignment( osgText::TextBase::XY_PLANE );
    text->setPosition( pos );
    text->setText( content );
    return text.release();
}

osg::Camera* createHUDCamera( double left, double right, double bottom, double top )
{
    osg::ref_ptr<osg::Camera> camera = new osg::Camera;
    camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
    camera->setClearMask( GL_DEPTH_BUFFER_BIT );
    camera->setRenderOrder( osg::Camera::POST_RENDER );
    camera->setAllowEventFocus( false );
    camera->setProjectionMatrix(
    osg::Matrix::ortho2D(left, right, bottom, top) );
    camera->getOrCreateStateSet()->setMode(
    GL_LIGHTING, osg::StateAttribute::OFF );
    return camera.release();
}

osg::Node* createHud(osgText::Text *text)
{
    osg::ref_ptr<osg::Geode> textGeode = new osg::Geode;
    textGeode->addDrawable( text );
    osg::ref_ptr<osg::Camera> hudCamera = createHUDCamera(0, 800, 0, 600);
    hudCamera->addChild( textGeode.get() );
    return hudCamera.release();
}


int main (int argc, const char** argv) {
    if (argc > 1)
    {
        OSG_WARN << "loading scene from " << argv[1] << "\n";

        osg::ref_ptr<osgText::Text> text = createText(osg::Vec3(50.0f, 50.0f, 0.0f), argv[1], 12.0f);

        osg::ref_ptr<osg::Group> world = new osg::Group;
        osg::ref_ptr<ScriptEngine> scriptEngine = new ScriptEngine;
        scriptEngine->openLibrary<EntityLibrary>("entity");
        osg::ref_ptr<ScriptLoaderCallback> callback = new ScriptLoaderCallback(scriptEngine, true);
        callback->setFilename(argv[1]);
        world->addUpdateCallback(callback);

        osg::ref_ptr<osg::Group> layers = new osg::Group;
        layers->setDataVariance(osg::Object::DYNAMIC);
        layers->addChild(world);
        layers->addChild(createHud(text));
        osgViewer::Viewer viewer;
        viewer.setSceneData(layers);
        viewer.getCamera()->addUpdateCallback(new ExperimentalSightHandler(text));
        viewer.setUpViewInWindow(0, 0, 800, 600);
        return viewer.run();
    }
    return 0;
}
