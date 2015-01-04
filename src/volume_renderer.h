#ifndef VOLUME_RENDERER_H
#define VOLUME_RENDERER_H

#include "volume_data.h"

#include <QMatrix4x4>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>


class TransferFunction;
class QQuaternion;
class QVector3D;

class VolumeRenderer
{
  public:
    VolumeRenderer(void)
      : m_proj()
      , m_data()
      , m_transfer_func(QOpenGLTexture::Target1D)
      , m_use_transfer_func(false)
      , m_use_lighting(false)
      , m_light_pos(QVector3D(0.0f, 1.0f, 1.0f))
      , m_light_ambient_col(QVector3D(0.0f, 0.0f, 0.0f))
      , m_light_diffuse_col(QVector3D(0.0f, 0.0f, 0.0f))
      , m_max_texture_size(0)
      , m_prog_bbox()
      , m_render_bbox(true)
    { }

    virtual ~VolumeRenderer(void) { }

    bool setVolumeData(const VolumeData & data) { return data.toVolumeDataOGL(m_data, m_use_lighting); }
    void setRenderBBox(bool enabled) { m_render_bbox = enabled; }
    void setUseTransferFunction(bool enabled) { m_use_transfer_func = enabled; }
    void setUseLighting(bool enabled) { m_use_lighting = enabled; }
    void setLightPosition(const QVector3D & light_pos) { m_light_pos = light_pos; }
    void setLightAmbientColor(const QVector3D & ambient_col) { m_light_ambient_col = ambient_col; }
    void setLightDiffuseColor(const QVector3D & diffuse_col) { m_light_diffuse_col = diffuse_col; }

    void setProjection(const QMatrix4x4 & proj) { m_proj = proj; }
    void setPerspectiveProjection(int width, int height)
    {
      m_proj.setToIdentity();
      m_proj.perspective(30.0f, ((float) width) / ((float) height), 0.01f, 1000.0f);
    }

    bool reset(int w, int h);
    bool resize(QRect rect);
    bool resize(int w, int h) { return resize(QRect(0, 0, w, h)); }

    void renderPreview(const QQuaternion & rotation,
                       const QVector3D & scale,
                       const QVector3D & translation,
                       const float peel_depth = 0.0f)
    { return render(rotation, scale, translation, m_data.maxSize() / 4, peel_depth); }

    void render(const QQuaternion & rotation,
                const QVector3D & scale,
                const QVector3D & translation,
                const int detail,
                const float peel_depth = 0.0f);

    void renderBBox(const QQuaternion & rotation, const QVector3D & scale, const QVector3D & translation);
    bool uploadTransferFunction(const TransferFunction & transfer_func);

  protected:
    virtual bool resize_impl(QRect /* rect */) { return true; }
    virtual bool reset_impl(int w, int h) = 0;
    virtual void render_impl(const QQuaternion & rotation,
                             const QVector3D & scale,
                             const QVector3D & translation,
                             float peel_depth,
                             int detail) = 0;

  protected:
    QMatrix4x4 m_proj;
    VolumeDataOGL m_data;
    QOpenGLTexture m_transfer_func;
    bool m_use_transfer_func;
    bool m_use_lighting;
    QVector3D m_light_pos;
    QVector3D m_light_ambient_col;
    QVector3D m_light_diffuse_col;

  private:
    int m_max_texture_size;
    QOpenGLShaderProgram m_prog_bbox;
    bool m_render_bbox;
};

#endif // VOLUME_RENDERER_H
