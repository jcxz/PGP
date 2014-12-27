#include "transfer_function.h"



QDebug operator<<(QDebug debug, const TransferControlPoint & tcp)
{
  return debug.nospace() << "TransferControlPoint(" << tcp.position() << ", " << tcp.color() << ", Alpha(" << tcp.alpha() << "))";
}


TransferControlPoint *TransferFunction::findByPosition(QPointF mouse_pos, QPointF tolerance)
{
  for (TransferControlPoint & pt : m_transfer_points)
  {
    QPointF pos = pt.position();
    if ((fabs(pos.x() - mouse_pos.x()) <= tolerance.x()) &&
        (fabs(pos.y() - mouse_pos.y()) <= tolerance.y()))
    {
      return &pt;
    }
  }

  return nullptr;
}


float TransferFunction::calcT(float idx, const TransferControlPoint * & cp1, const TransferControlPoint * & cp2) const
{
  // find upper boundary
  cp1 = &m_cp_left_border;
  for (int i = 0; i < m_transfer_points.size(); ++i)
  {
    QPointF pos = m_transfer_points[i].position();
    if (idx >= pos.x())
    {
      cp1 = &m_transfer_points[i];
      break;
    }
  }

  // find lower boundary
  cp2 = &m_cp_right_border;
  for (int i = 0; i < m_transfer_points.size(); ++i)
  {
    QPointF pos = m_transfer_points[i].position();
    if (idx <= pos.x())
    {
      cp2 = &m_transfer_points[i];
      break;
    }
  }

  float p1_x = cp1->position().x();
  float p2_x = cp2->position().x();

  return float(idx - p1_x) / float(p2_x - p1_x);
}


float TransferFunction::opacity(float idx) const
{
  const TransferControlPoint *p1 = nullptr;
  const TransferControlPoint *p2 = nullptr;
  const float t = calcT(idx, p1, p2);

  return p1->alpha() * t + p2->alpha() * (1.0f - t);
}


QColor TransferFunction::color(float idx) const
{
  const TransferControlPoint *p1 = nullptr;
  const TransferControlPoint *p2 = nullptr;

  const float t = calcT(idx, p1, p2);

  const QColor & p1_col = p1->color();
  const QColor & p2_col = p2->color();

  //qDebug() << "red=" << (p1_col.red()   * t + p2_col.red()   * (1.0f - t))
  //         << ", green=" << (p1_col.green() * t + p2_col.green() * (1.0f - t))
  //         << ", blue=" << (p1_col.blue()  * t + p2_col.blue()  * (1.0f - t));

  QColor c(p1_col.red()   * t + p2_col.red()   * (1.0f - t),
           p1_col.green() * t + p2_col.green() * (1.0f - t),
           p1_col.blue()  * t + p2_col.blue()  * (1.0f - t));

  return c;
}


QDebug operator<<(QDebug debug, const TransferFunction & tf)
{
  const TransferFunction::tContainer & tcps = tf.controlPoints();

  for (int i = 0; i < tcps.size(); ++i)
  {
    debug << tcps[i] << "\n";
  }

  return debug;
}
