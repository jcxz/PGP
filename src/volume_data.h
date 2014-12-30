#ifndef VOLUME_DATA_H
#define VOLUME_DATA_H

#include <QOpenGLTexture>
#include <memory>


class VolumeDataBase
{
  public:
    VolumeDataBase(void)
      : m_width(0)
      , m_height(0)
      , m_depth(0)
      , m_bit_depth(8)
      , m_scale_x(1.0f)
      , m_scale_y(1.0f)
      , m_scale_z(1.0f)
    { }

    virtual ~VolumeDataBase(void) { }

    int width(void) const { return m_width; }
    int height(void) const { return m_height; }
    int depth(void) const { return m_depth; }
    int bitDepth(void) const { return m_bit_depth; }
    float scaleX(void) const { return m_scale_x; }
    float scaleY(void) const { return m_scale_y; }
    float scaleZ(void) const { return m_scale_z; }

    void setSize(int width, int height, int depth)
    { m_width = width; m_height = height; m_depth = depth; }

    void setScale(float scalex, float scaley, float scalez)
    { m_scale_x = scalex; m_scale_y = scaley; m_scale_z = scalez; }

    bool setBitDepth(int bit_depth);

    int maxSize(void) const { return std::max(m_width, std::max(m_height, m_depth)); }
    float maxPhysicalSize(void) const { return std::max(physicalWidth(), std::max(physicalHeight(), physicalDepth())); }
    float minPhysicalSize(void) const { return std::min(physicalWidth(), std::min(physicalHeight(), physicalDepth())); }
    float physicalWidth(void) const { return m_width * m_scale_x; }
    float physicalHeight(void) const { return m_height * m_scale_y; }
    float physicalDepth(void) const { return m_depth * m_scale_z; }
    int voxelCount(void) const { return m_width * m_height * m_depth; }
    int maxIntensity(void) const { return m_bit_depth == 8 ? 255 : 65535; }

    static bool isBitDepthSupported(int bit_depth) { return (bit_depth == 8) || (bit_depth == 16); }

  protected:
    int m_width;      // sirka dat
    int m_height;     // vyska dat
    int m_depth;      // hlbka dat
    int m_bit_depth;  // bitova hlbka dat (8/16 bitov)
    float m_scale_x;  // udava realnu fyzicku vzdialenost (asi v milimetroch) medzi jednotlivymi voxelmi na x-ovej osi
    float m_scale_y;  // to iste, ale pre y-ovu os
    float m_scale_z;  // to iste, ale pre z-ovu os
};



class VolumeDataOGL : public VolumeDataBase
{
  public:
    VolumeDataOGL(void)
      : VolumeDataBase()
      , m_tex(QOpenGLTexture::Target3D)
    { }

    bool isValid(void) const { return m_tex.isCreated(); }
    GLuint oglID(void) const { return m_tex.textureId(); }
    QOpenGLTexture & texture(void) { return m_tex; }

  private:
    QOpenGLTexture m_tex;  // id OpenGL textury
};



class VolumeData : public VolumeDataBase
{
  private:
    typedef void (*tFreeFunc)(void *);
    typedef std::unique_ptr<unsigned char, tFreeFunc> tDataPtr;

  public:
    VolumeData(void)
      : VolumeDataBase()
      , m_data(nullptr, &free)
    { }

    const unsigned char *data(void) const { return m_data.get(); }
    unsigned char *data(void) { return m_data.get(); }
    bool isEmpty(void) const { return m_data.get() == nullptr; }

    bool load(const QString & filename);
    bool loadFromDICOM(const QString & filename);
    bool loadFromPVM(const QString & filename);
    bool loadFromRaw(const QString & filename, int width, int height, int depth, int bit_depth);

    bool toVolumeDataOGL(VolumeDataOGL & ogl_data) const;

  private:
    tDataPtr m_data;  // pointer na data
};

#endif // VOLUME_DATA_H
