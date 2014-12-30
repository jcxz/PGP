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

























#if 0
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QHash>
#include <limits>
#include <memory>


class VolumeDataHistogram
{
  private:
    // klucom je intenzita pixelu a hodnotou pocet pixelov s danou intenzitou
    typedef QHash<int, int> tContainer;

  public:
    VolumeDataHistogram(void)
      : m_hist()
      , m_max(std::numeric_limits<int>::min())
    { }

    int value(int intensity) const
    {
      tContainer::const_iterator it = m_hist.find(intensity);
      if (it == m_hist.end()) return 0;
      return *it;
    }

    int max(void) const { return m_max; }

    template <typename T>
    void rebuild(const T *p, int n)
    {
      m_hist.clear();
      m_max = std::numeric_limits<int>::min();
      for (int i = 0; i < n; ++i)
      {
        int val = ++m_hist[p[i]];
        if (val > m_max) m_max = val;
      }
    }

    void dump(QDebug dbg, int bit_depth) const;

  private:
    tContainer m_hist;
    int m_max;
};


class VolumeData
{
  public:
    VolumeData(void)
      : m_data(nullptr, &free)
      , m_tex(QOpenGLTexture::Target3D)
      , m_width(0)
      , m_height(0)
      , m_depth(0)
      , m_bit_depth(8)
      , m_scale_x(1.0f)
      , m_scale_y(1.0f)
      , m_scale_z(1.0f)
      , m_hist()
    { }

    //bool isValid(void) const { return m_id != 0; }

    unsigned int id(void) const { return m_tex.textureId(); }
    int width(void) const { return m_width; }
    int height(void) const { return m_height; }
    int depth(void) const { return m_depth; }
    int bitDepth(void) const { return m_bit_depth; }
    float scaleX(void) const { return m_scale_x; }
    float scaleY(void) const { return m_scale_y; }
    float scaleZ(void) const { return m_scale_z; }

    int maxSize(void) const { return std::max(m_width, std::max(m_height, m_depth)); }
    float maxPhysicalSize(void) const { return std::max(physicalWidth(), std::max(physicalHeight(), physicalDepth())); }
    float minPhysicalSize(void) const { return std::min(physicalWidth(), std::min(physicalHeight(), physicalDepth())); }
    float physicalWidth(void) const { return m_width * m_scale_x; }
    float physicalHeight(void) const { return m_height * m_scale_y; }
    float physicalDepth(void) const { return m_depth * m_scale_z; }

    int voxelCount(void) const { return m_width * m_height * m_depth; }
    int maxIntensity(void) const { return m_bit_depth == 8 ? 255 : 65535; }

    const VolumeDataHistogram & intensityHistogram(void) const { return m_hist; }

    bool load(const QString & filename);
    bool loadFromDICOM(const QString & filename);
    bool loadFromPVM(const QString & filename);
    bool loadFromRaw(const QString & filename, int width, int height, int depth, int bit_depth);

  private:
    bool loadRaw(const void *p_luminance, int width, int height, int depth, int bit_depth);

  private:
    typedef void (*tFreeFunc)(void *);

  private:
    std::unique_ptr<unsigned char, tFreeFunc> m_data;
    QOpenGLTexture m_tex;        // OpenGL 3D textura s datami
    int m_width;                 // sirka dat
    int m_height;                // vyska dat
    int m_depth;                 // hlbka dat
    int m_bit_depth;             // bitova hlbka dat (8/16 bitov)
    float m_scale_x;             // udava realnu fyzicku vzdialenost (asi v milimetroch) medzi jednotlivymi voxelmi na x-ovej osi
    float m_scale_y;             // to iste, ale pre y-ovu os
    float m_scale_z;             // to iste, ale pre z-ovu os
    VolumeDataHistogram m_hist;  // histogram roznych intenzit v datach
};
#endif

#endif // VOLUME_DATA_H
