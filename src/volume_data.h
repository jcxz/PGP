#ifndef VOLUME_DATA_H
#define VOLUME_DATA_H

#include <QOpenGLFunctions>


class VolumeData
{
  public:
    VolumeData(void)
      : m_id(0)
      , m_width(0)
      , m_height(0)
      , m_depth(0)
    { }

    ~VolumeData(void) { }

    unsigned int id(void) const { return m_id; }
    int width(void) const { return m_width; }
    int height(void) const { return m_height; }
    int depth(void) const { return m_depth; }

    bool load(const QString & filename);
    bool loadFromRaw(const QString & filename, int width, int height, int depth);

  private:
    unsigned int m_id;
    int m_width;
    int m_height;
    int m_depth;
};

#endif // VOLUME_DATA_H
