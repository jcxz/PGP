#ifndef TEXTURE3D_H
#define TEXTURE3D_H

#include <QtGui/QOpenGLFunctions>


class Texture3D
{
  public:
    Texture3D(void)
      : m_id(0)
      , m_width(0)
      , m_height(0)
      , m_depth(0)
    { }

    ~Texture3D(void) { }

    unsigned int id(void) const { return m_id; }
    int width(void) const { return m_width; }
    int height(void) const { return m_height; }
    int depth(void) const { return m_depth; }

    bool loadFromRaw(const char *filename, int width, int height, int depth);

  private:
    unsigned int m_id;
    int m_width;
    int m_height;
    int m_depth;
};

#endif // TEXTURE3D_H
