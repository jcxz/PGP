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

    static constexpr int INVALID_TCP_INDEX = -1;

  public:
    TransferFunction(void)
      : m_transfer_points()
      //, m_cp_left_border(QPointF(0.0f, 0.0f), QColor(Qt::white))
      //, m_cp_right_border(QPointF(1.0f, 0.0f), QColor(Qt::black))
    {
      // dva specialne krajne body, pretoze vo funkcii musia byt vzdy
      // aspon 2 krajne body
      m_transfer_points.push_back(TransferControlPoint(QPointF(0.0f, 0.0f), QColor(Qt::white)));
      m_transfer_points.push_back(TransferControlPoint(QPointF(1.0f, 0.0f), QColor(Qt::black)));
    }

    void addTCP(QPointF pos, QColor col)
    { addTCP(TransferControlPoint(pos, col)); }

    void addTCP(const TransferControlPoint & pt)
    {
      m_transfer_points.insert(m_transfer_points.size() - 1, pt);
      //m_transfer_points.push_back(pt);
    }

    void removeTCP(int idx);
    void setTCPPosition(int idx, QPointF pos);
    void setTCPColor(int idx, const QColor & col);

    float opacity(float idx) const;
    float opacity(int idx, int index_max = 256) const
    { return opacity(float(idx) / float(index_max)); }

    QColor color(float idx) const;
    QColor color(int idx, int index_max = 256) const
    { return color(float(idx) / float(index_max)); }

    const tContainer & controlPoints(void) const
    { return m_transfer_points; }

    // the mouse_pos and the tolerance have to be transformed to interval [0, 1]
    int findByPosition(QPointF mouse_pos, QPointF tolerance = QPointF(0.0f, 0.0f));

    friend QDebug operator<<(QDebug debug, const TransferFunction & tf);

  private:
    // calculate the interpolation parameter t
    float calcT(float idx, const TransferControlPoint * & cp1, const TransferControlPoint * & cp2) const;

    //inline bool isValid(void) const { return !m_transfer_points.isEmpty(); }
    inline bool isValid(void) const { return m_transfer_points.size() >= 3; }

    inline TransferControlPoint & getTCP(int idx)
    {
      //if (idx < 0) return m_cp_left_border;
      //if (idx >= m_transfer_points.size()) return m_cp_right_border;
      //return m_transfer_points[idx];

      if (idx < 0) return m_transfer_points[0];
      if (idx >= m_transfer_points.size()) return m_transfer_points.last();
      return m_transfer_points[idx];
    }

    inline TransferControlPoint & leftBorderTCP(void) { return m_transfer_points[0]; }
    inline TransferControlPoint & rightBorderTCP(void) { return m_transfer_points.last(); }

  private:
    tContainer m_transfer_points;
    //TransferControlPoint m_cp_left_border;
    //TransferControlPoint m_cp_right_border;
};

#endif // TRANSFER_FUNCTION_H
