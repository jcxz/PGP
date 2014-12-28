/*
 * The PVM loader implementation was taken from Voreen project: http://www.voreen.org
 */

// include this before any windows headers
#include "thirdparty/ddsbase.h"
#include "volume_data.h"
#include "ogl.h"

#include <QFile>
#include <QDebug>

#include <fstream>
#include <iostream>
#include <cstdint>



namespace {

inline void endian_swap(uint16_t & x)
{
  x = (x >> 8) | (x << 8);
}

template <typename T>
const T *intensityToRGBA(const T *p_intensity, const int n)
{
  T *p_rgba = new T[n * 4];

  for (int i = 0; i < n; ++i)
  {
    p_rgba[i * 4]     = p_intensity[i];
    p_rgba[i * 4 + 1] = p_intensity[i];
    p_rgba[i * 4 + 2] = p_intensity[i];
    p_rgba[i * 4 + 3] = p_intensity[i];
  }

  return p_rgba;
}

} // End of private namespace



bool VolumeData::load(const QString & filename)
{
  if (filename.endsWith(".pvm"))
    return loadFromPVM(filename);
  else if (filename.endsWith(".dcm"))
    return loadFromDICOM(filename);
  else
  {
    qWarning() << "Unsupported filetype";
    return false;
  }
}


bool VolumeData::loadFromDICOM(const QString & filename)
{
  (void) filename;
  return false;
}


bool VolumeData::loadFromPVM(const QString & filename)
{
  unsigned int width, height, depth, components;
  float scalex, scaley, scalez;
  unsigned char *description;
  unsigned char *courtesy;
  unsigned char *parameter;
  unsigned char *comment;

  qDebug() << "Reading PVM volume" << filename;

  uint8_t *data = readPVMvolume(const_cast<char*>(filename.toStdString().c_str()),
                                &width, &height, &depth, &components,
                                &scalex, &scaley, &scalez, &description, &courtesy,
                                &parameter, &comment);
  if (!data)
  {
    qWarning() << "PVM Reading failed";
    return false;
  }

  qDebug() << "Size: "       << width  << " x " << height << " x " << depth;
  qDebug() << "Spacing: "    << scalex << " x " << scaley << " x " << scalez;
  qDebug() << "Components: " << components;
  if (description) qDebug() << "Description: " << (char *) description;
  if (courtesy)    qDebug() << "Courtesy: "    << (char *) courtesy;
  if (parameter)   qDebug() << "Parameter: "   << (char *) parameter;
  if (comment)     qDebug() << "Comment: "     << (char *) comment;

  if (components == 1)
  {
    qDebug() << "Create 8 bit data set.";
    //loadRaw8bit(data, width, height, depth);
    loadRaw(data, width, height, depth, 8);
  }
#if 1
  else if (components == 2)
  {
    // the endianness conversion in ddsbase.cpp seem to be broken,
    // so we perform it here instead
/*
    uint16_t *data16 = reinterpret_cast<uint16_t *>(data);
    int n = width * height * depth;

    for (int i = 0; i < n; ++i)
    {
      endian_swap(data16[i]);
    }
*/
    loadRaw(data, width, height, depth, 16);
  }
#endif
  else
  {
    free(data);
    qWarning() << "Bit depth not supported.";
    return false;
  }

  // TODO now it is safe to free
  free(data);

  m_scale_x = scalex;
  m_scale_y = scaley;
  m_scale_z = scalez;

  return true;
}


bool VolumeData::loadFromRaw(const QString & filename, int width, int height, int depth, int bit_depth)
{
  QFile f(filename);
  if (!f.open(QFile::ReadOnly))
  {
    qCritical() << "Failed to open file" << filename;
    return false;
  }

  QByteArray data(f.readAll());

  qDebug() << "Reading raw volume" << filename;

  //return loadRaw8bit((const unsigned char *) data.data(), width, height, depth);
  if (!loadRaw((const unsigned char *) data.data(), width, height, depth, bit_depth)) return false;

  m_scale_x = 1.0f;
  m_scale_y = 1.0f;
  m_scale_z = 1.0f;

  qDebug() << "Size: "      << m_width  << " x " << m_height << " x " << m_depth;
  qDebug() << "Spacing: "   << m_scale_x << " x " << m_scale_y << " x " << m_scale_z;
  qDebug() << "Bit depth: " << m_bit_depth;

  return true;
}


/*
bool VolumeData::loadRaw8bit(const unsigned char *p_luminance, int width, int height, int depth)
{
  const int voxels_cnt = depth * width * height;
  unsigned char* p_rgba = new unsigned char[voxels_cnt * 4];

  for (int i = 0; i < voxels_cnt; ++i)
  {
    p_rgba[i * 4]     = p_luminance[i];
    p_rgba[i * 4 + 1] = p_luminance[i];
    p_rgba[i * 4 + 2] = p_luminance[i];
    p_rgba[i * 4 + 3] = p_luminance[i];
  }

  GLuint id = 0;
  glGenTextures(1, &id);

  glBindTexture(GL_TEXTURE_3D, id);
  //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
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

  delete [] p_rgba;

  return true;
}
*/


bool VolumeData::loadRaw(const void *p_luminance, int width, int height, int depth, int bit_depth)
{
  // Konverzia z grayscale na rgba (kvoli opengl, lebo format moze byt len GL_RGBA)
  const int voxels_cnt = depth * width * height;
  void *p_rgba = nullptr;
  GLenum data_type = 0;

  if (bit_depth == 8)
  {
    p_rgba = (void *) intensityToRGBA<uint8_t>((uint8_t *) p_luminance, voxels_cnt);
    data_type = GL_UNSIGNED_BYTE;
    m_hist.rebuild((uint8_t *) p_luminance, voxels_cnt);
  }
  else if (bit_depth == 16)
  {
    p_rgba = (void *) intensityToRGBA<uint16_t>((uint16_t *) p_luminance, voxels_cnt);
    data_type = GL_UNSIGNED_SHORT;
    m_hist.rebuild((uint16_t *) p_luminance, voxels_cnt);
  }
  else
  {
    qWarning() << "Unsupported bit depth for raw data";
    return false;
  }

  // nahratie dat do textury
  GLuint id = 0;
  glGenTextures(1, &id);

  glBindTexture(GL_TEXTURE_3D, id);
  //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


  OGLF->glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, height, depth, 0, GL_RGBA, data_type, p_rgba);
  glBindTexture(GL_TEXTURE_3D, 0);

  if (m_id != 0)
  {
    glDeleteTextures(1, (GLuint *) &m_id);
  }

  m_id = id;
  m_depth = depth;
  m_width = width;
  m_height = height;
  m_bit_depth = bit_depth;

  delete [] p_rgba;

  return true;
}
