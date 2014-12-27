#ifndef TRANSFER_FUNCTION_EDITOR_H
#define TRANSFER_FUNCTION_EDITOR_H

#include "transfer_function.h"

#include <QWidget>


class TransferFunctionEditor : public QWidget
{
    Q_OBJECT

  private:
    static constexpr int POINT_RENDER_RADIUS = 2;
    static constexpr int POINT_SELECT_RADIUS = 4;
    static constexpr int POINT_SEARCH_RADIUS = 8;
    static constexpr int INNER_PADDING = POINT_SELECT_RADIUS + 3;

  public:
    explicit TransferFunctionEditor(QWidget *parent = 0)
      : QWidget(parent)
      , m_transfer_func()
      , m_cur_tcp_idx(TransferFunction::INVALID_TCP_INDEX)
    {
      setFocusPolicy(Qt::ClickFocus);
      initTest();
    }

  signals:
    void transferFunctionChanged(const TransferFunction & func);

  protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;

    virtual void showEvent(QShowEvent *event) override;

  private:
    void initTest(void)
    {
      setFixedHeight(100);
      m_transfer_func.addTCP(toTCP2(QPoint(15,   0), 256, 100), QColor(Qt::red));
      m_transfer_func.addTCP(toTCP2(QPoint(50,   5), 256, 100), QColor(Qt::green));
      m_transfer_func.addTCP(toTCP2(QPoint(100,  3), 256, 100), QColor(Qt::blue));
      m_transfer_func.addTCP(toTCP2(QPoint(150, 10), 256, 100), QColor(Qt::yellow));
      m_transfer_func.addTCP(toTCP2(QPoint(200,  6), 256, 100), QColor(Qt::magenta));
      m_transfer_func.addTCP(toTCP2(QPoint(256, 50), 256, 100), QColor(Qt::cyan));
    }

    inline QPointF toTCP2(QPoint pt, int w, int h)
    {
      return QPointF(float(pt.x()) / float(w), float(pt.y()) / float(h));
    }

    inline QPointF scaleToTCP(QPoint pt)
    { return scaleToTCP(pt.x(), pt.y()); }

    inline QPointF scaleToTCP(float x, float y)
    {
      return QPointF(x / float(width()  - 2 * INNER_PADDING),
                     y / float(height() - 2 * INNER_PADDING));
    }

    inline QPointF toTCP(QPoint pt)
    {
      return QPointF(float(pt.x() - INNER_PADDING) / float(width()  - 2 * INNER_PADDING),
                     float(pt.y() - INNER_PADDING) / float(height() - 2 * INNER_PADDING));
    }

    inline QPoint fromTCP(QPointF pt)
    {
      return QPoint(pt.x() * (width()  - 2 * INNER_PADDING) + INNER_PADDING,
                    pt.y() * (height() - 2 * INNER_PADDING) + INNER_PADDING);
    }

  private:
    TransferFunction m_transfer_func;
    int m_cur_tcp_idx;
};

#endif // TRANSFER_FUNCTION_EDITOR_H
