/*
 * The PVM loader implementation was taken from Voreen project: http://www.voreen.org
 */

#include "thirdparty/ddsbase.h"
#include "volume_data.h"
#include "ogl.h"

#include <QFile>
#include <QDebug>

#include <fstream>
#include <iostream>
#include <cstdint>



namespace {

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

  bool res = true;

  if (components == 1)
  {
    qDebug() << "Creating 8 bit data set ...";
    res = loadRaw(data, width, height, depth, 8);
  }
  else if (components == 2)
  {
    qDebug() << "Creating 16 bit data set ...";
    res = loadRaw(data, width, height, depth, 16);
  }
  else
  {
    qWarning() << "Bit depth" << (components * 8) << "not supported.";
    res = false;
  }

  free(data);

  m_scale_x = scalex;
  m_scale_y = scaley;
  m_scale_z = scalez;

  return res;
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

  if (!loadRaw((const unsigned char *) data.data(), width, height, depth, bit_depth)) return false;

  m_scale_x = 1.0f;
  m_scale_y = 1.0f;
  m_scale_z = 1.0f;

  qDebug() << "Size: "      << m_width  << " x " << m_height << " x " << m_depth;
  qDebug() << "Spacing: "   << m_scale_x << " x " << m_scale_y << " x " << m_scale_z;
  qDebug() << "Bit depth: " << m_bit_depth;

  return true;
}


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
  m_tex.create();

  OGLF->glBindTexture(GL_TEXTURE_3D, m_tex.textureId());

  OGLF->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  OGLF->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  OGLF->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
  OGLF->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  OGLF->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  OGLF->glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, height, depth, 0, GL_RGBA, data_type, p_rgba);

  OGLF->glBindTexture(GL_TEXTURE_3D, 0);

  m_depth = depth;
  m_width = width;
  m_height = height;
  m_bit_depth = bit_depth;

  delete [] p_rgba;

  return true;
}
