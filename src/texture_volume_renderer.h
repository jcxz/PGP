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
      , m_attr_pos(0)
      , m_attr_tex_coords(0)
      , m_program()
      , m_prog_bbox()
      , m_tex_vol_data()
      , m_tex_transfer_func(0)
      , m_mouse_start()
      , m_angle_x(0.0f)
      , m_angle_y(0.0f)
      , m_dist_z(-10.0f)
      , m_depth_data(0.0f)
    { }

    ~TextureVolumeRenderer(void);

    virtual bool reset(void) override;
    virtual void render(const QQuaternion & rotation, const QVector3D & scale, const QVector3D & translation) override;

  private:
    void renderBBox(const QQuaternion &rotation, const QVector3D &scale, const QVector3D &translation);
    void genTransferFunc(void);

  private:
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_attr_pos;
    GLuint m_attr_tex_coords;
    QOpenGLShaderProgram m_program;
    QOpenGLShaderProgram m_prog_bbox;
    Texture3D m_tex_vol_data;
    GLuint m_tex_transfer_func;

    QPoint m_mouse_start;
    float m_angle_x;
    float m_angle_y;
    float m_dist_z;
    float m_depth_data;
};

#endif // TEXTURE_VOLUME_RENDERER_H
