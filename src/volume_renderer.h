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
      , m_detail(0)
      , m_max_texture_size(0)
      , m_prog_bbox()
    { }

    virtual ~VolumeRenderer(void) { }

    void setDetail(int level) { m_detail = level; }
    bool setVolumeData(const VolumeData & data) { return data.toVolumeDataOGL(m_data); }

    void setProjection(const QMatrix4x4 & proj) { m_proj = proj; }
    void setPerspectiveProjection(int width, int height)
    {
      m_proj.setToIdentity();
      m_proj.perspective(30.0f, ((float) width) / ((float) height), 0.01f, 1000.0f);
    }

    bool reset(int w, int h);
    bool resize(QRect rect);

    void renderPreview(const QQuaternion & rotation,
                       const QVector3D & scale,
                       const QVector3D & translation,
                       const float peel_depth = 0.0f)
    { return render_impl(rotation, scale, translation, peel_depth, m_data.maxSize() / 4); }

    void render(const QQuaternion & rotation,
                const QVector3D & scale,
                const QVector3D & translation,
                const float peel_depth = 0.0f)
    {
      return render_impl(rotation, scale, translation, peel_depth, m_detail);
    }

    void renderBBox(const QQuaternion & rotation, const QVector3D & scale, const QVector3D & translation);
    bool uploadTransferFunction(const TransferFunction & transfer_func);

  protected:
    virtual bool resize_impl(QRect /* rect */) { return true; }
    virtual bool reset_impl(void) = 0;
    virtual void render_impl(const QQuaternion & rotation,
                             const QVector3D & scale,
                             const QVector3D & translation,
                             float peel_depth,
                             int detail) = 0;

  protected:
    QMatrix4x4 m_proj;
    VolumeDataOGL m_data;
    QOpenGLTexture m_transfer_func;
    int m_width;
    int m_height;

  private:
    int m_detail;
    int m_max_texture_size;
    QOpenGLShaderProgram m_prog_bbox;
};

#endif // VOLUME_RENDERER_H
