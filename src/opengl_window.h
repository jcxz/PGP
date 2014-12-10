#ifndef OPENGL_WINDOW_H
#define OPENGL_WINDOW_H

#include <QtGui/QWindow>
#include <QtGui/QOpenGLFunctions>

class QPainter;
class QOpenGLContext;
class QOpenGLPaintDevice;

class OpenGLWindow : public QWindow, protected QOpenGLFunctions
{
    Q_OBJECT

  public:
    explicit OpenGLWindow(QWindow *parent = 0);
    virtual ~OpenGLWindow(void);

    virtual void render(QPainter *painter);
    virtual void render(void);
    virtual void initialize(void);

    void setAnimating(bool animating);

  public slots:
    void renderLater(void);
    void renderNow(void);

  protected:
    virtual bool event(QEvent *event) override;
    virtual void exposeEvent(QExposeEvent *event) override;

  private:
    bool m_update_pending;
    bool m_animating;

    QOpenGLContext *m_context;
    QOpenGLPaintDevice *m_device;
};

#endif // OPENGL_WINDOW_H
