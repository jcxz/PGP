#include "texture_volume_renderer.h"
#include "ogl.h"

#define NUM_PROXY_QUADS 300 //256 //109 //218 //100



struct Point2D
{
  GLfloat x, y;
};

struct Vertex
{
  Point2D pos;
  Point2D tex;
};



bool TextureVolumeRenderer::reset(void)
{
  // shader pre bounding box
  m_prog_bbox.addShaderFromSourceFile(QOpenGLShader::Vertex,   ":/src/opengl/wire_box.vert");
  m_prog_bbox.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/src/opengl/wire_box.frag");
  m_prog_bbox.link();

  m_prog_bbox.bind();
  m_prog_bbox.setUniformValue("col", QVector3D(1.0f, 0.0f, 0.0f));
  m_prog_bbox.setUniformValue("dimensions", QVector3D(1.0f, 1.0f, 1.0f));

  // shader na kreslenie stvorca (len pre debugovanie)
  m_prog_rectangle.addShaderFromSourceFile(QOpenGLShader::Vertex,   ":/src/opengl/rectangle.vert");
  m_prog_rectangle.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/src/opengl/rectangle.frag");
  m_prog_rectangle.link();

  m_prog_rectangle.bind();
  m_prog_rectangle.setUniformValue("col", QVector4D(0.0f, 0.0f, 0.9f, 0.5f));

  // shader pre kreslenie volumetrickych dat
  m_program.addShaderFromSourceFile(QOpenGLShader::Vertex,   ":/src/opengl/texture_volume_renderer.vert");
  m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/src/opengl/texture_volume_renderer.frag");
  m_program.link();

  m_program.bind();
  m_program.setUniformValue("num_instances", (GLfloat) NUM_PROXY_QUADS);
  m_program.setUniformValue("num_instances_inv", 1.0f / ((GLfloat) NUM_PROXY_QUADS));

  // Odbindovanie shader programov
  OGLF->glUseProgram(0);

  // vytvorenie a nabindovanie vertex array object (v core profile je vyzadovany)
  m_vao.create();
  m_vao.bind();

  // Vytvorenie bufferu pre screen space quad
  Vertex quad[] = {
    { { -1.0f,  1.0f }, { 0.0f, 1.0f } },
    { { -1.0f, -1.0f }, { 0.0f, 0.0f } },
    { {  1.0f,  1.0f }, { 1.0f, 1.0f } },
    { {  1.0f, -1.0f }, { 1.0f, 0.0f } }
  };

  m_vbo.create();
  m_vbo.bind();
  m_vbo.allocate(quad, sizeof(quad));

  // nastavenie atributov
  GLuint attr_pos = m_program.attributeLocation("pos");
  GLuint attr_tex_coords = m_program.attributeLocation("tex_coords_in");

  OGLF->glEnableVertexAttribArray(attr_pos);
  OGLF->glVertexAttribPointer(attr_pos, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);

  OGLF->glEnableVertexAttribArray(attr_tex_coords);
  OGLF->glVertexAttribPointer(attr_tex_coords, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) sizeof(Point2D));

  // Odbindovanie vertex array
  OGLF->glBindVertexArray(0);

  return true;
}


void TextureVolumeRenderer::renderBBox(const QQuaternion & rotation, const QVector3D & scale, const QVector3D & translation)
{
  m_prog_bbox.bind();

  QMatrix4x4 mv;

  mv.translate(translation);
  mv.translate(0.0f, 0.0f, -1.0f);
  mv.rotate(rotation);
  mv.scale(scale);

#if 0
  mv.scale(m_data->maxPhysicalSize() / m_data->physicalWidth(),
           m_data->maxPhysicalSize() / m_data->physicalHeight(),
           m_data->maxPhysicalSize() / m_data->physicalDepth());
#endif

  m_prog_bbox.setUniformValue("proj", m_proj);
  m_prog_bbox.setUniformValue("mv", mv);

  glDrawArrays(GL_LINES, 0, 24);

  m_prog_bbox.release();
}


void TextureVolumeRenderer::render_impl(const QQuaternion & rotation,
                                        const QVector3D & scale,
                                        const QVector3D & translation,
                                        const float peel_depth)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (m_data == nullptr)
  {
    qWarning() << "Not rendering because NO volume data is set";
    return;
  }

  renderBBox(rotation, scale, translation);

  // vypnutie depth testu a zapnutie blendovania
  glDepthMask(GL_FALSE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // aktivovanie shader programu
  m_program.bind();

  // nastavenie rotacie
  QMatrix4x4 tex_matrix;

  // presunutie sa naspat do povodnej polohy
  tex_matrix.translate(0.5f, 0.5f, 0.5f);

  // normalizacia proxy geometrie podla velkosti volumetrickych dat
  // Toto je potrebne, pretoze moje volumetricke data nemusia mat
  // v kazdej dimenzii rovnaky pocet voxelov
#if 0
  tex_matrix.scale(//-PROXY_GEOM_SIZE / float(m_tex_vol_data.width()),
                    PROXY_GEOM_SIZE / float(m_data->width()),
                   -PROXY_GEOM_SIZE / float(m_data->height()),
                    PROXY_GEOM_SIZE / float(m_data->depth()));
                   //-PROXY_GEOM_SIZE / float(m_tex_vol_data.depth()));
#else
  tex_matrix.scale(m_data->maxPhysicalSize() / m_data->physicalWidth(),
                   m_data->maxPhysicalSize() / m_data->physicalHeight(),
                   m_data->maxPhysicalSize() / m_data->physicalDepth());
#endif

  // rotacia s datami
  //tex_matrix.rotate(rotation);
  QMatrix4x4 rot;
  rot.rotate(rotation);

  tex_matrix *= rot.inverted();

  // presunutie sa do stredu volumetrickych dat
  tex_matrix.translate(-0.5f, -0.5f, -0.5f);

  // posun do vnutra dat
  tex_matrix.translate(0.0f, 0.0f, -peel_depth);

  QMatrix4x4 mvp_matrix;
  mvp_matrix *= m_proj;
  mvp_matrix.translate(translation);
  mvp_matrix.translate(0.0f, 0.0f, -1.0f);
  mvp_matrix.scale(scale);

#if 0
  mvp_matrix.scale(m_data->maxPhysicalSize() / m_data->physicalWidth(),
                   m_data->maxPhysicalSize() / m_data->physicalHeight(),
                   m_data->maxPhysicalSize() / m_data->physicalDepth());

  qDebug() << "scale=" << (m_data->maxPhysicalSize() / m_data->physicalWidth())
           << "," << (m_data->maxPhysicalSize() / m_data->physicalHeight())
           << "," << (m_data->maxPhysicalSize() / m_data->physicalDepth());
#endif

  m_program.setUniformValue("mvp_matrix", mvp_matrix);
  m_program.setUniformValue("tex_matrix", tex_matrix);

  // nastavenie textur
  OGLF->glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, m_data->id());

  m_program.setUniformValue("tex_data", 0);

  OGLF->glActiveTexture(GL_TEXTURE1);
  //m_transfer_func.bind();
  glBindTexture(GL_TEXTURE_1D, m_transfer_func.textureId());

  m_program.setUniformValue("tex_transfer_func", 1);

  // vykreslenie proxy geometrie
  m_vao.bind();
  OGLF->glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, NUM_PROXY_QUADS);
  OGLF->glBindVertexArray(0);

  // vykreslenie debugovacieho stvorca
  //glDisable(GL_DEPTH_TEST);
  //m_prog_rectangle.bind();
  //m_prog_rectangle.setUniformValue("mvp", mvp_matrix);
  //OGLF->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  //glEnable(GL_DEPTH_TEST);

  // deaktivovanie shader programu
  OGLF->glUseProgram(0);

  // vratenie blendovania a depth testov do povodneho stavu
  glDisable(GL_BLEND);
  glDepthMask(GL_TRUE);
}
