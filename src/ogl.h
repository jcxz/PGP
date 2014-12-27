#ifndef OGL_H
#define OGL_H

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_3_Core>
#include <cassert>


namespace ogl_helpers {

inline QOpenGLFunctions_3_3_Core *get_ogl_functions(void)
{
  QOpenGLContext *ctx = QOpenGLContext::currentContext();
  assert(ctx != nullptr);

  QOpenGLFunctions_3_3_Core *f = ctx->versionFunctions<QOpenGLFunctions_3_3_Core>();
  assert(f != nullptr);

  f->initializeOpenGLFunctions();

  return f;
}

inline bool init(void) { return get_ogl_functions() != nullptr; }

}

#define OGLF (ogl_helpers::get_ogl_functions())

#endif // OGL_H
