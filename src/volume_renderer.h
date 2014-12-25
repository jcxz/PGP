#ifndef VOLUME_RENDERER_H
#define VOLUME_RENDERER_H

#include <QMatrix4x4>

class VolumeData;
class QQuaternion;
class QVector3D;

class VolumeRenderer
{
  public:
    VolumeRenderer(void)
      : m_proj()
      , m_data(nullptr)
    { }

    virtual ~VolumeRenderer(void) { }

    void setProjection(const QMatrix4x4 & proj) { m_proj = proj; }
    void setPerspectiveProjection(int width, int height)
    {
      m_proj.setToIdentity();
      m_proj.perspective(30.0f, ((float) width) / ((float) height), 0.01f, 1000.0f);
    }

    void setVolumeData(VolumeData *data) { m_data = data; }

    void render(const QQuaternion & rotation,
                const QVector3D & scale,
                const QVector3D & translation,
                const float peel_depth = 0.0f)
    { return render_impl(rotation, scale, translation, peel_depth); }

    virtual bool reset(void) = 0;

  protected:
    virtual void render_impl(const QQuaternion & rotation,
                             const QVector3D & scale,
                             const QVector3D & translation,
                             const float peel_depth) = 0;

  protected:
    QMatrix4x4 m_proj;
    VolumeData *m_data;
};

#endif // VOLUME_RENDERER_H
