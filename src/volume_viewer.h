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
      , m_scale(0.15f)
      , m_track_ball()
      , m_peel_depth(0.0f)
      , m_shift_pressed(false)
      , m_volume_data()
      , m_transfer_func(nullptr)
      , m_transfer_func_changed(false)
      , m_logger()
    {
      // aby fungovala keyPressEvent
      setFocusPolicy(Qt::ClickFocus);
    }

    ~VolumeViewer(void);

    bool openRawFile(const QString & filename, int width, int height, int depth, int bit_depth);
    bool openFile(const QString & filename);

    void setRenderer(RendererType type);

  public slots:
    void toggleRenderer(void);
    bool setTransferFunction(const TransferFunction & transfer_func);

  protected:
    virtual void initializeGL(void) override;
    virtual void paintGL(void) override;
    virtual void resizeGL(int w, int h) override;

    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;

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
    float m_peel_depth;
    bool m_shift_pressed;

    VolumeData m_volume_data;

    const TransferFunction *m_transfer_func;
    bool m_transfer_func_changed;

    Logger m_logger;
};

#endif // VOLUME_VIEWER_H
