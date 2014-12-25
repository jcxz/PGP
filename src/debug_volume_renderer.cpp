#include "debug_volume_renderer.h"
#include "ogl.h"


namespace {

struct Vertex
{
  GLfloat pos[3];
  GLfloat col[3];
};

const Vertex g_cube_vertices[] = {
  { { -1.0f, -1.0f, -1.0f }, { 1.0f, 0.0f, 0.0f } },  // 0
  { { -1.0f, -1.0f,  1.0f }, { 0.0f, 1.0f, 0.0f } },  // 1
  { { -1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f, 1.0f } },  // 2
  { { -1.0f,  1.0f,  1.0f }, { 1.0f, 1.0f, 0.0f } },  // 3
  { {  1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f, 1.0f } },  // 4
  { {  1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f, 1.0f } },  // 5
  { {  1.0f,  1.0f, -1.0f }, { 1.0f, 1.0f, 1.0f } },  // 6
  { {  1.0f,  1.0f,  1.0f }, { 0.5f, 0.5f, 0.5f } }   // 7
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



bool DebugVolumeRenderer::reset(void)
{
  // kompilacia shaderov
  m_prog.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/src/opengl/debug_volume_renderer.vert");
  m_prog.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/src/opengl/debug_volume_renderer.frag");
  if (!m_prog.link())
  {
    qWarning() << "Failed to link shader programs";
    return false;
  }

  // Vytvorenie a nabindovanie vertex array object (asi vola glGenVertexArrays)
  m_vao.create();
  m_vao.bind();

  // Vytvorenie bufferov v OpenGL (asi vola glGenBuffers)
  m_vbo_cube.create();
  m_ibo_cube.create();

  // alokacia a nahratie dat na GPU
  m_ibo_cube.bind();
  m_ibo_cube.allocate(g_cube_indices, sizeof(g_cube_indices));

  m_vbo_cube.bind();
  m_vbo_cube.allocate(g_cube_vertices, sizeof(g_cube_vertices));

  // nastavenie arributov
  int loc_pos_vert = m_prog.attributeLocation("pos_vert");
  int loc_col_vert = m_prog.attributeLocation("col_vert");

  qDebug() << "loc_pos_vert=" << loc_pos_vert;
  qDebug() << "loc_col_vert=" << loc_col_vert;

  OGLF->glEnableVertexAttribArray(loc_pos_vert);
  OGLF->glVertexAttribPointer(loc_pos_vert, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);

  OGLF->glEnableVertexAttribArray(loc_col_vert);
  OGLF->glVertexAttribPointer(loc_col_vert, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) sizeof(float[3]));

  // odbindovanie VAO a buffer objectu
  OGLF->glBindVertexArray(0);
  OGLF->glBindBuffer(GL_ARRAY_BUFFER, 0);
  OGLF->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // nastavenie ostatnych atributov
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  return true;
}


void DebugVolumeRenderer::render_impl(const QQuaternion & rotation,
                                      const QVector3D & scale,
                                      const QVector3D & translation,
                                      const float /* peel_depth */)
{
  QMatrix4x4 mv;

  mv.translate(translation);
  mv.translate(0.0f, 0.0f, -1.0f);
  mv.rotate(rotation);
  mv.scale(scale);

  qDebug() << "mv=" << mv;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  OGLF->glUseProgram(m_prog.programId());

  //m_prog.setUniformValue("mvp", mvp);
  m_prog.setUniformValue("mvp", m_proj * mv);

  m_vao.bind();
  glDrawElements(GL_TRIANGLES, g_cube_indices_cnt, GL_UNSIGNED_INT, nullptr);
  OGLF->glBindVertexArray(0);

  OGLF->glUseProgram(0);
}
