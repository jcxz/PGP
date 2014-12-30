#include "volume_data_histogram.h"

#include <QDebug>

#include <cmath>



void VolumeDataHistogram::dump(QDebug dbg, int bit_depth) const
{
  int n = std::pow(2, bit_depth);

  for (int i = 0; i < n; ++i)
  {
    dbg << i << ":" << m_hist[i] << "\n";
  }
}
