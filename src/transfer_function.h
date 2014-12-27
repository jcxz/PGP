#ifndef TRANSFER_FUNCTION_H
#define TRANSFER_FUNCTION_H

#include <QVector>
#include <QColor>
#include <QPoint>
#include <QDebug>


class TransferControlPoint
{
  public:
    TransferControlPoint(void)
      : m_pos(), m_col()
    { }

    TransferControlPoint(QPointF pos, QColor col)
      : m_pos(pos), m_col(col)
    { }

    //float alpha(int height) const { return float(m_pos.y()) / float(height); }
    float alpha(void) const { return m_pos.y(); }

    QColor color(void) const { return m_col; }
    void setColor(const QColor & col) { m_col = col; }

    QPointF position(void) const { return m_pos; }
    void setPosition(QPointF pos) { m_pos = pos; }

    friend QDebug operator<<(QDebug debug, const TransferControlPoint & tcp);

  private:
    QPointF m_pos;
    QColor m_col;
};


class TransferFunction
{
  public:
    typedef QVector<TransferControlPoint> tContainer;

  public:
    TransferFunction(void)
      : m_transfer_points()
      , m_cp_left_border(QPointF(0.0f, 0.0f), QColor(Qt::white))
      , m_cp_right_border(QPointF(1.0f, 0.0f), QColor(Qt::black))
    { }

    void addControlPoint(QPointF pos, QColor col)
    { m_transfer_points.push_back(TransferControlPoint(pos, col)); }

    void addControlPoint(const TransferControlPoint & pt)
    { m_transfer_points.push_back(pt); }

    float opacity(float idx) const;
    float opacity(int idx, int index_max = 256) const
    { return opacity(float(idx) / float(index_max)); }

    QColor color(float idx) const;
    QColor color(int idx, int index_max = 256) const
    { return color(float(idx) / float(index_max)); }

    const tContainer & controlPoints(void) const
    { return m_transfer_points; }

    // the mouse_pos and the tolerance have to be transformed to interval [0, 1]
    TransferControlPoint *findByPosition(QPointF mouse_pos, QPointF tolerance = QPointF(0.0f, 0.0f));

    friend QDebug operator<<(QDebug debug, const TransferFunction & tf);

  private:
    // calculate the interpolation parameter t
    float calcT(float idx, const TransferControlPoint * & cp1, const TransferControlPoint * & cp2) const;

  private:
    tContainer m_transfer_points;
    TransferControlPoint m_cp_left_border;
    TransferControlPoint m_cp_right_border;
};

#endif // TRANSFER_FUNCTION_H
