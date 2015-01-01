#include "ray_cast_volume_renderer.h"

#include "ogl.h"

#include <QMatrix4x4>


namespace {

const GLfloat g_cube_vertices[][3] = {
  { 0.0f, 0.0f, 0.0f },  // 0
  { 0.0f, 0.0f, 1.0f },  // 1
  { 0.0f, 1.0f, 0.0f },  // 2
  { 0.0f, 1.0f, 1.0f },  // 3
  { 1.0f, 0.0f, 0.0f },  // 4
  { 1.0f, 0.0f, 1.0f },  // 5
  { 1.0f, 1.0f, 0.0f },  // 6
  { 1.0f, 1.0f, 1.0f }   // 7
};

const int g_cube_vertices_cnt = sizeof(g_cube_vertices) / sizeof(g_cube_vertices[0]);

const GLuint g_cube_indices[] = {
  // predna stena
  1, 5, 7,
  1, 7, 3,

  // zadna stena
  2, 4, 0,
  2, 6, 4,

  // prava stena
  5, 4, 6,
  5, 6, 7,

  // lava stena
  0, 1, 3,
  0, 3, 2,

  // horna stena
  3, 7, 6,
  3, 6, 2,

  // dolna stena
  0, 4, 5,
  0, 5, 1
};

const int g_cube_indices_cnt = sizeof(g_cube_indices) / sizeof(g_cube_indices[0]);

} // End of private namespace




bool RayCastVolumeRenderer::reset_impl(void)
{
  //*** zistenie defaultneho frame bufferu
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint *) &m_default_fbo);

  qDebug() << "default frame buffer object id: " << m_default_fbo;

  // kompilacia shaderov
  m_prog_gen_back_faces.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/src/opengl/ray_cast_volume_renderer.vert");
  m_prog_gen_back_faces.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/src/opengl/ray_cast_volume_renderer_back_faces.frag");
  m_prog_gen_back_faces.link();

  //*** Kompilacia shader programu, ktory zobrazuje frame bufffer na obrazovku
  m_prog_ray_cast.addShaderFromSourceFile(QOpenGLShader::Vertex,   ":/src/opengl/ray_cast_volume_renderer.vert");
  m_prog_ray_cast.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/src/opengl/ray_cast_volume_renderer.frag");
  m_prog_ray_cast.link();

  //*** Vytvorenie OpenGL bufferov
  m_vao.create();
  m_vao.bind();

  m_vbo_cube.create();
  m_ibo_cube.create();

  m_ibo_cube.bind();
  m_ibo_cube.allocate(g_cube_indices, sizeof(g_cube_indices));

  m_vbo_cube.bind();
  m_vbo_cube.allocate(g_cube_vertices, sizeof(g_cube_vertices));

  // nastavenie arributov
  m_prog_gen_back_faces.bind();
  int attr_pos = m_prog_gen_back_faces.attributeLocation("pos");

  OGLF->glEnableVertexAttribArray(attr_pos);
  OGLF->glVertexAttribPointer(attr_pos, 3, GL_FLOAT, GL_FALSE, sizeof(g_cube_vertices[0]), (void *) 0);

  // odbindovanie VAO a buffer objectu
  OGLF->glBindVertexArray(0);
  OGLF->glBindBuffer(GL_ARRAY_BUFFER, 0);
  OGLF->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  OGLF->glUseProgram(0);

  //*** Inicializacia frame bufferu
  return initFramebuffer(m_width, m_height);
}


RayCastVolumeRenderer::~RayCastVolumeRenderer(void)
{
  OGLF->glDeleteTextures(1, &m_color_attach);
  // pretoze Qt pouziva fbo cislo 1
  if ((m_fbo != 0) && (m_fbo != m_default_fbo))
  {
    OGLF->glDeleteFramebuffers(1, &m_fbo);
  }
}


bool RayCastVolumeRenderer::resize(QRect rect)
{
  VolumeRenderer::resize(rect);
  return initFramebuffer(rect.width(), rect.height());
}


bool RayCastVolumeRenderer::initFramebuffer(int w, int h)
{
  // alokacia textury
  if (m_color_attach == 0) OGLF->glGenTextures(1, &m_color_attach);

  OGLF->glBindTexture(GL_TEXTURE_2D, m_color_attach);

  OGLF->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  OGLF->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  OGLF->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  OGLF->glBindTexture(GL_TEXTURE_2D, 0);

  // Nastavenie framebufferu
  if (m_fbo == 0) OGLF->glGenFramebuffers(1, &m_fbo);

  OGLF->glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

  OGLF->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_attach, 0);

  // Nastavenie zoznamu attachementov, do ktorych sa bude kreslit
  GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
  OGLF->glDrawBuffers(1, draw_buffers);

  if (OGLF->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    qWarning() << "**initFramebuffer: Failed to create frame buffer: frame buffer is not complete";
    return false;
  }

  OGLF->glBindFramebuffer(GL_FRAMEBUFFER, m_default_fbo);

  return true;
}


void RayCastVolumeRenderer::render_impl(const QQuaternion & rotation,
                                        const QVector3D & scale,
                                        const QVector3D & translation,
                                        float peel_depth,
                                        int slice_count)
{
  // Transformacna model-view matica matica
  QMatrix4x4 mv;

  mv.translate(0.0f, 0.0f, -0.5f);
  mv.translate(translation);
  mv.rotate(rotation);
  mv.scale(scale);

  mv.scale((m_data.physicalWidth()  / m_data.maxPhysicalSize()),
           (m_data.physicalHeight() / m_data.maxPhysicalSize()),
           (m_data.physicalDepth()  / m_data.maxPhysicalSize()));

  mv.translate(-0.5f, -0.5f, -0.5f);

  // nabindovanie textur, geometrie a povolenie cullingu
  OGLF->glActiveTexture(GL_TEXTURE0);
  OGLF->glBindTexture(GL_TEXTURE_1D, m_transfer_func.textureId());
  OGLF->glActiveTexture(GL_TEXTURE1);
  OGLF->glBindTexture(GL_TEXTURE_2D, m_color_attach);
  OGLF->glActiveTexture(GL_TEXTURE2);
  OGLF->glBindTexture(GL_TEXTURE_3D, m_data.oglID());

  m_vao.bind();

  OGLF->glEnable(GL_DEPTH_TEST);
  OGLF->glEnable(GL_CULL_FACE);
  OGLF->glDisable(GL_BLEND);

  // Kreslenie sceny do framebufferu
  OGLF->glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
  OGLF->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  OGLF->glCullFace(GL_FRONT);

  m_prog_gen_back_faces.bind();
  m_prog_gen_back_faces.setUniformValue("mvp", m_proj * mv);

  OGLF->glDrawElements(GL_TRIANGLES, g_cube_indices_cnt, GL_UNSIGNED_INT, nullptr);

  // Ray-casting
  OGLF->glBindFramebuffer(GL_FRAMEBUFFER, m_default_fbo);
  OGLF->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  OGLF->glCullFace(GL_BACK);

  m_prog_ray_cast.bind();
  m_prog_ray_cast.setUniformValue("mvp", m_proj * mv);
  m_prog_ray_cast.setUniformValue("step", 0.005f);
  m_prog_ray_cast.setUniformValue("tex_transfer_func", 0);
  m_prog_ray_cast.setUniformValue("tex_back_faces", 1);
  m_prog_ray_cast.setUniformValue("tex_volume_data", 2);

  OGLF->glDrawElements(GL_TRIANGLES, g_cube_indices_cnt, GL_UNSIGNED_INT, nullptr);

  // Odbindovanie programu, geometrie, textur a zakazanie cullingu
  OGLF->glUseProgram(0);

  OGLF->glEnable(GL_BLEND);
  OGLF->glDisable(GL_CULL_FACE);
  OGLF->glDisable(GL_DEPTH_TEST);

  OGLF->glBindVertexArray(0);

  OGLF->glActiveTexture(GL_TEXTURE2);
  OGLF->glBindTexture(GL_TEXTURE_3D, 0);
  OGLF->glActiveTexture(GL_TEXTURE1);
  OGLF->glBindTexture(GL_TEXTURE_2D, 0);
  OGLF->glActiveTexture(GL_TEXTURE0);
  OGLF->glBindTexture(GL_TEXTURE_1D, 0);
}
