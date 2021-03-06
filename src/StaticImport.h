#ifndef STATIC_IMPORT_H
#define STATIC_IMPORT_H

#ifdef OSG_LIBRARY_STATIC

#include <osgDB/Registry>

// include the plugins we need
//USE_OSGPLUGIN(ive)
USE_OSGPLUGIN(osg)
USE_OSGPLUGIN(osg2)
USE_OSGPLUGIN(rgb)
USE_OSGPLUGIN(obj)
//USE_OSGPLUGIN(freetype)
//USE_OSGPLUGIN(OpenFlight)

USE_DOTOSGWRAPPER_LIBRARY(osg)
//USE_DOTOSGWRAPPER_LIBRARY(osgFX)
//USE_DOTOSGWRAPPER_LIBRARY(osgParticle)
//USE_DOTOSGWRAPPER_LIBRARY(osgShadow)
//USE_DOTOSGWRAPPER_LIBRARY(osgSim)
//USE_DOTOSGWRAPPER_LIBRARY(osgTerrain)
//USE_DOTOSGWRAPPER_LIBRARY(osgText)
//USE_DOTOSGWRAPPER_LIBRARY(osgViewer)
//USE_DOTOSGWRAPPER_LIBRARY(osgVolume)
//USE_DOTOSGWRAPPER_LIBRARY(osgWidget)

USE_SERIALIZER_WRAPPER_LIBRARY(osg)
//USE_SERIALIZER_WRAPPER_LIBRARY(osgAnimation)
//USE_SERIALIZER_WRAPPER_LIBRARY(osgFX)
//USE_SERIALIZER_WRAPPER_LIBRARY(osgManipulator)
//USE_SERIALIZER_WRAPPER_LIBRARY(osgParticle)
//USE_SERIALIZER_WRAPPER_LIBRARY(osgShadow)
//USE_SERIALIZER_WRAPPER_LIBRARY(osgSim)
//USE_SERIALIZER_WRAPPER_LIBRARY(osgTerrain)
//USE_SERIALIZER_WRAPPER_LIBRARY(osgText)
//USE_SERIALIZER_WRAPPER_LIBRARY(osgVolume)

// include the platform specific GraphicsWindow implementation.
USE_GRAPHICSWINDOW()

#endif

#endif // STATIC_IMPORT_H
