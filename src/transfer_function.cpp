#include "transfer_function.h"

#include <QDataStream>
#include <QFile>
#include <cassert>



QDebug operator<<(QDebug debug, const TransferControlPoint & tcp)
{
  bool b = debug.autoInsertSpaces();

  debug.nospace() << "TransferControlPoint(" << tcp.position();
  debug.nospace() << ", ";
  debug.nospace() << tcp.color();
  debug.nospace() << ", Alpha(" << tcp.alpha() << "))";

  debug.setAutoInsertSpaces(b);

  return debug;
}


QDataStream & operator>>(QDataStream & stream, TransferControlPoint & tcp)
{
  return stream >> tcp.m_col >> tcp.m_pos;
}


QDataStream & operator<<(QDataStream & stream, const TransferControlPoint & tcp)
{
  return stream << tcp.m_col << tcp.m_pos;
}


void TransferFunction::setTCPPosition(int idx, QPointF pos)
{
  qDebug() << __PRETTY_FUNCTION__;

  if (!isValid())
  {
    qWarning() << "setTCPPosition: Transfer function is invalid";
    return;
  }

  TransferControlPoint & pt_prev = getTCP(idx - 1);
  TransferControlPoint & pt = getTCP(idx);
  TransferControlPoint & pt_next = getTCP(idx + 1);

  if (pos.y() > 1.0f) pos.setY(1.0f);
  if (pos.y() < 0.0f) pos.setY(0.0f);

  if (idx <= 0)
  {
    pos.setX(m_transfer_points.first().position().x());
  }
  else if (idx >= (m_transfer_points.size() - 1))
  {
    pos.setX(m_transfer_points.last().position().x());
  }
  else
  {
    if (pos.x() < pt_prev.position().x()) pos.setX(pt_prev.position().x());
    if (pos.x() > pt_next.position().x()) pos.setX(pt_next.position().x());
  }

  pt.setPosition(pos);
}


void TransferFunction::setTCPColor(int idx, const QColor & col)
{
  if ((idx >= 0) && (idx < m_transfer_points.size()))
  {
    m_transfer_points[idx].setColor(col);
  }
}


void TransferFunction::removeTCP(int idx)
{
  if ((idx > 0) && (idx < (m_transfer_points.size() - 1)))
  {
    m_transfer_points.remove(idx);
  }
}


int TransferFunction::findByPosition(QPointF mouse_pos, QPointF tolerance)
{
  qDebug() << __PRETTY_FUNCTION__;

  for (int i = 0; i < m_transfer_points.size(); ++i)
  {
    QPointF pos = m_transfer_points[i].position();
    //qDebug() << "mouse_pos=" << mouse_pos << ", pos=" << pos << ", tolerance=" << tolerance;
    if ((fabs(pos.x() - mouse_pos.x()) <= tolerance.x()) &&
        (fabs(pos.y() - mouse_pos.y()) <= tolerance.y()))
    {
      return i;
    }
  }

  return INVALID_TCP_INDEX;
}


float TransferFunction::calcT(float idx, const TransferControlPoint * & cp1, const TransferControlPoint * & cp2) const
{
#if 0
  // find upper boundary
  //cp1 = &m_cp_left_border;
  //for (int i = 0; i < m_transfer_points.size(); ++i)

  cp1 = &m_transfer_points[0];
  for (int i = 1; i < m_transfer_points.size(); ++i)
  {
    QPointF pos = m_transfer_points[i].position();
    if (idx >= pos.x())
    {
      cp1 = &m_transfer_points[i];
      break;
    }
  }

  // find lower boundary
  //cp2 = &m_cp_right_border;
  //for (int i = 0; i < m_transfer_points.size(); ++i)

  cp2 = &m_transfer_points.last();
  for (int i = 0; i < (m_transfer_points.size() - 1); ++i)
  {
    QPointF pos = m_transfer_points[i].position();
    if (idx <= pos.x())
    {
      cp2 = &m_transfer_points[i];
      break;
    }
  }
#else
  // find upper boundary
  for (int i = 1; i < m_transfer_points.size(); ++i)
  {
    QPointF pos1 = m_transfer_points[i - 1].position();
    QPointF pos2 = m_transfer_points[i].position();
    if ((idx >= pos1.x()) && (idx <= pos2.x()))
    {
      cp1 = &m_transfer_points[i - 1];
      cp2 = &m_transfer_points[i];
      break;
    }
  }

  assert(cp1 != nullptr);
  assert(cp2 != nullptr);
#endif

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


bool TransferFunction::load(const QString & filename)
{
  QFile file(filename);
  if (!file.open(QFile::ReadOnly))
  {
    qWarning() << "Failed to open transfer function file " << filename;
    return false;
  }

  QDataStream stream(&file);

  stream >> m_transfer_points;

  return stream.status() == QDataStream::Ok;
}


bool TransferFunction::save(const QString & filename) const
{
  QFile file(filename);
  if (!file.open(QFile::WriteOnly))
  {
    qWarning() << "Failed to create transfer function file " << filename;
    return false;
  }

  QDataStream stream(&file);

  stream << m_transfer_points;

  return stream.status() == QDataStream::Ok;
}


QDataStream & operator>>(QDataStream & stream, TransferFunction & tf)
{
  return stream >> tf.m_transfer_points;
}


QDataStream & operator<<(QDataStream & stream, const TransferFunction & tf)
{
  return stream << tf.m_transfer_points;
}


QDebug operator<<(QDebug debug, const TransferFunction & tf)
{
  const TransferFunction::tContainer & tcps = tf.controlPoints();

  for (int i = 0; i < tcps.size(); ++i)
  {
    debug.nospace() << tcps[i] << "\n";
  }

  return debug;
}
