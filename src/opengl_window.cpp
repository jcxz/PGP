#include "opengl_window.h"
#include "ogl.h"

#include <QtCore/QCoreApplication>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPainter>



OpenGLWindow::OpenGLWindow(QWindow *parent)
  : QWindow(parent)
  , m_update_pending(false)
  , m_animating(false)
  , m_context(0)
  , m_device(0)
{
  setSurfaceType(QWindow::OpenGLSurface);
}


OpenGLWindow::~OpenGLWindow(void)
{
  delete m_device;
}


void OpenGLWindow::render(QPainter *painter)
{
  Q_UNUSED(painter);
}


void OpenGLWindow::initialize(void)
{
}


void OpenGLWindow::render(void)
{
  if (!m_device)
    m_device = new QOpenGLPaintDevice;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  m_device->setSize(size());

  QPainter painter(m_device);
  render(&painter);
}


void OpenGLWindow::renderLater(void)
{
  if (!m_update_pending)
  {
    m_update_pending = true;
    QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
  }
}


bool OpenGLWindow::event(QEvent *event)
{
  switch (event->type())
  {
    case QEvent::UpdateRequest:
      m_update_pending = false;
      renderNow();
      return true;
    default:
      return QWindow::event(event);
  }
}


void OpenGLWindow::exposeEvent(QExposeEvent *event)
{
  Q_UNUSED(event);

  if (isExposed())
    renderNow();
}


void OpenGLWindow::renderNow(void)
{
  if (!isExposed())
    return;

  bool needsInitialize = false;

  if (!m_context)
  {
    m_context = new QOpenGLContext(this);
    m_context->setFormat(requestedFormat());
    m_context->create();

    needsInitialize = true;
  }

  m_context->makeCurrent(this);

  if (needsInitialize)
  {
    initializeOpenGLFunctions();
    OGLF->initializeOpenGLFunctions();
    //ogl::initOGLFunctions(m_context);
    initialize();
  }

  render();

  m_context->swapBuffers(this);

  if (m_animating)
    renderLater();
}


void OpenGLWindow::setAnimating(bool animating)
{
  m_animating = animating;

  if (animating)
    renderLater();
}
