#include "texture3d.h"
#include "ogl.h"

#include <QFile>
#include <iostream>



bool Texture3D::loadFromRaw(const char *filename, int width, int height, int depth)
{
  QFile f(filename);
  if (!f.open(QFile::ReadOnly))
  {
    std::cerr << "Failed to open file" << filename << std::endl;
    return false;
  }

  QByteArray data(f.readAll());

  const int voxels_cnt = depth * width * height;
  unsigned char* p_rgba = new unsigned char[voxels_cnt * 4];
  const unsigned char *p_luminance = (const unsigned char *) data.data();

  for (int i = 0; i < voxels_cnt; ++i)
  {
    p_rgba[i * 4]     = p_luminance[i];
    p_rgba[i * 4 + 1] = p_luminance[i];
    p_rgba[i * 4 + 2] = p_luminance[i];
    p_rgba[i * 4 + 3] = p_luminance[i];
  }

  GLuint id = 0;
  glGenTextures(1, &id);

  glBindTexture( GL_TEXTURE_3D, id);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  OGLF->glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, p_rgba);
  glBindTexture(GL_TEXTURE_3D, 0);

  if (m_id != 0)
  {
    glDeleteTextures(1, (GLuint *) &m_id);
  }

  m_id = id;
  m_depth = depth;
  m_width = width;
  m_height = height;

  delete[] p_rgba;

  return true;
}
