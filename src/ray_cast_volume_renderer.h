#ifndef RAY_CAST_VOLUME_RENDERER_H
#define RAY_CAST_VOLUME_RENDERER_H

#include "volume_renderer.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFramebufferObject>
#include <memory>


class RayCastVolumeRenderer : public VolumeRenderer
{
  public:
    RayCastVolumeRenderer(void)
      : VolumeRenderer()
      , m_prog_ray_cast()
      , m_vao()
      , m_vbo_cube(QOpenGLBuffer::VertexBuffer)
      , m_ibo_cube(QOpenGLBuffer::IndexBuffer)
      , m_default_fbo(0)
      , m_fbo(0)
      , m_color_attach(0)
    {
      // tieto 2 prikazy v skutocnosti ani nie us potrebne,
      // pretoze StaticDraw je pre QOpenGLBuffer defaultny
      m_vbo_cube.setUsagePattern(QOpenGLBuffer::StaticDraw);
      m_ibo_cube.setUsagePattern(QOpenGLBuffer::StaticDraw);
    }

    ~RayCastVolumeRenderer(void);

    virtual bool resize(QRect rect) override;

  protected:
    virtual bool reset_impl(void) override;
    virtual void render_impl(const QQuaternion & rotation,
                             const QVector3D & scale,
                             const QVector3D & translation,
                             float peel_depth,
                             int slice_count) override;

  private:
    bool initFramebuffer(int w, int h);

  private:
    QOpenGLShaderProgram m_prog_gen_back_faces;
    QOpenGLShaderProgram m_prog_ray_cast;

    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo_cube;
    QOpenGLBuffer m_ibo_cube;

    GLuint m_default_fbo;       // id defaultneho frame buffer objektu (aby som vedel ako zresetovat frame buffer
                                // po skonceni kreslenia do mojho frame bufer-u, pretoze QOpenGLWidget interne pouziva
                                // na kreslenie svoj vlastny frame buffer)
    GLuint m_fbo;
    GLuint m_color_attach;
};

#endif // RAY_CAST_VOLUME_RENDERER_H
