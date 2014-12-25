#ifndef TEXTURE_VOLUME_RENDERER_H
#define TEXTURE_VOLUME_RENDERER_H

#include "volume_renderer.h"
#include "texture3d.h"

#include <QOpenGLShaderProgram>


class TextureVolumeRenderer : public VolumeRenderer
{
  public:
    TextureVolumeRenderer(void)
      : VolumeRenderer()
      , m_vao(0)
      , m_vbo(0)
      , m_program()
      , m_prog_bbox()
      , m_tex_vol_data()
      , m_tex_transfer_func(0)
      , m_depth_data(0.0f)
    { }

    ~TextureVolumeRenderer(void);

    virtual bool reset(void) override;

  protected:
    virtual void render_impl(const QQuaternion & rotation,
                             const QVector3D & scale,
                             const QVector3D & translation,
                             const float peel_depth) override;

  private:
    void renderBBox(const QQuaternion &rotation, const QVector3D &scale, const QVector3D &translation);
    void genTransferFunc(void);

  private:
    GLuint m_vao;
    GLuint m_vbo;
    QOpenGLShaderProgram m_program;
    QOpenGLShaderProgram m_prog_bbox;
    Texture3D m_tex_vol_data;
    GLuint m_tex_transfer_func;
    float m_depth_data;
};

#endif // TEXTURE_VOLUME_RENDERER_H
