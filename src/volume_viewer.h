#ifndef VOLUME_VIEWER_H
#define VOLUME_VIEWER_H

#include "logger.h"
#include "trackball.h"
#include "volume_renderer.h"
#include "volume_data.h"

#include <QOpenGLWidget>
#include <memory>


class VolumeRenderer;
class TransferFunction;

class VolumeViewer : public QOpenGLWidget
{
    Q_OBJECT

  public:
    enum RendererType {
      DebugRenderer,
      TextureRenderer,
      RayCastRenderer
    };

  private:
    static constexpr int RENDERER_COUNT = int(RayCastRenderer) + 1;

  public:
    explicit VolumeViewer(QWidget *parent = 0)
      : QOpenGLWidget(parent)
      , m_renderer_changed(false)
      , m_renderer_type(TextureRenderer)
      //, m_renderer_type(RayCastRenderer)
      , m_renderer(nullptr)
      , m_volume_data_changed(false)
      , m_volume_data(nullptr)
      , m_transfer_func_changed(false)
      , m_transfer_func(nullptr)
      , m_track_ball()
      , m_scale(0.15f)
      , m_peel_depth(0.0f)
      , m_transl(QVector3D(0.0f, 0.0f, 0.0f))
      , m_shift_pressed(false)
      , m_high_quality(true)
      , m_logger()
    {
      // aby fungovala keyPressEvent
      setFocusPolicy(Qt::ClickFocus);
    }

    ~VolumeViewer(void);

  signals:
    void error(const QString & msg);

  public slots:
    void toggleRenderer(void);
    void setRenderer(RendererType type);
    void setVolumeData(const VolumeData *volume_data);
    void setTransferFunction(const TransferFunction *transfer_func);
    void setDetail(int level);

  protected:
    virtual void initializeGL(void) override;
    virtual void paintGL(void) override;
    virtual void resizeGL(int w, int h) override;

    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;

  private:
    // renderer
    bool m_renderer_changed;
    RendererType m_renderer_type;
    std::unique_ptr<VolumeRenderer> m_renderer;

    // volume data
    bool m_volume_data_changed;
    const VolumeData *m_volume_data;

    // transfer function
    bool m_transfer_func_changed;
    const TransferFunction *m_transfer_func;

    // scene manipulation
    TrackBall m_track_ball;
    float m_scale;
    float m_peel_depth;
    QVector3D m_transl;
    bool m_shift_pressed;
    bool m_high_quality;    // whether the user manipulates with the scene (then render in high quality) or not (render in low quality)

    // OpenGL debug logging
    Logger m_logger;
};

#endif // VOLUME_VIEWER_H
