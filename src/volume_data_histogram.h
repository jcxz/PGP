#ifndef VOLUME_DATA_HISTOGRAM_H
#define VOLUME_DATA_HISTOGRAM_H

#include <QHash>
#include <limits>


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

    bool isEmpty(void) const { return m_hist.isEmpty(); }

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

#endif // VOLUME_DATA_HISTOGRAM_H
