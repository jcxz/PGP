#include "texture_volume_renderer.h"
#include "ogl.h"

// Tato konstanta predstavuje world-space velkost mojich volumetrickych dat,
// resp. work-size velkost bounding box-u okolo tychto mojich volumetrickych dat
#define PROXY_GEOM_SIZE 256.0f //100.0f
#define NUM_PROXY_QUADS 300    //256    //109 //218 //109 //256 //109 //100
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


TextureVolumeRenderer::~TextureVolumeRenderer(void)
{
  OGLF->glDeleteVertexArrays(1, &m_vao);
  glDeleteTextures(1, &m_tex_transfer_func);
}


void TextureVolumeRenderer::genTransferFunc(void)
{
  glGenTextures(1, &m_tex_transfer_func);
  glBindTexture(GL_TEXTURE_1D, m_tex_transfer_func);

  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  const int width = 256;
#if 0
  unsigned char (*pixels)[4] = new unsigned char[width][4];

  for (int i = 0; i < width; ++i)
  {
    pixels[i][0] = 255;
    pixels[i][1] = 0;
    pixels[i][2] = 0;
    pixels[i][3] = 255;
  }
#else
  unsigned char *pixels = new unsigned char[width * 4];

  for (int i = 0; i < width; i += 4)
  {
    pixels[i + 0] = 255;
    pixels[i + 1] = 255;
    pixels[i + 2] = 255;
    pixels[i + 3] = i;
  }
#endif

  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

  delete [] pixels;

  glBindTexture(GL_TEXTURE_1D, 0);
}


bool TextureVolumeRenderer::reset(void)
{
  // shader pre bounding box
  m_prog_bbox.addShaderFromSourceFile(QOpenGLShader::Vertex,   ":/src/opengl/wire_box.vert");
  m_prog_bbox.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/src/opengl/wire_box.frag");
  m_prog_bbox.link();

  m_prog_bbox.bind();
  m_prog_bbox.setUniformValue("col", QVector3D(1.0f, 0.0f, 0.0f));
  m_prog_bbox.setUniformValue("dimensions", QVector3D(1.0f, 1.0f, 1.0f));
  m_prog_bbox.release();

  // kompilacia shaderov
  m_program.addShaderFromSourceFile(QOpenGLShader::Vertex,   ":/src/opengl/texture_volume_renderer.vert");
  m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/src/opengl/texture_volume_renderer.frag");
  m_program.link();

  // atributy a uniformne premenne
  m_attr_pos = m_program.attributeLocation("pos");
  m_attr_tex_coords = m_program.attributeLocation("tex_coords_in");

  m_program.bind();
  m_program.setUniformValue("num_instances", (GLfloat) NUM_PROXY_QUADS);
  m_program.setUniformValue("num_instances_inv", 1.0f / ((GLfloat) NUM_PROXY_QUADS));
  OGLF->glUseProgram(0);

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
  OGLF->glVertexAttribPointer(m_attr_pos, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);

  OGLF->glEnableVertexAttribArray(m_attr_tex_coords);
  OGLF->glVertexAttribPointer(m_attr_tex_coords, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) sizeof(Point2D));

  // Odbindovanie vertex array
  OGLF->glBindVertexArray(0);

  // Nacitanie volumetrickych data do 3D textury
  if (!m_tex_vol_data.loadFromRaw(":/data/head256x256x109_8bit_chan.raw", 256, 256, 109))
  {
    qWarning("failed to load default volumetric data");
    return false;
  }

  return true;
}


void TextureVolumeRenderer::renderBBox(const QQuaternion & rotation, const QVector3D & scale, const QVector3D & translation)
{
  m_prog_bbox.bind();

  QMatrix4x4 mv;

  mv.translate(0.0f, 0.0f, -1.0f);
  mv.rotate(rotation);
  mv.scale(scale);

  m_prog_bbox.setUniformValue("proj", m_proj);
  m_prog_bbox.setUniformValue("mv", mv);

  glDrawArrays(GL_LINES, 0, 24);

  m_prog_bbox.release();
}


void TextureVolumeRenderer::render(const QQuaternion & rotation, const QVector3D & scale, const QVector3D & translation)
{
#if 1
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  renderBBox(rotation, scale, translation);

#if 1
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
  // kazdej dimenzii rovnaky pocet voxelov
  tex_matrix.scale(//-PROXY_GEOM_SIZE / float(m_tex_vol_data.width()),
                    PROXY_GEOM_SIZE / float(m_tex_vol_data.width()),
                   -PROXY_GEOM_SIZE / float(m_tex_vol_data.height()),
                    PROXY_GEOM_SIZE / float(m_tex_vol_data.depth()));
                   //-PROXY_GEOM_SIZE / float(m_tex_vol_data.depth()));

  // rotacia s datami
  //tex_matrix.rotate(rotation);
  QMatrix4x4 rot;
  rot.rotate(rotation);

  tex_matrix *= rot.inverted();

  // presunutie sa do stredu volumetrickych dat
  tex_matrix.translate(-0.5f, -0.5f, -0.5f);

  QMatrix4x4 mvp_matrix;
  mvp_matrix *= m_proj;
  mvp_matrix.translate(0.0f, 0.0f, -1.0f);
  mvp_matrix.scale(scale);

  m_program.setUniformValue("mvp_matrix", mvp_matrix);
  m_program.setUniformValue("tex_matrix", tex_matrix);

  // nastavenie textur
  OGLF->glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, m_tex_vol_data.id());

  m_program.setUniformValue("tex_data", 0);

  OGLF->glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_1D, m_tex_transfer_func);

  m_program.setUniformValue("tex_transfer_func", 1);

  // vykreslenie proxy geometrie
  OGLF->glBindVertexArray(m_vao);
  OGLF->glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, NUM_PROXY_QUADS);
  OGLF->glBindVertexArray(0);

  // deaktivovanie shader programu
  OGLF->glUseProgram(0);

  // vratenie blendovania a depth testov do povodneho stavu
  glDisable(GL_BLEND);
  glDepthMask(GL_TRUE);
#endif
#elif 0
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // vypnutie depth testu a zapnutie blendovania
  //glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
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
  tex_matrix.rotate(rotation);

  // presunutie sa naspat do povodnej polohy
  tex_matrix.translate(-0.5f, -0.5f, -0.5f);

  //tex_matrix.translate(0.0f, 0.0f, m_depth_data);
  //tex_matrix.scale(scale);

  //QMatrix4x4 mvp_matrix;
  //mvp_matrix.perspective(30.0f, (float) width() / (float) height(), 0.1f, 100.0f);
  //mvp_matrix.translate(0.0f, 0.0f, m_dist_z);

  QMatrix4x4 mvp_matrix;
  //mvp_matrix *= m_proj;
  mvp_matrix.scale(scale);
  //mvp_matrix.translate(0.0f, 0.0f, scale.z() * -2.0f);//-10.0f);

  m_program.setUniformValue("mvp_matrix", mvp_matrix);
  //m_program.setUniformValue("mvp_matrix", mvp);
  //m_program.setUniformValue("mvp_matrix", m_proj);
  m_program.setUniformValue("tex_matrix", tex_matrix);

  // nastavenie textur
  OGLF->glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, m_tex_vol_data.id());

  m_program.setUniformValue("tex_data", 0);

  OGLF->glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_1D, m_tex_transfer_func);

  m_program.setUniformValue("tex_transfer_func", 1);

  // vykreslenie proxy geometrie
  OGLF->glBindVertexArray(m_vao);
  OGLF->glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, NUM_PROXY_QUADS);
  OGLF->glBindVertexArray(0);

  // deaktivovanie shader programu
  OGLF->glUseProgram(0);

  // vratenie blendovania a depth testov do povodneho stavu
  //glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
#endif
}
