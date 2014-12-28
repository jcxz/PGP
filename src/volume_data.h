#ifndef VOLUME_DATA_H
#define VOLUME_DATA_H

#include <QOpenGLFunctions>
#include <QHash>
#include <limits>


class VolumeDataHistogram
{
  private:
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

  private:
    tContainer m_hist;
    int m_max;
};


class VolumeData
{
  public:
    VolumeData(void)
      : m_id(0)
      , m_width(0)
      , m_height(0)
      , m_depth(0)
      , m_bit_depth(8)
      , m_scale_x(1.0f)
      , m_scale_y(1.0f)
      , m_scale_z(1.0f)
      , m_hist()
    { }

    ~VolumeData(void) { glDeleteTextures(1, (GLuint *) &m_id); }

    bool isValid(void) const { return m_id != 0; }
    unsigned int id(void) const { return m_id; }
    int width(void) const { return m_width; }
    int height(void) const { return m_height; }
    int depth(void) const { return m_depth; }

    float maxPhysicalSize(void) const { return std::max(physicalWidth(), std::max(physicalHeight(), physicalDepth())); }
    float minPhysicalSize(void) const { return std::min(physicalWidth(), std::min(physicalHeight(), physicalDepth())); }
    float physicalWidth(void) const { return m_width * m_scale_x; }
    float physicalHeight(void) const { return m_height * m_scale_y; }
    float physicalDepth(void) const { return m_depth * m_scale_z; }

    int voxelCount(void) const { return m_width * m_height * m_depth; }
    int bitDepth(void) const { return m_bit_depth; }
    int maxIntensity(void) const { return m_bit_depth == 8 ? 255 : 65535; }
    float scaleX(void) const { return m_scale_x; }
    float scaleY(void) const { return m_scale_y; }
    float scaleZ(void) const { return m_scale_z; }

    const VolumeDataHistogram & intensityHistogram(void) const { return m_hist; }

    bool load(const QString & filename);
    bool loadFromDICOM(const QString & filename);
    bool loadFromPVM(const QString & filename);
    bool loadFromRaw(const QString & filename, int width, int height, int depth, int bit_depth);

  private:
    //bool loadRaw8bit(const unsigned char *p_luminance, int width, int height, int depth);
    //bool loadRaw16bit(const unsigned char *p_luminance, int width, int height, int depth);
    bool loadRaw(const void *p_luminance, int width, int height, int depth, int bit_depth);

  private:
    unsigned int m_id;  // id OpenGL 3D textury
    int m_width;        // sirka dat
    int m_height;       // vyska dat
    int m_depth;        // hlbka dat
    int m_bit_depth;    // bitova hlbka dat (8/16 bitov)
    float m_scale_x;
    float m_scale_y;
    float m_scale_z;
    VolumeDataHistogram m_hist;  // histogram roznych intenzit v datach
};

#endif // VOLUME_DATA_H
