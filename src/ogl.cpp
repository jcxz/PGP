#include "ogl.h"

#include <iostream>


namespace ogl {

//QOpenGLFunctions_4_3_Core *g_ogl_funcs = nullptr;
QOpenGLFunctions_3_3_Core *g_ogl_funcs = nullptr;


bool initOGLFunctions(QOpenGLContext *ctx)
{
  if (g_ogl_funcs) return true;

  //g_ogl_funcs = ctx->versionFunctions<QOpenGLFunctions_4_3_Core>();
  g_ogl_funcs = ctx->versionFunctions<QOpenGLFunctions_3_3_Core>();
  if (!g_ogl_funcs)
  {
    std::cerr << "Could not obtain OpenGL versions object" << std::endl;
    return false;
  }

  if (!g_ogl_funcs->initializeOpenGLFunctions())
  {
    std::cerr << "Failed to initialize OpenGL functions" << std::endl;
    return false;
  }

  return true;
}

} // End of ogl namespace
