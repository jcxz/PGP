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
      : m_pos(), m_col(), m_enabled(true)
    { }

    TransferControlPoint(QPointF pos, QColor col)
      : m_pos(pos), m_col(col), m_enabled(true)
    { }

    bool isEnabled(void) const { return m_enabled; }
    void setEnabled(bool enabled) { m_enabled = enabled; }

    float alpha(void) const { return m_pos.y(); }

    QColor color(void) const { return m_col; }
    void setColor(const QColor & col) { m_col = col; }

    QPointF position(void) const { return m_pos; }
    void setPosition(QPointF pos) { m_pos = pos; }

    friend QDebug operator<<(QDebug debug, const TransferControlPoint & tcp);

    // serialization functions
    friend QDataStream & operator>>(QDataStream & stream, TransferControlPoint & tcp); // input
    friend QDataStream & operator<<(QDataStream & stream, const TransferControlPoint & tcp); // output

  private:
    QPointF m_pos;
    QColor m_col;
    bool m_enabled;
};


class TransferFunction
{
  public:
    typedef QVector<TransferControlPoint> tContainer;

    static constexpr int INVALID_TCP_INDEX = -1;

  private:
    static TransferControlPoint defaultLeftBorderCP(void)
    { return TransferControlPoint(QPointF(0.0f, 0.0f), QColor(Qt::white)); }

    static TransferControlPoint defaultRightBorderCP(void)
    { return TransferControlPoint(QPointF(1.0f, 0.0f), QColor(Qt::black)); }

  public:
    TransferFunction(void)
      : m_transfer_points()
      //, m_cp_left_border(QPointF(0.0f, 0.0f), QColor(Qt::white))
      //, m_cp_right_border(QPointF(1.0f, 0.0f), QColor(Qt::black))
    {
      // dva specialne krajne body, pretoze vo funkcii musia byt vzdy
      // aspon 2 krajne body
      m_transfer_points.push_back(defaultLeftBorderCP());
      m_transfer_points.push_back(defaultRightBorderCP());
    }

    void addTCP(QPointF pos, QColor col)
    { addTCP(TransferControlPoint(pos, col)); }

    void addTCP(const TransferControlPoint & pt)
    {
      m_transfer_points.insert(m_transfer_points.size() - 1, pt);
      //m_transfer_points.push_back(pt);
    }

    void addTCP(QPointF pos);
    void removeTCP(int idx);
    void setTCPPosition(int idx, QPointF pos);
    void setTCPColor(int idx, const QColor & col);
    void setTCPEnabled(int idx, bool enabled);

    void clear(void)
    {
      m_transfer_points.clear();
      m_transfer_points.push_back(defaultLeftBorderCP());
      m_transfer_points.push_back(defaultRightBorderCP());
    }

    bool isBorderPoint(int idx) const
    { return (idx <= 0) || (idx >= (m_transfer_points.size() - 1)); }

    float opacity(float pos_x) const;
    float opacity(int idx, int index_max = 256) const
    { return opacity(float(idx) / float(index_max)); }

    QColor color(float pos_x) const;
    QColor color(int idx, int index_max = 256) const
    { return color(float(idx) / float(index_max)); }

    const tContainer & controlPoints(void) const
    { return m_transfer_points; }

    TransferControlPoint & tcp(int idx)
    {
      //if (idx < 0) return m_cp_left_border;
      //if (idx >= m_transfer_points.size()) return m_cp_right_border;
      //return m_transfer_points[idx];

      if (idx < 0) return m_transfer_points.first();
      if (idx >= m_transfer_points.size()) return m_transfer_points.last();
      return m_transfer_points[idx];
    }

    const TransferControlPoint & tcp(int idx) const
    {
      if (idx < 0) return m_transfer_points.first();
      if (idx >= m_transfer_points.size()) return m_transfer_points.last();
      return m_transfer_points[idx];
    }

    // the mouse_pos and the tolerance have to be transformed to interval [0, 1]
    int findByPosition(QPointF mouse_pos, QPointF tolerance = QPointF(0.0f, 0.0f)) const;

    bool load(const QString & filename);
    bool save(const QString & filename) const;

    friend QDebug operator<<(QDebug debug, const TransferFunction & tf);

    // serialization functions
    friend QDataStream & operator>>(QDataStream & stream, TransferFunction & tf); // input
    friend QDataStream & operator<<(QDataStream & stream, const TransferFunction & tf); // output

  private:
    // calculate the interpolation parameter t
    float calcT(float pos_x, const TransferControlPoint * & cp1, const TransferControlPoint * & cp2, int *idx = nullptr) const;

    //inline bool isValid(void) const { return !m_transfer_points.isEmpty(); }
    inline bool isValid(void) const { return m_transfer_points.size() >= 3; }

    inline TransferControlPoint & leftBorderTCP(void) { return m_transfer_points[0]; }
    inline TransferControlPoint & rightBorderTCP(void) { return m_transfer_points.last(); }

  private:
    tContainer m_transfer_points;
    //TransferControlPoint m_cp_left_border;
    //TransferControlPoint m_cp_right_border;
};

#endif // TRANSFER_FUNCTION_H
