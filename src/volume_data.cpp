#include "thirdparty/ddsbase.h"
#include "volume_data.h"
#include "ogl.h"

#include <QFile>
#include <QDebug>
#include <cstdint>



bool VolumeDataBase::setBitDepth(int bit_depth)
{
  if (!isBitDepthSupported(bit_depth))
  {
    qCritical() << "Bit depth" << bit_depth << "is not supported";
    return false;
  }

  m_bit_depth = bit_depth;

  return true;
}


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

  unsigned char *data = readPVMvolume(const_cast<char*>(filename.toStdString().c_str()),
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

  if (!isBitDepthSupported(components * 8))
  {
    free(data);
    qCritical() << "Bit depth" << (components * 8) << "is not supported";
    return false;
  }

  m_data.reset(data);
  m_width = width;
  m_height = height;
  m_depth = depth;
  m_bit_depth = components * 8;
  m_scale_x = scalex;
  m_scale_y = scaley;
  m_scale_z = scalez;

  return true;
}


bool VolumeData::loadFromRaw(const QString & filename, int width, int height, int depth, int bit_depth)
{
  if (!isBitDepthSupported(bit_depth))
  {
    qCritical() << "Bit depth" << bit_depth << "is not supported";
    return false;
  }

  QFile f(filename);
  if (!f.open(QFile::ReadOnly))
  {
    qCritical() << "Failed to open file" << filename;
    return false;
  }

  qint64 size = width * height * depth * (bit_depth / 8);
  unsigned char *data = (unsigned char *) malloc(size);
  if (data == nullptr)
  {
    qCritical() << "Failed to allocate memory to store data from" << filename;
    return false;
  }

  qDebug() << "Reading raw volume" << filename;

  int bytes_read = f.read((char *) data, size);
  if (bytes_read == -1)
  {
    free(data);
    qCritical() << "Failed to read data from" << filename;
    return false;
  }

  qDebug() << "Number of bytes read:" << bytes_read;

  m_data.reset(data);
  m_width = width;
  m_height = height;
  m_depth = depth;
  m_bit_depth = bit_depth;
  m_scale_x = 1.0f;
  m_scale_y = 1.0f;
  m_scale_z = 1.0f;

  qDebug() << "Size: "      << m_width  << " x " << m_height << " x " << m_depth;
  qDebug() << "Spacing: "   << m_scale_x << " x " << m_scale_y << " x " << m_scale_z;
  qDebug() << "Bit depth: " << m_bit_depth;

  return true;
}


namespace {

template <typename T>
const T *intensityToRGBA(const T *p_intensity, const int n)
{
  T *p_rgba = (T *) malloc(sizeof(T) * n * 4);
  if (p_rgba == nullptr)
  {
    qCritical() << "Failed to allocate memory for the conversion";
    return nullptr;
  }

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


bool VolumeData::toVolumeDataOGL(VolumeDataOGL & ogl_data) const
{
  // Konverzia z grayscale na rgba (kvoli opengl, lebo format moze byt len GL_RGBA)
  uint8_t *p_rgba = nullptr;
  GLenum data_type = 0;

  if (m_bit_depth == 8)
  {
    p_rgba = (uint8_t *) intensityToRGBA<uint8_t>((uint8_t *) m_data.get(), voxelCount());
    data_type = GL_UNSIGNED_BYTE;
  }
  else if (m_bit_depth == 16)
  {
    p_rgba = (uint8_t *) intensityToRGBA<uint16_t>((uint16_t *) m_data.get(), voxelCount());
    data_type = GL_UNSIGNED_SHORT;
  }
  else
  {
    qWarning() << "Failed to convert volume data to OpenGL texture."
                  "Unsupported bit depth for raw data";
    return false;
  }

  if (p_rgba == nullptr)
  {
    qCritical() << "Failed to convert volume data to OpenGL texture."
                   "Conversion from luminance to RGBA format failed";
    return false;
  }

  // nahratie dat do textury
  if (!ogl_data.texture().create())
  {
    free(p_rgba);
    qCritical() << "Failed to convert volume data to OpenGL texture."
                   "Failed to create OpenGL texture";
    return false;
  }

  OGLF->glBindTexture(GL_TEXTURE_3D, ogl_data.oglID());

  OGLF->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  OGLF->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  OGLF->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
  OGLF->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  OGLF->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  OGLF->glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, m_width, m_height, m_depth, 0, GL_RGBA, data_type, p_rgba);

  OGLF->glBindTexture(GL_TEXTURE_3D, 0);

  // nastavenie parametrov dat
  ogl_data.setSize(m_width, m_height, m_depth);
  ogl_data.setBitDepth(m_bit_depth);
  ogl_data.setScale(m_scale_x, m_scale_y, m_scale_z);

  // uvolnenie zdrojov
  free(p_rgba);

  return true;
}
