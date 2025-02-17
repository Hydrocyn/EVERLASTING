#ifndef STATEEX_H
#define STATEEX_H

#include <osg/State>

/// Needed for mixing osg rendering with Qt 2D drawing using QPainter...
/// See http://forum.openscenegraph.org/viewtopic.php?t=15627&view=previous

class StateEx : public osg::State
{
public:
    StateEx() : defaultFbo(0) {}
    inline void setDefaultFbo(GLuint fbo)
    {
        defaultFbo = fbo;
    }
    inline GLuint getDefaultFbo() const
    {
        return defaultFbo;
    }

protected:
    GLuint defaultFbo;
};

#endif // STATEEX_H
