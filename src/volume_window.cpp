#include "volume_window.h"
#include "ogl.h"

#include <QtGui/QScreen>
#include <QtGui/QMouseEvent>

// Tato konstanta predstavuje world-space velkost mojich volumetrickych dat,
// resp. work-size velkost bounding box-u okolo tychto mojich volumetrickych dat
#define PROXY_GEOM_SIZE 256.0f //100.0f
#define NUM_PROXY_QUADS 300 //256    //109 //218 //109 //256 //109 //100
#define OGL_DEBUG

struct Point2D
{
  GLfloat x, y;
};

struct Vertex
{
  Point2D pos;
  Point2D tex;
};


VolumeWindow::VolumeWindow(QWindow *parent)
  : OpenGLWindow(parent)
  , m_vao(0)
  , m_attr_pos(0)
  , m_attr_tex_coords(0)
  , m_program(this)
  , m_tex_vol_data()
  , m_logger(this)
  , m_mouse_start()
  , m_angle_x(0.0f)
  , m_angle_y(0.0f)
  , m_dist_z(-10.0f)
  , m_depth_data(0.0f)
{
  QSurfaceFormat format;

  // asks for a OpenGL 4.3 debug context using the Core profile
  format.setMajorVersion(3);
  format.setMinorVersion(3);
  format.setProfile(QSurfaceFormat::CoreProfile);
#ifdef OGL_DEBUG
  format.setOption(QSurfaceFormat::DebugContext);
#endif
  format.setSamples(16);

  setFormat(format);
  resize(640, 480);
}


VolumeWindow::~VolumeWindow(void)
{
  OGLF->glDeleteVertexArrays(1, &m_vao);
}


void VolumeWindow::initialize(void)
{
  // kompilacia shaderov
  m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/src/opengl/volume_renderer.vert");
  m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/src/opengl/volume_renderer.frag");
  m_program.link();

  // atributy a uniformne premenne
  m_attr_pos = m_program.attributeLocation("pos");
  m_attr_tex_coords = m_program.attributeLocation("tex_coords_in");

  m_program.bind();
  m_program.setUniformValue("num_instances", (GLfloat) NUM_PROXY_QUADS);
  glUseProgram(0);

  // vytvorenie a nabindovanie vertex array object (v core profile je vyzadovany)
  OGLF->glGenVertexArrays(1, &m_vao);
  OGLF->glBindVertexArray(m_vao);

  // Vytvorenie bufferu pre screen space quad
  Vertex quad[] = {
    { { -1.0f,  1.0f }, { 0.0f, 1.0f } },
    { { -1.0f, -1.0f }, { 0.0f, 0.0f } },
    { {  1.0f,  1.0f }, { 1.0f, 1.0f } },
    { {  1.0f, -1.0f }, { 1.0f, 0.0f } }
  };

  OGLF->glGenBuffers(1, &m_vbo);
  OGLF->glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  OGLF->glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

  // nastavenie atributov
  OGLF->glEnableVertexAttribArray(m_attr_pos);
  //OGLF->glEnableVertexAttribArray(0);
  OGLF->glVertexAttribPointer(m_attr_pos, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);

  OGLF->glEnableVertexAttribArray(m_attr_tex_coords);
  OGLF->glVertexAttribPointer(m_attr_tex_coords, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) sizeof(Point2D));

  // Odbindovanie vertex array
  OGLF->glBindVertexArray(0);

  // Nacitanie volumetrickych data do 3D textury
  if (!m_tex_vol_data.loadFromRaw(":/data/head256x256x109_8bit_chan.raw", 256, 256, 109))
  {
    qWarning("failed to load default volumetric data");
  }

  // debugovanie
#ifdef OGL_DEBUG
  if (!m_logger.initialize())
  {
    qWarning("Failed to initialize debug logger");
    return;
  }

  connect(&m_logger, SIGNAL(messageLogged(const QOpenGLDebugMessage & )),
          this, SLOT(handleLoggedMessage(const QOpenGLDebugMessage & )),
          Qt::DirectConnection);

  m_logger.startLogging(QOpenGLDebugLogger::SynchronousLogging);
  m_logger.enableMessages();
#endif
}


void VolumeWindow::render(void)
{
  const qreal retinaScale = devicePixelRatio();
  glViewport(0, 0, width() * retinaScale, height() * retinaScale);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // vypnutie depth testu a zapnutie blendovania
  //glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // aktivovanie shader programu
  m_program.bind();

  // nastavenie rotacie
  QMatrix4x4 tex_matrix;

  // presunutie sa do stredu volumetrickych dat
  tex_matrix.translate(0.5f, 0.5f, 0.5f);

  // normalizacia proxy geometrie podla velkosti volumetrickych dat
  // Toto je potrebne, pretoze moje volumetricke data nemusia mat
  // kazdej dimenzii rovnaky pocet voxelov
  tex_matrix.scale(PROXY_GEOM_SIZE / float(m_tex_vol_data.width()),
                   PROXY_GEOM_SIZE / float(m_tex_vol_data.height()),
                   PROXY_GEOM_SIZE / float(m_tex_vol_data.depth()));

  // rotacia s datami
  tex_matrix.rotate(m_angle_x, 1, 0, 0);
  tex_matrix.rotate(m_angle_y, 0, 1, 0);

  // presunutie sa naspat do povodnej polohy
  tex_matrix.translate(-0.5f, -0.5f, -0.5f);

  tex_matrix.translate(0.0f, 0.0f, m_depth_data);

  QMatrix4x4 mvp_matrix;
  mvp_matrix.perspective(30.0f, (float) width() / (float) height(), 0.1f, 100.0f);
  mvp_matrix.translate(0.0f, 0.0f, m_dist_z);

  m_program.setUniformValue("mvp_matrix", mvp_matrix);
  m_program.setUniformValue("tex_matrix", tex_matrix);

  // nastavenie textur
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, m_tex_vol_data.id());

  m_program.setUniformValue("tex_data", 0);

  // vykreslenie proxy geometrie
  OGLF->glBindVertexArray(m_vao);
  OGLF->glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, NUM_PROXY_QUADS);
  OGLF->glBindVertexArray(0);

  // deaktivovanie shader programu
  glUseProgram(0);

  // vratenie blendovania a depth testov do povodneho stavu
  //glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
}


void VolumeWindow::handleLoggedMessage(const QOpenGLDebugMessage & msg)
{
  qDebug() << msg;
}


void VolumeWindow::mousePressEvent(QMouseEvent *event)
{
  m_mouse_start = event->pos();
  renderLater();
  return OpenGLWindow::mousePressEvent(event);
}


void VolumeWindow::mouseMoveEvent(QMouseEvent *event)
{
  if (event->buttons() == Qt::LeftButton)
  {
    m_angle_y += (event->pos().x() - m_mouse_start.x()) * 0.05f;
    m_angle_x += (event->pos().y() - m_mouse_start.y()) * 0.05f;
    renderLater();
  }
  else if (event->buttons() == Qt::RightButton)
  {
    m_dist_z += (event->pos().y() - m_mouse_start.y()) * -0.01f;
    if (m_dist_z > 0.0f) m_dist_z = 0.0f;
    else if (m_dist_z < -20.0f) m_dist_z = -20.0f;
    renderLater();
  }
  else if (event->buttons() == Qt::MidButton)
  {
    if ((event->pos().y() - m_mouse_start.y()) > 0.0f)
    {
      m_depth_data += 0.01f;
      if (m_depth_data > 0.0f) m_depth_data = 0.0f;
    }
    else
    {
      m_depth_data -= 0.01f;
      if (m_depth_data < -1.0f) m_depth_data = -1.0f;
    }
    renderLater();
  }

  qDebug() << __PRETTY_FUNCTION__
           << "m_angle_x=" << m_angle_x
           << ", m_angle_y=" << m_angle_y
           << ", m_dist_z=" << m_dist_z
           << ", m_depth_data=" << m_depth_data;

  return OpenGLWindow::mouseMoveEvent(event);
}
