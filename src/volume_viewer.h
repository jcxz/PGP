#ifndef VOLUME_VIEWER_H
#define VOLUME_VIEWER_H

#include "logger.h"
#include "trackball.h"
#include "volume_renderer.h"

#include <QOpenGLWidget>
#include <memory>


class VolumeRenderer;

class VolumeViewer : public QOpenGLWidget
{
    Q_OBJECT

  public:
    enum RendererType {
      DebugRenderer,
      TextureRenderer
    };

  private:
    static constexpr int RENDERER_COUNT = int(TextureRenderer) + 1;

  public:
    explicit VolumeViewer(QWidget *parent = 0)
      : QOpenGLWidget(parent)
      , m_renderer_type(TextureRenderer)
      , m_renderer_changed(false)
      , m_renderer(nullptr)
      , m_scale(0.15f) //1.0f)
      , m_track_ball()
      , m_logger()
    { }

    void setRenderer(RendererType type);

  public slots:
    void toggleRenderer(void);

  protected:
    virtual void initializeGL(void) override;
    virtual void paintGL(void) override;
    virtual void resizeGL(int w, int h) override;

    virtual void mousePressEvent(QMouseEvent *event) override;
    //virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;

  private:
    RendererType m_renderer_type;
    bool m_renderer_changed;
    std::unique_ptr<VolumeRenderer> m_renderer;
    float m_scale;
    TrackBall m_track_ball;
    Logger m_logger;
};

#endif // VOLUME_VIEWER_H
