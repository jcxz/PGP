#include "volume_viewer.h"
#include "ogl.h"
#include "debug_volume_renderer.h"
#include "texture_volume_renderer.h"
#include "ray_cast_volume_renderer.h"

#include <QWheelEvent>
#include <QKeyEvent>
#include <QMatrix4x4>



VolumeViewer::~VolumeViewer(void)
{
  makeCurrent();  // aby ked budem deletovat renderer bol aktivny nejaky kontext
}


void VolumeViewer::toggleRenderer(void)
{
  setRenderer(RendererType((int(m_renderer_type) + 1) % RENDERER_COUNT));
}


void VolumeViewer::setRenderer(RendererType type)
{
  makeCurrent();

  switch (type)
  {
    case DebugRenderer:
      qDebug() << "Using debug renderer";
      m_renderer.reset(new DebugVolumeRenderer);
      m_renderer_type = DebugRenderer;
      m_renderer_changed = true;
      update();
      break;

    case TextureRenderer:
      qDebug() << "Using texture renderer";
      m_renderer.reset(new TextureVolumeRenderer);
      m_renderer_type = TextureRenderer;
      m_renderer_changed = true;
      update();
      break;

    case RayCastRenderer:
      qDebug() << "Using raycasting renderer";
      m_renderer.reset(new RayCastVolumeRenderer);
      m_renderer_type = RayCastRenderer;
      m_renderer_changed = true;
      update();
      break;
  }
}


void VolumeViewer::setVolumeData(const VolumeData *volume_data)
{
  m_volume_data = volume_data;
  m_volume_data_changed = true;
  update();
}


void VolumeViewer::setTransferFunction(const TransferFunction *transfer_func)
{
  m_transfer_func = transfer_func;
  m_transfer_func_changed = true;
  update();
}


void VolumeViewer::setDetail(int count)
{
  m_renderer->setSliceCount(count);
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
  // inicializacia rendereru (renderer treba inicializovat az vtedy ked uz
  // existuje platny OpenGL kontext.
  // A navyse medzi renderermi sa da aj prepinat ...)
  if (m_renderer_changed)
  {
    if (!m_renderer->reset(width(), height()))
    {
      emit error(tr("Failed to initialize renderer"));
      return;
    }

    m_renderer->setPerspectiveProjection(width(), height());

    m_renderer_changed = false;
    m_volume_data_changed = true;
    m_transfer_func_changed = true;
  }

  // nastavenie volumetrickych dat
  if (m_volume_data_changed)
  {
    if ((m_volume_data != nullptr) && (!m_renderer->setVolumeData(*m_volume_data)))
    {
      emit error(tr("Failed to upload volume data to OpenGL"));
    }

    m_volume_data_changed = false;
  }

  // nastavenie transfer funkcie
  if (m_transfer_func_changed)
  {
    if ((m_transfer_func != nullptr) && (!m_renderer->uploadTransferFunction(*m_transfer_func)))
    {
      emit error(tr("Failed to upload transfer function to OpenGL"));
    }

    m_transfer_func_changed = false;
  }

  // renderovanie
  if (m_high_quality)
  {
    m_renderer->render(m_track_ball.getRotation(),
                       QVector3D(m_scale, m_scale, m_scale),
                       //QVector3D(0.0f, 0.0f, 0.0f),
                       m_transl,
                       m_peel_depth);
  }
  else
  {
    m_renderer->renderPreview(m_track_ball.getRotation(),
                              QVector3D(m_scale, m_scale, m_scale),
                              //QVector3D(0.0f, 0.0f, 0.0f),
                              m_transl,
                              m_peel_depth);
    m_high_quality = true;
  }
}


void VolumeViewer::resizeGL(int w, int h)
{
  qDebug() << __PRETTY_FUNCTION__;
  m_renderer->resize(rect());
  //m_renderer->resize(w, h);
  //glViewport(0, 0, w, h);
  //m_renderer->setPerspectiveProjection(w, h);
  m_high_quality = false;
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
    m_high_quality = false;
    event->accept();
    update();
  }

  return QOpenGLWidget::mousePressEvent(event);
}


void VolumeViewer::mouseReleaseEvent(QMouseEvent *event)
{
  m_high_quality = true;
  update();
  return QOpenGLWidget::mouseReleaseEvent(event);
}


void VolumeViewer::mouseMoveEvent(QMouseEvent *event)
{
  if (event->buttons() & Qt::LeftButton)
  {
    m_track_ball.move(event->pos(), width(), height());
    m_high_quality = false;
    event->accept();
    update();
  }
  else if (event->buttons() & Qt::MiddleButton)
  {
    //m_transl.setX(event->pos().x());
    //m_transl.setY(event->pos().y());
    qDebug() << "m_transl=" << m_transl;
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

    if (m_peel_depth >= 1.0f) m_peel_depth = 1.0f;
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
