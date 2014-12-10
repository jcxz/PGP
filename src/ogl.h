#ifndef OGL_H
#define OGL_H

//#include <QtGui/QOpenGLFunctions_4_3_Core>
#include <QtGui/QOpenGLFunctions_3_3_Core>
#include <cassert>


namespace ogl {

//extern QOpenGLFunctions_4_3_Core *g_ogl_funcs;
extern QOpenGLFunctions_3_3_Core *g_ogl_funcs;

bool initOGLFunctions(QOpenGLContext *ctx);

} // End of ogl namespace

#define OGLF (assert(ogl::g_ogl_funcs != nullptr), ogl::g_ogl_funcs)

#endif // OGL_H
