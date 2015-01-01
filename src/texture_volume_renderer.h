#ifndef TEXTURE_VOLUME_RENDERER_H
#define TEXTURE_VOLUME_RENDERER_H

#include "volume_renderer.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>


class TextureVolumeRenderer : public VolumeRenderer
{
  public:
    TextureVolumeRenderer(void)
      : VolumeRenderer()
      , m_vao()
      , m_vbo(QOpenGLBuffer::VertexBuffer)
      , m_program()
      , m_prog_bbox()
      , m_prog_rectangle()
    {
      // tento prikaz v skutocnosti ani nie je potrebny,
      // pretoze StaticDraw je pre QOpenGLBuffer defaultny
      m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    }

  protected:
    virtual bool reset_impl(void) override;
    virtual void render_impl(const QQuaternion & rotation,
                             const QVector3D & scale,
                             const QVector3D & translation,
                             float peel_depth,
                             int slice_count) override;

  private:
    void renderBBox(const QQuaternion &rotation, const QVector3D &scale, const QVector3D &translation);

  private:
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;
    QOpenGLShaderProgram m_program;
    QOpenGLShaderProgram m_prog_bbox;
    QOpenGLShaderProgram m_prog_rectangle;
};

#endif // TEXTURE_VOLUME_RENDERER_H
