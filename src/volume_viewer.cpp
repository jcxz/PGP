#include "volume_viewer.h"
#include "ogl.h"
#include "debug_volume_renderer.h"
#include "texture_volume_renderer.h"

#include <QWheelEvent>
#include <QKeyEvent>
#include <QMatrix4x4>



bool VolumeViewer::openRawFile(const QString & filename, int width, int height, int depth)
{
  if (!m_volume_data.loadFromRaw(filename, width, height, depth))
  {
    qWarning() << "Failed to open file" << filename;
    return false;
  }

  m_renderer->setVolumeData(&m_volume_data);

  return true;
}


bool VolumeViewer::openFile(const QString & filename)
{
  if (!m_volume_data.load(filename))
  {
    qWarning() << "Failed to open file" << filename;
    return false;
  }

  m_renderer->setVolumeData(&m_volume_data);

  return true;
}


void VolumeViewer::setRenderer(RendererType type)
{
  switch (type)
  {
    case DebugRenderer:
      m_renderer.reset(new DebugVolumeRenderer);
      m_renderer_type = DebugRenderer;
      m_renderer_changed = true;
      break;

    case TextureRenderer:
      m_renderer.reset(new TextureVolumeRenderer);
      m_renderer_type = TextureRenderer;
      m_renderer_changed = true;
      break;
  }
}


void VolumeViewer::toggleRenderer(void)
{
  qDebug() << __PRETTY_FUNCTION__;
  setRenderer(RendererType((int(m_renderer_type) + 1) % RENDERER_COUNT));
  update();
}


void VolumeViewer::setTransferFunction(const TransferFunction *transfer_func)
{
  qDebug() << __PRETTY_FUNCTION__;
  m_transfer_func = transfer_func;
  m_transfer_func_changed = true;
  update();
}


void VolumeViewer::initializeGL(void)
{
  qDebug() << __PRETTY_FUNCTION__;

  // inicializacia OpenGL Funkcii
  //OGLF->initializeOpenGLFunctions();

  // inicializacia logovania sprav z debug kontextu
  m_logger.init();

  // nastavenie rendereru
  setRenderer(m_renderer_type);
}


void VolumeViewer::paintGL(void)
{
  //qDebug() << __PRETTY_FUNCTION__;

  // vypocet transformacnej matice
  /*
  QMatrix4x4 mvp;

  mvp.perspective(30.0f, float(width()) / float(height()), 0.01f, 1000.0f);
  mvp.translate(0.0f, 0.0f, -1.0f);
  mvp.rotate(m_track_ball.getRotation());
  mvp.scale(m_scale);

  qDebug() << "width=" << width() << ", height=" << height() << ", " << (((float) width()) / ((float) height()));
  qDebug() << "mvp=" << mvp;

  // renderovanie
  m_renderer->render(mvp);
  */

  // nacitanie volumetrickych dat (treba nacitavat az ked mam aktivny OpenGL kontext)
  if (!m_volume_filename.isNull())
  {
    if (!m_volume_data.loadFromRaw(m_volume_filename, 256, 256, 109))
    {
      qWarning() << "Failed to load volume data";
    }

    m_volume_filename = QString();
  }

  // inicializacia rendereru (renderer treba inicializovat az vtedy ked uz
  // existuje platny OpenGL kontext.
  // A navyse medzi renderermi sa da aj prepinat ...)
  if (m_renderer_changed)
  {
    if (!m_renderer->reset())
    {
      qWarning() << "Failed to initialize renderer";
    }

    m_renderer->setPerspectiveProjection(width(), height());
    m_renderer->setVolumeData(&m_volume_data);

    m_renderer_changed = false;
  }

  if (m_transfer_func_changed)
  {
    m_renderer->uploadTransferFunction(*m_transfer_func);
    m_transfer_func_changed = false;
  }

  m_renderer->render(m_track_ball.getRotation(),
                     QVector3D(m_scale, m_scale, m_scale),
                     QVector3D(0.0f, 0.0f, 0.0f),
                     m_peel_depth);
}


void VolumeViewer::resizeGL(int w, int h)
{
  qDebug() << __PRETTY_FUNCTION__;
  glViewport(0, 0, w, h);
  m_renderer->setPerspectiveProjection(w, h);
}


void VolumeViewer::keyPressEvent(QKeyEvent *event)
{
  qDebug() << __PRETTY_FUNCTION__;

  if (event->key() == Qt::Key_Shift)
    m_shift_pressed = true;
  else
    m_shift_pressed = false;

  return QOpenGLWidget::keyPressEvent(event);
}


void VolumeViewer::keyReleaseEvent(QKeyEvent *event)
{
  qDebug() << __PRETTY_FUNCTION__;
  m_shift_pressed = false;
  return QOpenGLWidget::keyReleaseEvent(event);
}


void VolumeViewer::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
  {
    m_track_ball.push(event->pos(), width(), height());
    event->accept();
    update();
  }

  return QOpenGLWidget::mousePressEvent(event);
}


void VolumeViewer::mouseMoveEvent(QMouseEvent *event)
{
  if (event->buttons() & Qt::LeftButton)
  {
    m_track_ball.move(event->pos(), width(), height());
    event->accept();
    update();
  }

  return QOpenGLWidget::mouseMoveEvent(event);
}


void VolumeViewer::wheelEvent(QWheelEvent *event)
{
  float delta = float(event->delta()) * 0.0001f;

  if (m_shift_pressed)
  {
    m_peel_depth += delta;

    if (m_peel_depth >= 2.0f) m_peel_depth = 2.0f;
    else if (m_peel_depth <= 0.0f) m_peel_depth = 0.0f;
  }
  else
  {
    m_scale += delta;

    if (m_scale >= 2.0f) m_scale = 2.0f;
    else if (m_scale <= 0.000001f) m_scale = 0.000001f;
  }

  qDebug() << "scale=" << m_scale << ", peel_depth=" << m_peel_depth;

  update();

  return QOpenGLWidget::wheelEvent(event);
}
