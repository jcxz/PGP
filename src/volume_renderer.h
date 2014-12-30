#ifndef VOLUME_RENDERER_H
#define VOLUME_RENDERER_H

#include "volume_data.h"

#include <QMatrix4x4>
#include <QOpenGLTexture>


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
      , m_slice_count(0)
    { }

    virtual ~VolumeRenderer(void) { }

    void setSliceCount(int slice_count) { m_slice_count = slice_count; }

    void setProjection(const QMatrix4x4 & proj) { m_proj = proj; }
    void setPerspectiveProjection(int width, int height)
    {
      m_proj.setToIdentity();
      m_proj.perspective(30.0f, ((float) width) / ((float) height), 0.01f, 1000.0f);
    }

    bool setVolumeData(const VolumeData & data) { return data.toVolumeDataOGL(m_data); }

    bool uploadTransferFunction(const TransferFunction & transfer_func);

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
      return render_impl(rotation, scale, translation, peel_depth, m_slice_count);
    }

    virtual bool reset(void) = 0;

  protected:
    virtual void render_impl(const QQuaternion & rotation,
                             const QVector3D & scale,
                             const QVector3D & translation,
                             float peel_depth,
                             int slice_count) = 0;

  protected:
    QMatrix4x4 m_proj;
    VolumeDataOGL m_data;
    QOpenGLTexture m_transfer_func;

  private:
    int m_slice_count;
};

#endif // VOLUME_RENDERER_H
