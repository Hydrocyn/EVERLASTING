#ifndef RENDERSTAGEEX_H
#define RENDERSTAGEEX_H

#include <osgUtil/RenderStage>

/// Needed for mixing osg rendering with Qt 2D drawing using QPainter...
/// See http://forum.openscenegraph.org/viewtopic.php?t=15627&view=previous

class RenderStageEx : public osgUtil::RenderStage
{
public:
    virtual void drawInner(osg::RenderInfo& renderInfo,
                           osgUtil::RenderLeaf*& previous, bool& doCopyTexture);
};

#endif // RENDERSTAGEEX_H
